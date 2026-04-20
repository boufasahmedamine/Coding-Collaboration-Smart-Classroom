#include "services/automation/projector_logic.h"

ProjectorLogic::ProjectorLogic(IRProjector& projector, StateMachine& sm, PresenceService& presence)
    : _projector(projector), _stateMachine(sm), _presence(presence), _lastState(false)
{
}

void ProjectorLogic::update()
{
    // Safety Rule: Activated only when session is active, override is NOT active, and room is occupied
    bool sessionActive = _stateMachine.isSessionActive();
    bool overrideActive = _stateMachine.isOverrideActive();
    bool occupied = _presence.isOccupied();

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