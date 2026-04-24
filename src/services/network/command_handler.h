#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include "system/state_machine.h"
#include "services/network/time_service.h"
#include "drivers/actuators/lighting.h"
#include "drivers/actuators/ir_projector.h"
#include <ArduinoJson.h>

class AuthProxy;

class CommandHandler
{
public:
    CommandHandler(StateMachine* sm, TimeService* ts, Lighting* lights, IRProjector* projector, AuthProxy* auth = nullptr);

    /**
     * Parses incoming MQTT payloads (JSON or Plain String) and executes the logic.
     */
    void handleCommand(const char* payload, const char* topic);

private:
    StateMachine* _stateMachine;
    TimeService* _timeService;
    Lighting* _lights;
    IRProjector* _projector;
    AuthProxy* _auth;
};

#endif
