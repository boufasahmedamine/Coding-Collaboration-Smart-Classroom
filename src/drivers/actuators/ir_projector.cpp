#include "drivers/actuators/ir_projector.h"
#include <Arduino.h>

IRProjector::IRProjector(int pin) : _pin(pin), _isOn(false) {}

void IRProjector::begin()
{
    pinMode(_pin, OUTPUT);
    turnOff();
}

void IRProjector::turnOn()
{
    digitalWrite(_pin, HIGH);
    _isOn = true;
    Serial.println("[PROJECTOR] ON");
}

void IRProjector::turnOff()
{
    digitalWrite(_pin, LOW);
    _isOn = false;
    Serial.println("[PROJECTOR] OFF");
}