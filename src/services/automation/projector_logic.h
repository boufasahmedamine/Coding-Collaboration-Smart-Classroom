#ifndef PROJECTOR_LOGIC_H
#define PROJECTOR_LOGIC_H

#include "drivers/actuators/ir_projector.h"
#include "system/state_machine.h"
#include "services/automation/presence_service.h"

class ProjectorLogic
{
public:
    ProjectorLogic(IRProjector& projector, StateMachine& sm, PresenceService& presence);
    void update();

private:
    IRProjector& _projector;
    StateMachine& _stateMachine;
    PresenceService& _presence;
    bool _lastState;
};

#endif
