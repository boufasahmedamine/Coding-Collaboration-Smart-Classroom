#ifndef HEARTBEAT_SERVICE_H
#define HEARTBEAT_SERVICE_H

#include "communication/mqtt_manager.h"

class HeartbeatService
{
public:
    HeartbeatService(MQTTManager* mqtt);

    void update();

private:
    MQTTManager* _mqtt;

    unsigned long _lastHeartbeat;
    const unsigned long _interval = 30000; // 30 seconds
};

#endif
