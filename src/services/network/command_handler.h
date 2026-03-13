#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include "system/state_machine.h"

class CommandHandler
{
public:
    CommandHandler(StateMachine* sm);

    void handleCommand(const char* command);

private:
    StateMachine* _stateMachine;
};

#endif
