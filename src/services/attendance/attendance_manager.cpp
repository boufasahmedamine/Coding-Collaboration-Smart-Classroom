#include "services/attendance/attendance_manager.h"
#include "system/state_machine.h"
#include "services/logging/log_manager.h"

#include <Arduino.h>

AttendanceManager::AttendanceManager(LogManager* logManager, StateMachine* stateMachine)
    : _logManager(logManager), _stateMachine(stateMachine)
{
}

void AttendanceManager::update()
{
    if (!_stateMachine) return;

    SessionRecord& session = _stateMachine->getCurrentSession();

    if (_stateMachine->getState() == StateMachine::SystemState::UNLOCKED)
    {
        // Session started logic
        if (session.active && !session.loggedStart)
        {
            if (_logManager)
            {
                _logManager->logSessionStart(session);
            }
            session.loggedStart = true;
        }
    }
    else
    {
        // Session ended logic
        if (!session.active && !session.loggedEnd && session.loggedStart)
        {
            if (_logManager)
            {
                _logManager->logSessionEnd(session);
            }
            session.loggedEnd = true;
        }
    }
}
