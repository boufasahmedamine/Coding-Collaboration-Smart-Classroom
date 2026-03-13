#include "services/automation/projector_logic.h"

ProjectorLogic::ProjectorLogic(IRProjector& projector, StateMachine& sm, OccupancyLogic& occupancy)
    : _projector(projector), _stateMachine(sm), _occupancy(occupancy), _lastState(false)
{
}

void ProjectorLogic::update()
{
    // Safety Rule: Activated only when session is active, override is NOT active, and room is occupied
    bool sessionActive = _stateMachine.isSessionActive();
    bool overrideActive = _stateMachine.isOverrideActive();
    bool occupied = _occupancy.isOccupied();

    bool shouldBeOn = sessionActive && !overrideActive && occupied;
    
    if (shouldBeOn != _lastState)
    {
        if (shouldBeOn)
        {
            _projector.turnOn();
        }
        else
        {
            _projector.turnOff();
        }
        _lastState = shouldBeOn;
    }
}