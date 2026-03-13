#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <stdint.h>
#include "drivers/doorlock/doorlock_driver.h"
#include "system/session_record.h"

class AttendanceManager;

class StateMachine {
public:
    enum class SystemEvent {
    ACCESS_GRANTED,
    PRESENCE_DETECTED,
    PRESENCE_LOST,
    OVERRIDE_ON,
    OVERRIDE_OFF,
    UNLOCK_REQUEST
};
enum class SystemState { // Renamed from State
    LOCKED,
    UNLOCKED // Changed from SESSION_ACTIVE
};

    static const unsigned long DEFAULT_SESSION_TIMEOUT_MS = 5400000; // 1.5 hours

    StateMachine(DoorLockDriver& doorLock, unsigned long sessionTimeoutMs); // Removed AttendanceManager

    void init();
    void update();

    // Event handling interface for future expansion
    void handleEvent(SystemEvent event, const uint8_t* uid = nullptr, uint8_t uidLength = 0);

    SystemState getState() const; // Changed return type from State to SystemState
    SessionRecord& getCurrentSession();

    // Future expansion hooks
    void setPresenceDetected(bool detected);
    void setOverrideActive(bool active);

private:
    DoorLockDriver& _doorLock; // Changed from DoorLock&

    SystemState _currentState; // Changed type from State

    unsigned long _sessionStartTime;
    unsigned long _sessionDurationMs; // Kept as sessionDurationMs, as the instruction didn't explicitly change the member variable name.

    bool _presenceDetected;
    bool _overrideActive;

    SessionRecord _session;

    void transitionTo(SystemState newState);

    void handleLockedState(SystemEvent event, const uint8_t* uid, uint8_t uidLength);
    void handleSessionActiveState(SystemEvent event);
};

#endif