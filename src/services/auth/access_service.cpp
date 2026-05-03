#include "services/auth/access_service.h"
#include "system/diagnostics.h"
#include <Arduino.h>

extern SemaphoreHandle_t xMutex_SPIBus;
extern SemaphoreHandle_t xMutex_StateMachine;

AccessService::AccessService(PN532Driver* outside, PN532Driver* inside, 
                             AuthProxy* proxy, LocalAuthService* local, StateMachine* sm)
    : _rfidOutside(outside), _rfidInside(inside), 
      _authProxy(proxy), _localAuth(local), _stateMachine(sm),
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
    bool outsideRead = false;
    if (xMutex_SPIBus && xSemaphoreTake(xMutex_SPIBus, 10 / portTICK_PERIOD_MS) == pdTRUE) {
        outsideRead = _rfidOutside->readCard(uid, &uidLength);
        xSemaphoreGive(xMutex_SPIBus);
    }

    if (outsideRead) {
        handleOutsideScan(uid, uidLength);
    }

    // --- 2. Poll Inside Reader (Attendance) ---
    bool insideRead = false;
    if (xMutex_SPIBus && xSemaphoreTake(xMutex_SPIBus, 10 / portTICK_PERIOD_MS) == pdTRUE) {
        insideRead = _rfidInside->readCard(uid, &uidLength);
        xSemaphoreGive(xMutex_SPIBus);
    }

    if (insideRead) {
        handleInsideScan(uid, uidLength);
    }

    // --- 3. Health Monitoring & Auto-Recovery ---
    performHealthCheck();
}

void AccessService::handleOutsideScan(uint8_t* uid, uint8_t len) {
    Diagnostics::logEvent("[RFID] Outside scan detected");
    
    if (_localAuth->isAdmin(uid, len)) {
        if (xSemaphoreTake(xMutex_StateMachine, portMAX_DELAY) == pdTRUE) {
            _stateMachine->handleEvent(StateMachine::SystemEvent::ADMIN_BYPASS, uid, len);
            xSemaphoreGive(xMutex_StateMachine);
        }
    } else {
        _authProxy->requestAuthorization(uid, len);
        if (xSemaphoreTake(xMutex_StateMachine, portMAX_DELAY) == pdTRUE) {
            _stateMachine->handleEvent(StateMachine::SystemEvent::RFID_READ, uid, len);
            xSemaphoreGive(xMutex_StateMachine);
        }
    }
}

void AccessService::handleInsideScan(uint8_t* uid, uint8_t len) {
    Diagnostics::logEvent("[RFID] Inside scan detected");
    _authProxy->requestAttendance(uid, len);
}

void AccessService::performHealthCheck() {
    if (millis() - _lastHeartbeat < _heartbeatInterval) return;
    _lastHeartbeat = millis();

    if (xMutex_SPIBus && xSemaphoreTake(xMutex_SPIBus, 50 / portTICK_PERIOD_MS) == pdTRUE) {
        // Only heartbeat/recover if the reader was present at boot
        if (_rfidOutside->isInitialized() && !_rfidOutside->isAlive()) {
            Diagnostics::logEvent("[RFID] OUT failure detected! Recovering...");
            _rfidOutside->resetCommunication();
        }
        if (_rfidInside->isInitialized() && !_rfidInside->isAlive()) {
            Diagnostics::logEvent("[RFID] IN failure detected! Recovering...");
            _rfidInside->resetCommunication();
        }
        xSemaphoreGive(xMutex_SPIBus);
    }
}
