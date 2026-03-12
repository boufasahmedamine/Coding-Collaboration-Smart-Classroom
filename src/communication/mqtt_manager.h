#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <PubSubClient.h>
#include <WiFiClient.h>

class MQTTManager
{
public:
    MQTTManager(const char* broker, int port);

    void begin();
    void update();

    bool isConnected();
    void publish(const char* topic, const char* message);

private:
    const char* _broker;
    int _port;

    WiFiClient _wifiClient;
    PubSubClient _client;

    void reconnect();
};

#endif
