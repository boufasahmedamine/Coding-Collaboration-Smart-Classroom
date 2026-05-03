#ifndef ACCESS_SERVICE_H
#define ACCESS_SERVICE_H

#include "drivers/rfid/pn532.h"
#include "services/auth/auth_proxy.h"
#include "services/auth/local_auth_service.h"
#include "system/state_machine.h"
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

class AccessService {
public:
    AccessService(PN532Driver* outside, PN532Driver* inside, 
                  AuthProxy* proxy, LocalAuthService* local, StateMachine* sm);

    void init();
    void update(); // Called within the RFID task

private:
    PN532Driver* _rfidOutside;
    PN532Driver* _rfidInside;
    AuthProxy* _authProxy;
    LocalAuthService* _localAuth;
    StateMachine* _stateMachine;

    unsigned long _lastHeartbeat;
    const unsigned long _heartbeatInterval = 5000;

    void handleOutsideScan(uint8_t* uid, uint8_t len);
    void handleInsideScan(uint8_t* uid, uint8_t len);
    void performHealthCheck();
};

#endif
