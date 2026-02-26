#include "system/state_machine.h"
#include <Arduino.h>

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

    // Exit logic (none for now)

    // Transition
    _currentState = newState;

    // Entry logic
    switch (_currentState) {

        case State::SESSION_ACTIVE:
            _doorLock.unlock();
            _sessionStartTime = millis();
            break;

        case State::LOCKED:
            // Locking remains handled externally (preserve behavior)
            break;

        default:
            break;
    }
}

void StateMachine::handleEvent(SystemEvent event) {
    switch (event) {

        case SystemEvent::ACCESS_GRANTED:
            if (_currentState == State::LOCKED) {
                transitionTo(State::SESSION_ACTIVE);
            }
            break;

        case SystemEvent::OVERRIDE_ON:
            _overrideActive = true;
            if (_currentState == State::LOCKED) {
                transitionTo(State::SESSION_ACTIVE);
            }
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