#ifndef LIGHTING_LOGIC_H
#define LIGHTING_LOGIC_H

#include "drivers/actuators/lighting.h"
#include "services/automation/occupancy_logic.h"
#include "drivers/ldr/ldr_driver.h"
#include "config/automation_config.h"

class LightingLogic
{
public:
    LightingLogic(Lighting& lights, OccupancyLogic& occupancy, LDRDriver& ldr);
    void update();

private:
    Lighting& _lights;
    OccupancyLogic& _occupancy;
    LDRDriver& _ldr;
    bool _lastState;
};

#endif
