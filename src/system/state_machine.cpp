#include "system/state_machine.h"
#include <Arduino.h>

//#define DEBUG_STATE_MACHINE

#ifdef DEBUG_STATE_MACHINE
static const char* stateToString(StateMachine::State state) {
    switch (state) {
        case StateMachine::State::LOCKED: return "LOCKED";
        case StateMachine::State::SESSION_ACTIVE: return "SESSION_ACTIVE";
        default: return "UNKNOWN";
    }
}
#endif

StateMachine::StateMachine(DoorLock& doorLock, unsigned long sessionDurationMs)
    : _doorLock(doorLock),
      _currentState(State::LOCKED),
      _sessionStartTime(0),
      _sessionDurationMs(sessionDurationMs),
      _presenceDetected(false),
      _overrideActive(false)
{
}

void StateMachine::init() {
    _doorLock.init();
    transitionTo(State::LOCKED);
}

void StateMachine::transitionTo(State newState) {
    if (_currentState == newState) {
        return;
    }

#ifdef DEBUG_STATE_MACHINE
    State oldState = _currentState;
#endif

    _currentState = newState;

#ifdef DEBUG_STATE_MACHINE
    Serial.print("[SM] ");
    Serial.print(stateToString(oldState));
    Serial.print(" -> ");
    Serial.println(stateToString(_currentState));
#endif

    switch (_currentState) {

        case State::SESSION_ACTIVE:
            _doorLock.unlock();
            _sessionStartTime = millis();
            break;

        case State::LOCKED:
            break;

        default:
            break;
    }
}

void StateMachine::handleEvent(SystemEvent event) {

    switch (_currentState) {

        case State::LOCKED:
            handleLockedState(event);
            break;

        case State::SESSION_ACTIVE:
            handleSessionActiveState(event);
            break;

        default:
            break;
    }
}

void StateMachine::handleLockedState(SystemEvent event) {

    switch (event) {

        case SystemEvent::ACCESS_GRANTED:
            transitionTo(State::SESSION_ACTIVE);
            break;

        case SystemEvent::OVERRIDE_ON:
            _overrideActive = true;
            transitionTo(State::SESSION_ACTIVE);
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
    if (_currentState == State::SESSION_ACTIVE) {

        // Safety override always keeps it unlocked
        if (_overrideActive) {
            return;
        }

        unsigned long currentTime = millis();

        if (currentTime - _sessionStartTime >= _sessionDurationMs) {

            // Future: only lock if no presence
            if (!_presenceDetected) {
                _doorLock.lock();
                transitionTo(State::LOCKED);
            }
        }
    }
}

StateMachine::State StateMachine::getState() const {
    return _currentState;
}

void StateMachine::setPresenceDetected(bool detected) {
    _presenceDetected = detected;
}

void StateMachine::setOverrideActive(bool active) {
    _overrideActive = active;

    // Immediate unlock if override activated
    if (_overrideActive && _currentState == State::LOCKED) {
        transitionTo(State::SESSION_ACTIVE);
    }
}