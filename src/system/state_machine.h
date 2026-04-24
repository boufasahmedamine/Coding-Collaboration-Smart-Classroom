#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <stdint.h>
#include "drivers/doorlock/doorlock_driver.h"
#include "system/session_record.h"

class AttendanceManager;
class MQTTManager;

class StateMachine {
public:
    enum class SystemEvent {
    RFID_READ,
    ACCESS_GRANTED,
    ACCESS_DENIED,
    ACCESS_TIMEOUT,
    PRESENCE_DETECTED,
    PRESENCE_LOST,
    OVERRIDE_ON,
    OVERRIDE_OFF,
    UNLOCK_REQUEST
};
enum class SystemState {
    LOCKED,
    WAITING_FOR_AUTH,
    UNLOCKED
};

    static const unsigned long DEFAULT_SESSION_TIMEOUT_MS = 5400000; // 1.5 hours

    StateMachine(DoorLockDriver& doorLock, unsigned long sessionTimeoutMs, MQTTManager* mqtt = nullptr);

    void init();
    void update();

    void handleEvent(SystemEvent event, const uint8_t* uid = nullptr, uint8_t uidLength = 0);

    SystemState getState() const;
    SessionRecord& getCurrentSession();

    void cancelPendingRequest();

    bool isSessionActive() const;
    bool isOverrideActive() const;
    void setPresenceDetected(bool detected);
    void setOverrideActive(bool active);

private:
    DoorLockDriver& _doorLock;
    SystemState _currentState;
    MQTTManager* _mqtt;

    unsigned long _sessionStartTime;
    unsigned long _sessionDurationMs;
    
    unsigned long _authStartTime;
    unsigned long _authTimeoutMs;

    bool _presenceDetected;
    bool _overrideActive;

    SessionRecord _session;

    void transitionTo(SystemState newState);

    void handleLockedState(SystemEvent event, const uint8_t* uid, uint8_t uidLength);
    void handleSessionActiveState(SystemEvent event);
    void emitResultEvent(const char* status, const char* reason);
};

#endif