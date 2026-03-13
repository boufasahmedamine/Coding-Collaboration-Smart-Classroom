#ifndef LIGHT_SERVICE_H
#define LIGHT_SERVICE_H

#include "drivers/ldr/ldr_driver.h"

enum LightLevel
{
    LIGHT_DARK,
    LIGHT_NORMAL,
    LIGHT_BRIGHT
};

class LightService
{
public:
    LightService(LDRDriver* driver);

    void update();

    LightLevel getLightLevel();

    bool isDark();

private:
    LDRDriver* _driver;

    LightLevel _level;
};

#endif
