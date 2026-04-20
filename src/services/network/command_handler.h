#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include "system/state_machine.h"
#include "services/network/time_service.h"

class CommandHandler
{
public:
    CommandHandler(StateMachine* sm, TimeService* ts);

    void handleCommand(const char* command);

private:
    StateMachine* _stateMachine;
    TimeService* _timeService;
};

#endif
