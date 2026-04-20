#include "tests/test_auth_proxy.h"
#include <Arduino.h>
#include "services/auth/auth_proxy.h"
#include "communication/mqtt_manager.h"

// Needs valid MQTT instance for full test, but we can verify logic calls.
static MQTTManager dummyMqtt("1.1.1.1", 1883);
static AuthProxy authProxy(&dummyMqtt);

void runAuthProxyTest() {
    static bool initialized = false;
    if (!initialized) {
        Serial.begin(115200);
        Serial.println("AuthProxy Test Initialized");
        Serial.println("Type 'a' to simulate UID scan and MQTT request");
        initialized = true;
    }

    if (Serial.available()) {
        char c = Serial.read();
        if (c == 'a') {
            uint8_t dummyUID[] = {0xDE, 0xAD, 0xBE, 0xEF};
            Serial.println("[TEST] Simulating valid card detection...");
            authProxy.requestAuthorization(dummyUID, 4);
        }
    }
}
