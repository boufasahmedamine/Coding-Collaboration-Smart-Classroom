#include <Arduino.h>
#include "system/diagnostics.h"

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
        Diagnostics::logEvent("LD2410 HW DRIVER INTIALIZED");
        Diagnostics::setRadarStatus("READY");
        _initialized = true;
    } else {
        Diagnostics::logEvent("LD2410 HARDWARE NOT FOUND");
        Diagnostics::setRadarStatus("ERROR");
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
                Diagnostics::logEvent("LD2410 Simulated Presence ON");
                Diagnostics::setRadarStatus("ACTIVE (SIM)");
            }
            else if (command == 'n')
            {
                xQueueReceive(_simQueue, &command, 0);
                _presence = false;
                Diagnostics::logEvent("LD2410 Simulated Presence OFF");
                Diagnostics::setRadarStatus("IDLE (SIM)");
            }
        }
    }

    // 2. Hardware Parsing
    if (_initialized) {
        _radar.read();
        if (_radar.isConnected()) {
            // Update presence from the real hardware frame
            _presence = _radar.presenceDetected();
            Diagnostics::setRadarStatus(_presence ? "ACTIVE" : "IDLE");
        }
    }
}

bool LD2410Driver::isPresenceDetected()
{
    return _presence;
}
