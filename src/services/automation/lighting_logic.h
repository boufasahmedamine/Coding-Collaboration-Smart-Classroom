#ifndef LIGHTING_LOGIC_H
#define LIGHTING_LOGIC_H

#include "drivers/actuators/lighting.h"
#include "services/automation/presence_service.h"
#include "drivers/ldr/ldr_driver.h"
#include "config/automation_config.h"

class LightingLogic
{
public:
    LightingLogic(Lighting& lights, PresenceService& presence, LDRDriver& ldr);
    void update();

private:
    Lighting& _lights;
    PresenceService& _presence;
    LDRDriver& _ldr;
    bool _lastState;
};

#endif
