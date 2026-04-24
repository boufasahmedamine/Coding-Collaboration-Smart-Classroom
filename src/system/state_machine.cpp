#include "system/state_machine.h"
#include "system/diagnostics.h"
#include "config/mqtt_config.h"
#include "communication/mqtt_manager.h"
#include <ArduinoJson.h>
#include <Arduino.h>

static const char* stateToString(StateMachine::SystemState state) {
    switch (state) {
        case StateMachine::SystemState::LOCKED: return "LOCKED";
        case StateMachine::SystemState::WAITING_FOR_AUTH: return "WAITING_FOR_AUTH";
        case StateMachine::SystemState::UNLOCKED: return "UNLOCKED";
        default: return "UNKNOWN";
    }
}

StateMachine::StateMachine(DoorLockDriver& doorLock, unsigned long sessionTimeoutMs, MQTTManager* mqtt)
    : _doorLock(doorLock),
      _currentState(SystemState::LOCKED),
      _sessionStartTime(0),
      _sessionDurationMs(sessionTimeoutMs),
      _authStartTime(0),
      _authTimeoutMs(AUTH_TIMEOUT_MS),
      _presenceDetected(false),
      _overrideActive(false),
      _mqtt(mqtt)
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

    _currentState = newState;
    Diagnostics::setSystemState(stateToString(_currentState));

    switch (_currentState) {
        case SystemState::UNLOCKED:
            _doorLock.unlock();
            _sessionStartTime = millis();
            break;

        case SystemState::WAITING_FOR_AUTH:
            _authStartTime = millis();
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
            switch (event) {
                case SystemEvent::RFID_READ:
                    // 🔴 ABORT & REPLACE: Reset timer for the new scan
                    _authStartTime = millis(); 
                    if (uid != nullptr && uidLength > 0) {
                        _session.uidLength = uidLength;
                        memcpy(_session.uid, uid, uidLength);
                    }
                    break;
                case SystemEvent::ACCESS_GRANTED:
                    _session.startTime = millis();
                    _session.active = true;
                    transitionTo(SystemState::UNLOCKED);
                    emitResultEvent("success", "");
                    break;
                case SystemEvent::ACCESS_DENIED:
                    transitionTo(SystemState::LOCKED);
                    Diagnostics::logEvent("SM: Access Denied");
                    emitResultEvent("rejected", "denied_by_server");
                    break;
                case SystemEvent::ACCESS_TIMEOUT:
                    transitionTo(SystemState::LOCKED);
                    Diagnostics::logEvent("SM: Auth Timeout");
                    emitResultEvent("timeout", "server_no_response");
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
            transitionTo(SystemState::UNLOCKED);
            break;

        case SystemEvent::UNLOCK_REQUEST:
            _session.startTime = millis();
            _session.active = true;
            transitionTo(SystemState::UNLOCKED);
            break;

        case SystemEvent::OVERRIDE_ON:
            _overrideActive = true;
            _session.startTime = millis();
            _session.active = true;
            transitionTo(SystemState::UNLOCKED);
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

void StateMachine::update() {
    unsigned long currentTime = millis();

    if (_currentState == SystemState::UNLOCKED) {
        if (_overrideActive) return;

        if (currentTime - _sessionStartTime >= _sessionDurationMs) {
            if (!_presenceDetected) {
                _session.endTime = currentTime;
                _session.active = false;
                _doorLock.lock();
                transitionTo(SystemState::LOCKED);
                Diagnostics::logEvent("SESSION: Timeout reached. Locking door.");
            }
        }
    } 
    else if (_currentState == SystemState::WAITING_FOR_AUTH) {
        // 🔴 AUTH TIMEOUT Logic
        if (currentTime - _authStartTime >= _authTimeoutMs) {
            handleEvent(SystemEvent::ACCESS_TIMEOUT);
        }
    }
}

void StateMachine::cancelPendingRequest() {
    if (_currentState == SystemState::WAITING_FOR_AUTH) {
        transitionTo(SystemState::LOCKED);
    }
}

void StateMachine::emitResultEvent(const char* status, const char* reason) {
    if (!_mqtt || !_mqtt->isConnected()) return;

    JsonDocument doc;
    doc["event"] = "access_result";
    doc["classroom_name"] = CLASSROOM_NAME;
    
    JsonObject data = doc["data"].to<JsonObject>();
    data["status"] = status;
    if (strlen(reason) > 0) {
        data["reason"] = reason;
    }

    String output;
    serializeJson(doc, output);
    _mqtt->publish(TOPIC_EVENTS, output.c_str());
}

StateMachine::SystemState StateMachine::getState() const { return _currentState; }
SessionRecord& StateMachine::getCurrentSession() { return _session; }
void StateMachine::setPresenceDetected(bool detected) { _presenceDetected = detected; }
bool StateMachine::isSessionActive() const { return _session.active; }
bool StateMachine::isOverrideActive() const { return _overrideActive; }

void StateMachine::setOverrideActive(bool active) {
    _overrideActive = active;
    if (_overrideActive && _currentState == SystemState::LOCKED) {
        transitionTo(SystemState::UNLOCKED);
    }
}