#include "drivers/actuators/lighting.h"
#include <Arduino.h>

Lighting::Lighting(int pin)
    : _pin(pin), _isOn(false)
{
}

void Lighting::begin()
{
    pinMode(_pin, OUTPUT);
    turnOff();
}

void Lighting::turnOn()
{
    digitalWrite(_pin, HIGH);
    _isOn = true;
    Serial.println("[LIGHTS] ON");
}

void Lighting::turnOff()
{
    digitalWrite(_pin, LOW);
    _isOn = false;
    Serial.println("[LIGHTS] OFF");
}