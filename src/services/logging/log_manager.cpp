#include "services/logging/log_manager.h"
#include <Arduino.h>

LogManager::LogManager(MQTTManager* mqtt, SDLogger* sd)
    : _mqtt(mqtt),
      _sd(sd)
{
}

void LogManager::logSessionStart(const SessionRecord& session)
{
    String logMsg = formatSessionStart(session);
    Serial.println("[LOG] " + logMsg);

    if (_mqtt && _mqtt->isConnected())
    {
        _mqtt->publish("classroom/attendance/start", logMsg.c_str());
    }

    if (_sd)
    {
        _sd->log(logMsg.c_str());
    }
}

void LogManager::logSessionEnd(const SessionRecord& session)
{
    String logMsg = formatSessionEnd(session);
    Serial.println("[LOG] " + logMsg);

    if (_mqtt && _mqtt->isConnected())
    {
        _mqtt->publish("classroom/attendance/end", logMsg.c_str());
    }

    if (_sd)
    {
        _sd->log(logMsg.c_str());
    }
}

String LogManager::formatSessionStart(const SessionRecord& session) 
{
    String line = "START,UID:";
    for (int i = 0; i < session.uidLength; i++) {
        line += String(session.uid[i], HEX) + " ";
    }
    line += ",TIME:" + String(session.startTime);
    return line;
}

String LogManager::formatSessionEnd(const SessionRecord& session) 
{
    String line = "END,UID:";
    for (int i = 0; i < session.uidLength; i++) {
        line += String(session.uid[i], HEX) + " ";
    }
    line += ",TIME:" + String(session.endTime);
    return line;
}
