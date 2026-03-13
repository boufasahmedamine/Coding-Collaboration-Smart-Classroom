#include "services/automation/lighting_logic.h"

LightingLogic::LightingLogic(Lighting& lights, OccupancyLogic& occupancy, LDRDriver& ldr)
    : _lights(lights), _occupancy(occupancy), _ldr(ldr), _lastState(false)
{
}

void LightingLogic::update()
{
    bool presence = _occupancy.isOccupied();
    int lightLevel = _ldr.getLightLevel();
    
    // Automation Rule: Lights ON if presence detected AND it is dark enough
    bool shouldBeOn = presence && (lightLevel < LIGHT_THRESHOLD);
    
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