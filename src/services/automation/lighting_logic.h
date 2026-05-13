#ifndef LIGHTING_LOGIC_H
#define LIGHTING_LOGIC_H

#include "drivers/actuators/lighting.h"
#include "services/automation/presence_service.h"
#include "system/state_machine.h"
#include "config/automation_config.h"

class LightingLogic
{
public:
    LightingLogic(Lighting& lights, PresenceService& presence, StateMachine& stateMachine);
    void update();
    void toggleManualOverride();

private:
    Lighting& _lights;
    PresenceService& _presence;
    StateMachine& _stateMachine;
    
    bool _lastState;
    bool _sessionStickyOn;
    bool _manualOverride;
};

#endif
