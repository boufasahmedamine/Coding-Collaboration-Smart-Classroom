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
    const int samples = 10;
    long total = 0;

    for (int i = 0; i < samples; i++)
    {
        total += analogRead(_pin);
        delayMicroseconds(200);
    }

    int raw = total / samples;

    // Trick 2: temporal low-pass filter
    _lightLevel = (_lightLevel * 3 + raw) / 4;
}

int LDRDriver::getLightLevel()
{
    return _lightLevel;
}
