#include "services/network/dashboard_service.h"
#include "config/mqtt_config.h"
#include <Arduino.h>

DashboardService::DashboardService(MQTTManager* mqtt, StateMachine* sm, PresenceService* presence, Lighting* lights, IRProjector* projector)
    : _mqtt(mqtt), _sm(sm), _presence(presence), _lights(lights), _projector(projector), _lastBroadcast(0)
{
}

void DashboardService::update(bool force)
{
    if (!force && (millis() - _lastBroadcast < _interval)) return;
    if (!_mqtt || !_mqtt->isConnected()) return;

    _lastBroadcast = millis();

    JsonDocument doc;
    doc["classroom_name"] = CLASSROOM_NAME;
    doc["occupied"] = _presence ? _presence->isOccupied() : false;
    doc["lights_on"] = _lights ? _lights->isOn() : false;
    doc["door"] = (_sm && _sm->getState() == StateMachine::SystemState::UNLOCKED);
    doc["projector_on"] = _projector ? _projector->isOn() : false;
    
    // Safety check for empty indicators
    doc["smoke_detected"] = false;
    doc["danger_indicator"] = false;

    if (_sm) {
        SessionRecord& session = _sm->getCurrentSession();
        if (session.active) {
            char uidStr[20];
            String hex = "";
            for (uint8_t i = 0; i < session.uidLength; i++) {
                if (session.uid[i] < 0x10) hex += "0";
                hex += String(session.uid[i], HEX);
            }
            doc["teacher_rfid"] = hex;
        } else {
            doc["teacher_rfid"] = "";
        }
        doc["new_session"] = false; 
    }

    String output;
    serializeJson(doc, output);
    
    _mqtt->publish(TOPIC_EVENTS, output.c_str());
}
