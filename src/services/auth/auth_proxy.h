#ifndef AUTH_PROXY_H
#define AUTH_PROXY_H

#include <stdint.h>
#include "communication/mqtt_manager.h"
#include <ArduinoJson.h>

class StateMachine;

class AuthProxy {
public:
    AuthProxy(MQTTManager* mqtt);

    void requestAuthorization(const uint8_t* uid, uint8_t uidLength);
    void requestAttendance(const uint8_t* uid, uint8_t uidLength);

    void setStateMachine(StateMachine* sm) { _sm = sm; }
    void handleResponse(JsonDocument& doc);

private:
    MQTTManager* _mqtt;
    StateMachine* _sm;
    String _pendingRequestId;

    String generateRequestId();
};

#endif
