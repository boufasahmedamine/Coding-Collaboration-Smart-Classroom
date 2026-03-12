#include "services/attendance/attendance_manager.h"
#include <Arduino.h>

AttendanceManager::AttendanceManager() {}

void AttendanceManager::onSessionStart(const SessionRecord& session)
{
    Serial.print("[ATTENDANCE] Session started by UID: ");

    for (int i = 0; i < session.uidLength; i++)
    {
        Serial.print(session.uid[i], HEX);
        Serial.print(" ");
    }

    Serial.println();
}

void AttendanceManager::onSessionEnd(const SessionRecord& session)
{
    Serial.print("[ATTENDANCE] Session ended. Duration: ");

    unsigned long duration = session.endTime - session.startTime;

    Serial.print(duration / 1000);
    Serial.println(" seconds");
}
