#ifndef PROJECTOR_LOGIC_H
#define PROJECTOR_LOGIC_H

#include "drivers/actuators/ir_projector.h"
#include "system/state_machine.h"
#include "services/automation/occupancy_logic.h"

class ProjectorLogic
{
public:
    ProjectorLogic(IRProjector& projector, StateMachine& sm, OccupancyLogic& occupancy);
    void update();

private:
    IRProjector& _projector;
    StateMachine& _stateMachine;
    OccupancyLogic& _occupancy;
    bool _lastState;
};

#endif
