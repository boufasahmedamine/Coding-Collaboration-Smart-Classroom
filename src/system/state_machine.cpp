#include "system/state_machine.h"
#include "system/diagnostics.h"
#include <Arduino.h>

//#define DEBUG_STATE_MACHINE

static const char* stateToString(StateMachine::SystemState state) {
    switch (state) {
        case StateMachine::SystemState::LOCKED: return "LOCKED";
        case StateMachine::SystemState::WAITING_FOR_AUTH: return "WAITING_FOR_AUTH";
        case StateMachine::SystemState::UNLOCKED: return "UNLOCKED";
        default: return "UNKNOWN";
    }
}

StateMachine::StateMachine(DoorLockDriver& doorLock, unsigned long sessionTimeoutMs)
    : _doorLock(doorLock),
      _currentState(SystemState::LOCKED),
      _sessionStartTime(0),
      _sessionDurationMs(sessionTimeoutMs),
      _presenceDetected(false),
      _overrideActive(false)
{
    _session.active = false;
    _session.uidLength = 0;
    _session.startTime = 0;
    _session.endTime = 0;
    for (int i = 0; i < 7; i++) {
        _session.uid[i] = 0;
    }
}

void StateMachine::init() {
    _doorLock.begin();
    _session.active = false;
    transitionTo(SystemState::LOCKED);
}

void StateMachine::transitionTo(SystemState newState) {
    if (_currentState == newState) {
        return;
    }

#ifdef DEBUG_STATE_MACHINE
    SystemState oldState = _currentState;
#endif

    _currentState = newState;

    Diagnostics::setSystemState(stateToString(_currentState));

#ifdef DEBUG_STATE_MACHINE
    Serial.print("[SM] ");
    Serial.print(stateToString(oldState));
    Serial.print(" -> ");
    Serial.println(stateToString(_currentState));
#endif

    switch (_currentState) {

        case SystemState::UNLOCKED:
            _doorLock.unlock();
            _sessionStartTime = millis();
            break;

        case SystemState::LOCKED:
            break;

        default:
            break;
    }
}

void StateMachine::handleEvent(SystemEvent event, const uint8_t* uid, uint8_t uidLength) {

    switch (_currentState) {

        case SystemState::LOCKED:
            handleLockedState(event, uid, uidLength);
            break;

        case SystemState::WAITING_FOR_AUTH:
            // Forward events to a new handler for the waiting state
            switch (event) {
                case SystemEvent::ACCESS_GRANTED:
                    _session.startTime = millis();
                    _session.active = true;
                    // UID should have been stored already or passed here
                    transitionTo(SystemState::UNLOCKED);
                    break;
                case SystemEvent::ACCESS_DENIED:
                    transitionTo(SystemState::LOCKED);
                    Diagnostics::logEvent("SM: Access Denied by Server");
                    break;
                default:
                    break;
            }
            break;

        case SystemState::UNLOCKED:
            handleSessionActiveState(event);
            break;

        default:
            break;
    }
}

void StateMachine::handleLockedState(SystemEvent event, const uint8_t* uid, uint8_t uidLength) {

    switch (event) {

        case SystemEvent::RFID_READ:
            if (uid != nullptr && uidLength > 0) {
                _session.uidLength = uidLength;
                memcpy(_session.uid, uid, uidLength);
            }
            transitionTo(SystemState::WAITING_FOR_AUTH);
            break;

        case SystemEvent::ACCESS_GRANTED:
            _session.startTime = millis();
            _session.active = true;
            if (uid != nullptr && uidLength > 0) {
                _session.uidLength = uidLength;
                memcpy(_session.uid, uid, uidLength);
            }
            transitionTo(SystemState::UNLOCKED);
            break;

        case SystemEvent::UNLOCK_REQUEST:
            Diagnostics::logEvent("SM: Remote Unlock Request processed");
            _session.startTime = millis();
            _session.uidLength = 0; // Remote unlock might have no UID
            _session.active = true;
            _session.loggedStart = false;
            _session.loggedEnd = false;
            transitionTo(SystemState::UNLOCKED);
            break;

        case SystemEvent::OVERRIDE_ON:
            _overrideActive = true;
            _session.startTime = millis();
            _session.active = true;
            _session.loggedStart = false; // Override also starts a session, so initialize flags
            _session.loggedEnd = false;
            transitionTo(SystemState::UNLOCKED);
            break;

        case SystemEvent::OVERRIDE_OFF:
            _overrideActive = false;
            break;

        case SystemEvent::PRESENCE_DETECTED:
            _presenceDetected = true;
            break;

        case SystemEvent::PRESENCE_LOST:
            _presenceDetected = false;
            break;

        default:
            break;
    }
}

void StateMachine::handleSessionActiveState(SystemEvent event) {

    switch (event) {

        case SystemEvent::OVERRIDE_ON:
            _overrideActive = true;
            break;

        case SystemEvent::OVERRIDE_OFF:
            _overrideActive = false;
            break;

        case SystemEvent::PRESENCE_DETECTED:
            _presenceDetected = true;
            break;

        case SystemEvent::PRESENCE_LOST:
            _presenceDetected = false;
            break;

        default:
            break;
    }
}

/*
Timeout Semantics (Phase 7 Formalization)

1. A session begins when entering SESSION_ACTIVE.
2. The session duration is fixed at _sessionDurationMs.
3. Timeout condition is evaluated during update().
4. Lock transition occurs when:
    - State == SESSION_ACTIVE
    - Override is NOT active
    - Elapsed time >= session duration
    - Presence is NOT detected
5. If presence is detected at expiration time,
   the session remains active until presence is lost.
6. If override is active at expiration time,
   the session remains active until override is disabled.
7. The session timer is NOT reset by presence or override.
8. Lock occurs immediately once all blocking conditions are cleared.
*/
void StateMachine::update() {
    if (_currentState == SystemState::UNLOCKED) {

        // Safety override always keeps it unlocked
        if (_overrideActive) {
            return;
        }

        unsigned long currentTime = millis();

        if (currentTime - _sessionStartTime >= _sessionDurationMs) {

            // Future: only lock if no presence
            if (!_presenceDetected) {
                _session.endTime = millis();
                _session.active = false;

                _doorLock.lock();
                transitionTo(SystemState::LOCKED);
                Diagnostics::logEvent("SESSION: Timeout reached. Locking door.");
            }
        }
    }
}

StateMachine::SystemState StateMachine::getState() const
{
    return _currentState;
}

SessionRecord& StateMachine::getCurrentSession()
{
    return _session;
}

void StateMachine::setPresenceDetected(bool detected) {
    _presenceDetected = detected;
}

bool StateMachine::isSessionActive() const {
    return _session.active;
}

bool StateMachine::isOverrideActive() const {
    return _overrideActive;
}

void StateMachine::setOverrideActive(bool active) {
    _overrideActive = active;

    // Immediate unlock if override activated
    if (_overrideActive && _currentState == SystemState::LOCKED) {
        transitionTo(SystemState::UNLOCKED);
    }
}