#include "drivers/actuators/lighting.h"
#include <Arduino.h>

Lighting::Lighting(int pin) : _pin(pin) {}

void Lighting::begin()
{
    pinMode(_pin, OUTPUT);
    turnOff();
}

void Lighting::turnOn()
{
    digitalWrite(_pin, HIGH);
    Serial.println("[LIGHTS] ON");
}

void Lighting::turnOff()
{
    digitalWrite(_pin, LOW);
    Serial.println("[LIGHTS] OFF");
}