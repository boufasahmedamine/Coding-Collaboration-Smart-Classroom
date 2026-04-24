#include "services/system/heartbeat_service.h"
#include "config/mqtt_config.h"
#include <ArduinoJson.h>

HeartbeatService::HeartbeatService(MQTTManager* mqtt)
    : _mqtt(mqtt), _lastHeartbeat(0)
{
}

void HeartbeatService::update()
{
    unsigned long now = millis();

    if (now - _lastHeartbeat >= _interval)
    {
        _lastHeartbeat = now;

        if (_mqtt && _mqtt->isConnected())
        {
            JsonDocument doc;
            doc["event"] = "heartbeat";
            doc["classroom_name"] = CLASSROOM_NAME;
            
            JsonObject data = doc["data"].to<JsonObject>();
            data["status"] = "alive";
            data["uptime"] = now / 1000;

            String output;
            serializeJson(doc, output);
            _mqtt->publish(TOPIC_EVENTS, output.c_str());
        }
    }
}
