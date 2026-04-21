#include "services/auth/auth_proxy.h"
#include "config/mqtt_config.h"
#include <Arduino.h>

AuthProxy::AuthProxy(MQTTManager* mqtt)
    : _mqtt(mqtt)
{
}

void AuthProxy::requestAuthorization(const uint8_t* uid, uint8_t uidLength)
{
    if (!_mqtt || !_mqtt->isConnected()) return;

    char uidBuf[20];
    char* p = uidBuf;
    for (uint8_t i = 0; i < uidLength; i++) {
        p += sprintf(p, "%02X", uid[i]);
    }

    JsonDocument doc;
    doc["teacher_rfid"] = uidBuf;
    doc["request_id"] = generateRequestId();

    String output;
    serializeJson(doc, output);
    
    Serial.printf("[AUTH] Sending Access Request: %s\n", output.c_str());
    _mqtt->publish(TOPIC_ACCESS_REQUEST, output.c_str());
}

void AuthProxy::requestAttendance(const uint8_t* uid, uint8_t uidLength)
{
    if (!_mqtt || !_mqtt->isConnected()) return;

    char uidBuf[20];
    char* p = uidBuf;
    for (uint8_t i = 0; i < uidLength; i++) {
        p += sprintf(p, "%02X", uid[i]);
    }

    JsonDocument doc;
    JsonArray rfids = doc["student_rfids"].to<JsonArray>();
    rfids.add(uidBuf);
    doc["event"] = "attendance_request";

    String output;
    serializeJson(doc, output);
    
    Serial.printf("[AUTH] Sending Attendance Request: %s\n", output.c_str());
    _mqtt->publish(TOPIC_ATTEND_REQUEST, output.c_str());
}

String AuthProxy::generateRequestId() {
    return "req-" + String(millis());
}
