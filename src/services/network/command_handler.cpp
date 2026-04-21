#include "services/network/command_handler.h"
#include <Arduino.h>
#include <string.h>

#include "services/network/command_handler.h"
#include "config/mqtt_config.h"
#include <Arduino.h>
#include <string.h>

CommandHandler::CommandHandler(StateMachine* sm, TimeService* ts, Lighting* lights, IRProjector* projector)
    : _stateMachine(sm), _timeService(ts), _lights(lights), _projector(projector)
{
}

void CommandHandler::handleCommand(const char* payload, const char* topic)
{
    Serial.printf("[MQTT CMD] Topic: %s | Payload: %s\n", topic, payload);

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, payload);

    if (error == DeserializationError::Ok) {
        // --- Structured JSON Command (Commands Topic) ---
        if (strstr(topic, "/commands")) {
            const char* command = doc["command"];
            bool value = doc["value"];

            if (command && strcmp(command, "lights") == 0) {
                if (_lights) value ? _lights->turnOn() : _lights->turnOff();
            } else if (command && strcmp(command, "projector") == 0) {
                if (_projector) value ? _projector->turnOn() : _projector->turnOff();
            }
        }
        // --- Structured Access Response ---
        else if (strstr(topic, "/access/response")) {
            bool approved = doc["approved"];
            if (approved) {
                if (_stateMachine) _stateMachine->handleEvent(StateMachine::SystemEvent::ACCESS_GRANTED);
            } else {
                if (_stateMachine) _stateMachine->handleEvent(StateMachine::SystemEvent::ACCESS_DENIED);
            }
        }
    } 
    else {
        // --- Legacy / Plain String Commands ---
        if (strcmp(payload, "access_granted") == 0 || strcmp(payload, "unlock") == 0) {
            if (_stateMachine) _stateMachine->handleEvent(StateMachine::SystemEvent::ACCESS_GRANTED);
        } else if (strcmp(payload, "access_denied") == 0) {
            if (_stateMachine) _stateMachine->handleEvent(StateMachine::SystemEvent::ACCESS_DENIED);
        } else if (strncmp(payload, "time_sync:", 10) == 0) {
            uint32_t epoch = strtoul(payload + 10, NULL, 10);
            if (_timeService) _timeService->syncTime(epoch);
        }
    }
}
