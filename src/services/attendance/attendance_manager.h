#ifndef ATTENDANCE_MANAGER_H
#define ATTENDANCE_MANAGER_H

#include <stdint.h>
#include "system/session_record.h"
#include "services/logging/log_manager.h"

class AttendanceManager {
public:
    AttendanceManager(LogManager* logManager);

    void onSessionStart(const SessionRecord& session);
    void onSessionEnd(const SessionRecord& session);
    
private:
    LogManager* _logManager;
};

#endif
