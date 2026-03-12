#include "services/logging/log_manager.h"
#include <Arduino.h>

LogManager::LogManager() {}

void LogManager::logSessionStart(const SessionRecord& session)
{
    Serial.print("[LOG] Session start UID: ");

    for (int i = 0; i < session.uidLength; i++)
    {
        Serial.print(session.uid[i], HEX);
        Serial.print(" ");
    }

    Serial.println();
}

void LogManager::logSessionEnd(const SessionRecord& session)
{
    Serial.print("[LOG] Session end duration: ");

    unsigned long duration = session.endTime - session.startTime;

    Serial.print(duration / 1000);
    Serial.println(" seconds");
}
