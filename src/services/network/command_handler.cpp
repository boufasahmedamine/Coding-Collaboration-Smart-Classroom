#include "services/network/command_handler.h"
#include "services/auth/auth_proxy.h"
#include "config/mqtt_config.h"
#include <Arduino.h>
#include <string.h>

CommandHandler::CommandHandler(StateMachine* sm, TimeService* ts, Lighting* lights, IRProjector* projector, AuthProxy* auth)
    : _stateMachine(sm), _timeService(ts), _lights(lights), _projector(projector), _auth(auth)
{
}

void CommandHandler::handleCommand(const char* payload, const char* topic)
{
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, payload);

    if (error == DeserializationError::Ok) {
        // --- Structured JSON Command (Commands Topic) ---
        if (strstr(topic, "/commands")) {
            if (doc["command"].isNull() || doc["value"].isNull()) {
                Serial.println("[CMD] Rejected: Missing command or value key");
                return;
            }

            const char* cmd = doc["command"];
            
            // 🟠 Semantic Validation: Check if value is actually a boolean
            if (!doc["value"].is<bool>()) {
                Serial.printf("[CMD] Rejected: Command '%s' requires a boolean value\n", cmd);
                return;
            }

            bool val = doc["value"].as<bool>();

            if (strcmp(cmd, "lights") == 0) {
                if (_lights) val ? _lights->turnOn() : _lights->turnOff();
            } else if (strcmp(cmd, "projector") == 0) {
                if (_projector) val ? _projector->turnOn() : _projector->turnOff();
            } else {
                Serial.printf("[CMD] Ignored: Unknown command '%s'\n", cmd);
            }
        }
        // --- Structured Access Response (Delegated to AuthProxy for Correlation) ---
        else if (strstr(topic, "/access/response")) {
            if (_auth) {
                _auth->handleResponse(doc);
            } else {
                Serial.println("[CMD] Error: AuthProxy not wired to CommandHandler");
            }
        }
    } 
    else {
        // --- Legacy / Insecure Plain String Commands (Deprecation Warning) ---
        Serial.println("[CMD] WARNING: Processing legacy plain-string command. These are insecure and will be removed.");
        
        if (strcmp(payload, "unlock") == 0) {
            if (_stateMachine) _stateMachine->handleEvent(StateMachine::SystemEvent::ACCESS_GRANTED);
        } else if (strncmp(payload, "time_sync:", 10) == 0) {
            uint32_t epoch = strtoul(payload + 10, NULL, 10);
            if (_timeService) _timeService->syncTime(epoch);
        } else {
            Serial.printf("[CMD] Rejected: Malformed JSON or invalid legacy string '%s'\n", payload);
        }
    }
}
