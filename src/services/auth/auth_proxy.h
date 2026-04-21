#ifndef AUTH_PROXY_H
#define AUTH_PROXY_H

#include <stdint.h>
#include "communication/mqtt_manager.h"
#include <ArduinoJson.h>

class AuthProxy {
public:
    AuthProxy(MQTTManager* mqtt);

    /**
     * Publishes a teacher access request for entry (Outside Reader).
     */
    void requestAuthorization(const uint8_t* uid, uint8_t uidLength);

    /**
     * Publishes a student attendance request (Inside Reader).
     */
    void requestAttendance(const uint8_t* uid, uint8_t uidLength);

private:
    MQTTManager* _mqtt;
    String generateRequestId();
};

#endif
