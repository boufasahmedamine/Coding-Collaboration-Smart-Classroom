#include "services/network/command_handler.h"
#include <Arduino.h>
#include <string.h>

CommandHandler::CommandHandler(StateMachine* sm, TimeService* ts)
    : _stateMachine(sm), _timeService(ts)
{
}

void CommandHandler::handleCommand(const char* command)
{
    Serial.print("[MQTT CMD] Received: ");
    Serial.println(command);

    if (strcmp(command, "access_granted") == 0 || strcmp(command, "unlock") == 0)
    {
        Serial.println("[MQTT CMD] Access Granted");
        if (_stateMachine) _stateMachine->handleEvent(StateMachine::SystemEvent::ACCESS_GRANTED);
    }
    else if (strcmp(command, "access_denied") == 0)
    {
        Serial.println("[MQTT CMD] Access Denied");
        if (_stateMachine) _stateMachine->handleEvent(StateMachine::SystemEvent::ACCESS_DENIED);
    }
    else if (strncmp(command, "time_sync:", 10) == 0)
    {
        uint32_t epoch = strtoul(command + 10, NULL, 10);
        Serial.printf("[MQTT CMD] Time Sync: %u\n", epoch);
        if (_timeService) _timeService->syncTime(epoch);
    }
    else
    {
        Serial.print("[MQTT CMD] Unknown command: ");
        Serial.println(command);
    }
}
