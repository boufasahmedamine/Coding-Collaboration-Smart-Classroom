#include "services/automation/lighting_logic.h"

LightingLogic::LightingLogic(Lighting& lights, PresenceService& presence, StateMachine& stateMachine)
    : _lights(lights), _presence(presence), _stateMachine(stateMachine), 
      _lastState(false), _sessionStickyOn(false)
{
}

void LightingLogic::update()
{
    bool sessionActive = _stateMachine.isSessionActive();
    bool presence = _presence.isOccupied();

    // Session-Sticky Logic:
    // Once presence is detected during a session, lights stay ON for the duration.
    if (!sessionActive) {
        _sessionStickyOn = false;
    } else {
        if (presence) {
            _sessionStickyOn = true;
        }
    }

    bool shouldBeOn = _sessionStickyOn;
    
    if (shouldBeOn != _lastState)
    {
        if (shouldBeOn)
        {
            _lights.turnOn();
        }
        else
        {
            _lights.turnOff();
        }
        _lastState = shouldBeOn;
    }
}