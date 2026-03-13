#ifndef PROJECTOR_LOGIC_H
#define PROJECTOR_LOGIC_H

#include "drivers/actuators/ir_projector.h"
#include "system/state_machine.h"

class ProjectorLogic
{
public:
    ProjectorLogic(IRProjector& projector, StateMachine& sm);
    void update();

private:
    IRProjector& _projector;
    StateMachine& _stateMachine;
    bool _lastState;
};

#endif
