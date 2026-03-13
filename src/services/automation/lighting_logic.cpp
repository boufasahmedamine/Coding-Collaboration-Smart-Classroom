#include "services/automation/lighting_logic.h"

LightingLogic::LightingLogic(Lighting& lights, OccupancyLogic& occupancy)
    : _lights(lights), _occupancy(occupancy), _lastState(false)
{
}

void LightingLogic::update()
{
    bool currentlyOccupied = _occupancy.isOccupied();
    
    if (currentlyOccupied != _lastState)
    {
        if (currentlyOccupied)
        {
            _lights.turnOn();
        }
        else
        {
            _lights.turnOff();
        }
        _lastState = currentlyOccupied;
    }
}