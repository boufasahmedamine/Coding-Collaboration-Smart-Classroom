#include "services/attendance/attendance_manager.h"

AttendanceManager::AttendanceManager(LogManager* logManager)
    : _logManager(logManager)
{
}

void AttendanceManager::onSessionStart(const SessionRecord& session)
{
    _logManager->logSessionStart(session);
}

void AttendanceManager::onSessionEnd(const SessionRecord& session)
{
    _logManager->logSessionEnd(session);
}
