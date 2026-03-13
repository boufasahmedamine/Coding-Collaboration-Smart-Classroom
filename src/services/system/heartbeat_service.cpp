#include "services/system/heartbeat_service.h"
#include <Arduino.h>

HeartbeatService::HeartbeatService(MQTTManager* mqtt)
    : _mqtt(mqtt),
      _lastHeartbeat(0)
{
}

void HeartbeatService::update()
{
    unsigned long now = millis();

    if (now - _lastHeartbeat >= _interval)
    {
        _lastHeartbeat = now;

        Serial.println("[HEARTBEAT] Node alive");

        if (_mqtt && _mqtt->isConnected())
        {
            _mqtt->publish("zarzara/node/heartbeat", "alive");
        }
    }
}
