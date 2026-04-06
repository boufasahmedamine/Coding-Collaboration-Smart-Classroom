#include "drivers/ld2410/ld2410_driver.h"
#include <Arduino.h>

LD2410Driver::LD2410Driver(int rxPin, int txPin)
    : _rxPin(rxPin), _txPin(txPin), _presence(false), _simQueue(NULL)
{
}

void LD2410Driver::setSimQueue(QueueHandle_t q)
{
    _simQueue = q;
}

void LD2410Driver::begin()
{
    Serial.println("[LD2410] Driver initialized");
}

void LD2410Driver::update()
{
    // Simulation until hardware arrives
    if (_simQueue != NULL)
    {
        char command;
        // Peek at the queue without consuming to see if it's for us
        if (xQueuePeek(_simQueue, &command, 0) == pdTRUE)
        {
            if (command == 'p')
            {
                xQueueReceive(_simQueue, &command, 0); // Consume
                _presence = true;
                // Note: Serial.println here should ideally be protected by a Mutex in a real deployment
                // We will wrap Serial calls in the router or main loop.
                Serial.println("[LD2410] Presence detected (Queue)");
            }
            else if (command == 'n')
            {
                xQueueReceive(_simQueue, &command, 0); // Consume
                _presence = false;
                Serial.println("[LD2410] No presence (Queue)");
            }
        }
    }
}

bool LD2410Driver::isPresenceDetected()
{
    return _presence;
}
