#include "drivers/ld2410/ld2410_driver.h"
#include <Arduino.h>

extern SemaphoreHandle_t xMutex_Serial;

LD2410Driver::LD2410Driver(int rxPin, int txPin)
    : _rxPin(rxPin), _txPin(txPin), _presence(false), _initialized(false), _simQueue(NULL)
{
}

void LD2410Driver::setSimQueue(QueueHandle_t q)
{
    _simQueue = q;
}

void LD2410Driver::begin()
{
    // Initialize Hardware Serial 2 for ESP32
    Serial2.begin(256000, SERIAL_8N1, _rxPin, _txPin);
    
    // Give radar time to power up
    delay(500);

    if (_radar.begin(Serial2)) {
        Serial.println("[LD2410] HARDWARE DRIVER INITIALIZED");
        _initialized = true;
    } else {
        Serial.println("[LD2410] HARDWARE NOT FOUND. Check wiring.");
        // We do not freeze so simulation can still run
    }
}

void LD2410Driver::update()
{
    // 1. Process Simulation
    if (_simQueue != NULL)
    {
        char command;
        if (xQueuePeek(_simQueue, &command, 0) == pdTRUE)
        {
            if (command == 'p')
            {
                xQueueReceive(_simQueue, &command, 0);
                _presence = true;
                // Note: Serial access assumes mutex where needed or wrapped externally
                if (xSemaphoreTake(xMutex_Serial, portMAX_DELAY) == pdTRUE) { Serial.println("[LD2410] Simulated Presence ON"); xSemaphoreGive(xMutex_Serial); }
            }
            else if (command == 'n')
            {
                xQueueReceive(_simQueue, &command, 0);
                _presence = false;
                if (xSemaphoreTake(xMutex_Serial, portMAX_DELAY) == pdTRUE) { Serial.println("[LD2410] Simulated Presence OFF"); xSemaphoreGive(xMutex_Serial); }
            }
        }
    }

    // 2. Hardware Parsing
    if (_initialized) {
        _radar.read();
        if (_radar.isConnected()) {
            // Update presence from the real hardware frame
            _presence = _radar.presenceDetected();
        }
    }
}

bool LD2410Driver::isPresenceDetected()
{
    return _presence;
}
