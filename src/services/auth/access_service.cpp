#include "services/auth/access_service.h"
#include "system/diagnostics.h"
#include <Arduino.h>

extern SemaphoreHandle_t xMutex_StateMachine;

AccessService::AccessService(IRFIDReader* outside, IRFIDReader* inside, 
                             AuthProxy* proxy, LocalAuthService* local, StateMachine* sm, ProjectorLogic* projector)
    : _rfidOutside(outside), _rfidInside(inside), 
      _authProxy(proxy), _localAuth(local), _stateMachine(sm), _projectorLogic(projector),
      _lastHeartbeat(0)
{
}

void AccessService::init() {
    _rfidOutside->init();
    _rfidInside->init();
    _lastHeartbeat = millis();
}

void AccessService::update() {
    uint8_t uid[7];
    uint8_t uidLength;

    // --- 1. Poll Outside Reader (Access Control) ---
    bool outsideRead = _rfidOutside->readCard(uid, &uidLength);

    if (outsideRead) {
        handleOutsideScan(uid, uidLength);
    }

    // --- 2. Poll Inside Reader (Attendance) ---
    bool insideRead = _rfidInside->readCard(uid, &uidLength);

    if (insideRead) {
        handleInsideScan(uid, uidLength);
    }

    // --- 3. Health Monitoring & Auto-Recovery ---
    performHealthCheck();
}

void AccessService::handleOutsideScan(uint8_t* uid, uint8_t len) {
    Diagnostics::logEvent("[RFID] Outside scan detected");
    
    LocalAuthService::UserRole role = _localAuth->getRole(uid, len);

    if (xSemaphoreTake(xMutex_StateMachine, portMAX_DELAY) == pdTRUE) {
        bool isLatchedSession = _stateMachine->isLatched();
        bool isSessionActive = _stateMachine->isSessionActive();

        // --- TOGGLE LOGIC: Scan-Out for Admin/Maintenance ---
        if (isSessionActive && isLatchedSession) {
            if (role == LocalAuthService::UserRole::ADMIN || role == LocalAuthService::UserRole::MAINTENANCE) {
                Diagnostics::logEvent("[RFID] High-Level Card: Terminating Latched Session");
                _stateMachine->terminateSession();
                xSemaphoreGive(xMutex_StateMachine);
                return;
            }
        }

        // --- NEW SESSION LOGIC ---
        if (role == LocalAuthService::UserRole::ADMIN || role == LocalAuthService::UserRole::MAINTENANCE) {
            Diagnostics::logEvent("[RFID] High-Level Card: Starting Soft-Latched Session (8h)");
            _stateMachine->setLatched(true);
            _stateMachine->setSessionDuration(28800000UL); // 8 Hours Safety Net
            
            // Role-specific Authorization
            if (_projectorLogic) {
                _projectorLogic->setAuthorized(role == LocalAuthService::UserRole::ADMIN);
            }

            _stateMachine->handleEvent(StateMachine::SystemEvent::ADMIN_BYPASS, uid, len);
        } else {
            // Normal Student Path
            _authProxy->requestAuthorization(uid, len);
            _stateMachine->handleEvent(StateMachine::SystemEvent::RFID_READ, uid, len);
        }

        xSemaphoreGive(xMutex_StateMachine);
    }
}

void AccessService::handleInsideScan(uint8_t* uid, uint8_t len) {
    Diagnostics::logEvent("[RFID] Inside scan detected");
    
    // Special Action: Teacher/Admin scan on inside reader authorizes the projector
    if (_localAuth->isAdmin(uid, len)) {
        Diagnostics::logEvent("[RFID] Teacher/Admin Inside - Projector Authorized");
        if (_projectorLogic) {
            _projectorLogic->setAuthorized(true);
        }
    }
    
    _authProxy->requestAttendance(uid, len);
}

void AccessService::performHealthCheck() {
    if (millis() - _lastHeartbeat < _heartbeatInterval) return;
    _lastHeartbeat = millis();

    // Only heartbeat/recover if the reader was present at boot
    if (_rfidOutside->isInitialized() && !_rfidOutside->isAlive()) {
        Diagnostics::logEvent("[RFID] OUT failure detected! Recovering...");
        _rfidOutside->resetCommunication();
    }
    if (_rfidInside->isInitialized() && !_rfidInside->isAlive()) {
        Diagnostics::logEvent("[RFID] IN failure detected! Recovering...");
        _rfidInside->resetCommunication();
    }
}
