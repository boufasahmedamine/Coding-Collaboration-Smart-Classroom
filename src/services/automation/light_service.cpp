#include "services/automation/light_service.h"

LightService::LightService(LDRDriver* driver)
    : _driver(driver), _level(LIGHT_NORMAL)
{
}

void LightService::update()
{
    int value = _driver->getLightLevel();

    if (value < 800)
    {
        _level = LIGHT_DARK;
    }
    else if (value < 2500)
    {
        _level = LIGHT_NORMAL;
    }
    else
    {
        _level = LIGHT_BRIGHT;
    }
}

LightLevel LightService::getLightLevel()
{
    return _level;
}

bool LightService::isDark()
{
    return _level == LIGHT_DARK;
}
