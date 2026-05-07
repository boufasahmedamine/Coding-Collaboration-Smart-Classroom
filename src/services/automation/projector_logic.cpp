#include "services/automation/projector_logic.h"

ProjectorLogic::ProjectorLogic(IRProjector& projector, StateMachine& sm)
    : _projector(projector), _stateMachine(sm), _lastState(false), _authorized(false)
{
}

void ProjectorLogic::update()
{
    // Projector logic: Active only when session is active, override is NOT active, and teacher authorized it
    bool sessionActive = _stateMachine.isSessionActive();
    bool overrideActive = _stateMachine.isOverrideActive();

    bool shouldBeOn = sessionActive && !overrideActive && _authorized;
    
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

    // Reset authorization when session ends
    if (!sessionActive) {
        _authorized = false;
    }
}