#ifndef LIGHTING_LOGIC_H
#define LIGHTING_LOGIC_H

#include "drivers/actuators/lighting.h"
#include "services/automation/occupancy_logic.h"

class LightingLogic
{
public:
    LightingLogic(Lighting& lights, OccupancyLogic& occupancy);
    void update();

private:
    Lighting& _lights;
    OccupancyLogic& _occupancy;
    bool _lastState;
};

#endif
