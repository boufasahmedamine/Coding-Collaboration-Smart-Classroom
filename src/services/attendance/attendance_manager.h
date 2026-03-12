#ifndef ATTENDANCE_MANAGER_H
#define ATTENDANCE_MANAGER_H

#include <stdint.h>
#include "system/session_record.h"

class AttendanceManager {
public:
    AttendanceManager();

    void onSessionStart(const SessionRecord& session);
    void onSessionEnd(const SessionRecord& session);
};

#endif
