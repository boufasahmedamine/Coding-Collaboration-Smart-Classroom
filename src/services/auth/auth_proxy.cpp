#include "services/auth/auth_proxy.h"
#include <Arduino.h>

AuthProxy::AuthProxy(MQTTManager* mqtt)
    : _mqtt(mqtt)
{
}

void AuthProxy::requestAuthorization(const uint8_t* uid, uint8_t uidLength)
{
    if (!_mqtt || !_mqtt->isConnected()) {
        Serial.println("[AUTH] ERROR: MQTT not connected, cannot request auth");
        return;
    }

    // Format UID as HEX string for the server
    char uidBuf[20];
    char* p = uidBuf;
    for (uint8_t i = 0; i < uidLength; i++) {
        p += sprintf(p, "%02X", uid[i]);
    }

    Serial.printf("[AUTH] Requesting validation for UID: %s\n", uidBuf);
    
    _mqtt->publish(AUTH_REQUEST_TOPIC, uidBuf);
}
