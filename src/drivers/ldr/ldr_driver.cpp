#include "drivers/ldr/ldr_driver.h"
#include <Arduino.h>

LDRDriver::LDRDriver(int pin)
    : _pin(pin), _lightLevel(0)
{
}

void LDRDriver::begin()
{
    pinMode(_pin, INPUT);
    Serial.println("[LDR] Driver initialized");
}

void LDRDriver::update()
{
    _lightLevel = analogRead(_pin);
}

int LDRDriver::getLightLevel()
{
    return _lightLevel;
}
