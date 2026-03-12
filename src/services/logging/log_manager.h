#ifndef LOG_MANAGER_H
#define LOG_MANAGER_H

#include <stdint.h>
#include "system/session_record.h"

class LogManager {
public:
    LogManager();

    void logSessionStart(const SessionRecord& session);
    void logSessionEnd(const SessionRecord& session);
};

#endif
