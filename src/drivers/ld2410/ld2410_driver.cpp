#include "drivers/ld2410/ld2410_driver.h"
#include <Arduino.h>

LD2410Driver::LD2410Driver(int rxPin, int txPin)
    : _rxPin(rxPin), _txPin(txPin), _presence(false)
{
}

void LD2410Driver::begin()
{
    Serial.println("[LD2410] Driver initialized");
}

void LD2410Driver::update()
{
    // Simulation until hardware arrives
    if (Serial.available())
    {
        char c = Serial.peek(); // Using peek so main.cpp can still process 'c' and 'u' if needed, or we just consume if it is 'p' or 'n'
        
        if (c == 'p')
        {
            Serial.read(); // consume
            _presence = true;
            Serial.println("[LD2410] Presence detected");
        }
        else if (c == 'n')
        {
            Serial.read(); // consume
            _presence = false;
            Serial.println("[LD2410] No presence");
        }
    }
}

bool LD2410Driver::isPresenceDetected()
{
    return _presence;
}
