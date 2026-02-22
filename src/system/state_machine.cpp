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
    _currentState = State::LOCKED;
}

void StateMachine::requestSessionStart() {
    if (_currentState == State::LOCKED) {
        _doorLock.unlock();
        _sessionStartTime = millis();
        _currentState = State::SESSION_UNLOCKED;
    }
}

void StateMachine::update() {
    if (_currentState == State::SESSION_UNLOCKED) {

        // Safety override always keeps it unlocked
        if (_overrideActive) {
            return;
        }

        unsigned long currentTime = millis();

        if (currentTime - _sessionStartTime >= _sessionDurationMs) {

            // Future: only lock if no presence
            if (!_presenceDetected) {
                _doorLock.lock();
                _currentState = State::LOCKED;
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
        _doorLock.unlock();
        _currentState = State::SESSION_UNLOCKED;
    }
}