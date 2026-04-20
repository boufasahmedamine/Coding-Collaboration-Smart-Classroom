#include "drivers/pir/pir_driver.h"
#include <Arduino.h>

PIRDriver::PIRDriver(int pin)
    : _pin(pin), _motion(false)
{
}

void PIRDriver::begin()
{
    pinMode(_pin, INPUT);
    Serial.println("[PIR] Driver initialized");
}

void PIRDriver::update()
{
    // Basic digital read for PIR sensor
    // PIR sensors typically stay HIGH for a few seconds after motion
    _motion = (digitalRead(_pin) == HIGH);
}

bool PIRDriver::isMotionDetected()
{
    return _motion;
}
