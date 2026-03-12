#include "communication/mqtt_manager.h"
#include <Arduino.h>

MQTTManager::MQTTManager(const char* broker, int port)
    : _broker(broker),
      _port(port),
      _client(_wifiClient)
{
}

void MQTTManager::begin()
{
    _client.setServer(_broker, _port);
}

void MQTTManager::update()
{
    if (!_client.connected())
    {
        reconnect();
    }

    _client.loop();
}

bool MQTTManager::isConnected()
{
    return _client.connected();
}

void MQTTManager::publish(const char* topic, const char* message)
{
    if (_client.connected())
    {
        _client.publish(topic, message);
    }
}

void MQTTManager::reconnect()
{
    if (_client.connect("zarzara_node"))
    {
        Serial.println("[MQTT] Connected");
    }
}
