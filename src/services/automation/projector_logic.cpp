#include "services/automation/projector_logic.h"

ProjectorLogic::ProjectorLogic(IRProjector& projector, StateMachine& sm)
    : _projector(projector), _stateMachine(sm), _lastState(false)
{
}

void ProjectorLogic::update()
{
    bool sessionActive = (_stateMachine.getState() == StateMachine::SystemState::UNLOCKED);
    
    if (sessionActive != _lastState)
    {
        if (sessionActive)
        {
            _projector.turnOn();
        }
        else
        {
            _projector.turnOff();
        }
        _lastState = sessionActive;
    }
}