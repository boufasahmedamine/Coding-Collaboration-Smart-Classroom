#include "services/automation/lighting_logic.h"

LightingLogic::LightingLogic(Lighting& lights, PresenceService& presence, StateMachine& stateMachine)
    : _lights(lights), _presence(presence), _stateMachine(stateMachine), 
      _lastState(false), _sessionStickyOn(false), _manualOverride(false)
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
        _manualOverride = false; // Reset override when session ends for next class
    } else {
        if (presence) {
            _sessionStickyOn = true;
        }
    }

    // Force Light Override Logic
    // Manual override OR sticky session presence
    bool shouldBeOn = _manualOverride || _sessionStickyOn;
    
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

void LightingLogic::toggleManualOverride() {
    _manualOverride = !_manualOverride;
}