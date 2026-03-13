#include "services/automation/automation_controller.h"
#include <Arduino.h>

AutomationController::AutomationController(PresenceService* presence, LightService* light)
    : _presence(presence),
      _light(light),
      _lightsOn(false)
{
}

void AutomationController::update()
{
    bool occupied = _presence->isOccupied();
    bool dark = _light->isDark();

    if (occupied && dark && !_lightsOn)
    {
        _lightsOn = true;
        Serial.println("[AUTO] Lights ON (occupied + dark)");
    }

    if ((!occupied || !dark) && _lightsOn)
    {
        _lightsOn = false;
        Serial.println("[AUTO] Lights OFF");
    }
}
