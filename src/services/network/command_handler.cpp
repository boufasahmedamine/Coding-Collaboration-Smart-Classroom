#include "services/network/command_handler.h"
#include <Arduino.h>
#include <string.h>

CommandHandler::CommandHandler(StateMachine* sm)
    : _stateMachine(sm)
{
}

void CommandHandler::handleCommand(const char* command)
{
    Serial.print("[MQTT CMD] Received: ");
    Serial.println(command);

    if (strcmp(command, "unlock") == 0)
    {
        Serial.println("[MQTT CMD] Unlock command recognized");

        if (_stateMachine)
        {
            _stateMachine->handleEvent(StateMachine::SystemEvent::UNLOCK_REQUEST);
        }
    }
    else
    {
        Serial.print("[MQTT CMD] Unknown command: ");
        Serial.println(command);
    }
}
