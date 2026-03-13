#include "drivers/actuators/ir_projector.h"
#include <Arduino.h>

IRProjector::IRProjector(int pin) : _pin(pin) {}

void IRProjector::begin()
{
    pinMode(_pin, OUTPUT);
    turnOff();
}

void IRProjector::turnOn()
{
    digitalWrite(_pin, HIGH);
    Serial.println("[PROJECTOR] ON");
}

void IRProjector::turnOff()
{
    digitalWrite(_pin, LOW);
    Serial.println("[PROJECTOR] OFF");
}