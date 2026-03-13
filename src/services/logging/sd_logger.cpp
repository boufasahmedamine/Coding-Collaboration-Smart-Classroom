#include "services/logging/sd_logger.h"
#include <Arduino.h>

SDLogger::SDLogger(int csPin)
    : _csPin(csPin)
{
}

void SDLogger::begin()
{
    if (!SD.begin(_csPin))
    {
        Serial.println("[SD] Initialization failed");
        return;
    }

    Serial.println("[SD] Card initialized");
}

void SDLogger::log(const char* message)
{
    File file = SD.open("/logs.txt", FILE_APPEND);

    if (!file)
    {
        Serial.println("[SD] Failed to open log file");
        return;
    }

    file.println(message);
    file.close();
}
