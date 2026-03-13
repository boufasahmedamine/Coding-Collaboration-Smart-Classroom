#ifndef ATTENDANCE_MANAGER_H
#define ATTENDANCE_MANAGER_H

#include <stdint.h>
#include "system/session_record.h"

class LogManager;
class StateMachine;

class AttendanceManager {
public:
    AttendanceManager(LogManager* logManager, StateMachine* stateMachine);

    void update();

private:
    LogManager* _logManager;
    StateMachine* _stateMachine;
};

#endif
