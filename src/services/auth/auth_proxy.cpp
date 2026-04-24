#include "services/auth/auth_proxy.h"
#include "system/state_machine.h"
#include "config/mqtt_config.h"
#include <Arduino.h>

AuthProxy::AuthProxy(MQTTManager* mqtt)
    : _mqtt(mqtt), _sm(nullptr), _pendingRequestId("")
{
}

void AuthProxy::requestAuthorization(const uint8_t* uid, uint8_t uidLength)
{
    if (!_mqtt || !_mqtt->isConnected()) return;

    // 🔴 ABORT & REPLACE: Invalidate any existing request
    _pendingRequestId = generateRequestId();

    char uidBuf[20];
    char* p = uidBuf;
    for (uint8_t i = 0; i < uidLength; i++) {
        p += sprintf(p, "%02X", uid[i]);
    }

    JsonDocument doc;
    doc["event"] = "access_request";
    doc["classroom_name"] = CLASSROOM_NAME;
    
    JsonObject data = doc["data"].to<JsonObject>();
    data["teacher_rfid"] = uidBuf;
    data["request_id"] = _pendingRequestId;

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
    doc["event"] = "attendance_request";
    doc["classroom_name"] = CLASSROOM_NAME;

    JsonObject data = doc["data"].to<JsonObject>();
    JsonArray rfids = data["student_rfids"].to<JsonArray>();
    rfids.add(uidBuf);

    String output;
    serializeJson(doc, output);
    
    Serial.printf("[AUTH] Sending Attendance Request: %s\n", output.c_str());
    _mqtt->publish(TOPIC_ATTEND_REQUEST, output.c_str());
}

void AuthProxy::handleResponse(JsonDocument& doc) {
    // 🟠 JSON Validation (Trust Boundary)
    if (!doc["event"].is<String>() || !doc["request_id"].is<String>()) {
        Serial.println("[AUTH] Response rejected: Missing event or request_id");
        return;
    }

    String reqId = doc["request_id"].as<String>();
    bool approved = doc["approved"].as<bool>();

    // 🔴 Correlation Check
    if (_pendingRequestId == "" || reqId != _pendingRequestId) {
        Serial.printf("[AUTH] Response ignored: ID mismatch or late (%s vs %s)\n", reqId.c_str(), _pendingRequestId.c_str());
        return;
    }

    // 🟠 Semantic Validation
    if (!doc["approved"].is<bool>()) {
        Serial.println("[AUTH] Response rejected: 'approved' must be boolean");
        return;
    }

    Serial.printf("[AUTH] Request %s resolved: %s\n", reqId.c_str(), approved ? "GRANTED" : "DENIED");

    // Clear ID to prevent replay
    _pendingRequestId = "";

    if (_sm) {
        _sm->handleEvent(approved ? StateMachine::SystemEvent::ACCESS_GRANTED : StateMachine::SystemEvent::ACCESS_DENIED);
    }
}

String AuthProxy::generateRequestId() {
    char buf[9];
    uint32_t r = esp_random();
    sprintf(buf, "%08X", r);
    return String(buf);
}
