#include "communication/mqtt_manager.h"
#include "services/network/command_handler.h"
#include <Arduino.h>

MQTTManager::MQTTManager(const char* broker, int port)
    : _broker(broker),
      _port(port),
      _client(_wifiClient),
      _commandHandler(nullptr)
{
}

void MQTTManager::begin()
{
    _client.setServer(_broker, _port);
    _client.setCallback([this](char* topic, uint8_t* payload, unsigned int length) {
        this->callback(topic, payload, length);
    });
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

void MQTTManager::setCommandHandler(CommandHandler* handler)
{
    _commandHandler = handler;
}

void MQTTManager::callback(char* topic, byte* payload, unsigned int length)
{
    char message[64];
    unsigned int len = (length < 63) ? length : 63;
    memcpy(message, payload, len);
    message[len] = '\0';

    Serial.print("[MQTT] Message received on topic [");
    Serial.print(topic);
    Serial.print("]: ");
    Serial.println(message);

    if (_commandHandler)
    {
        _commandHandler->handleCommand(message);
    }
}

void MQTTManager::reconnect()
{
    if (_client.connect("zarzara_node"))
    {
        Serial.println("[MQTT] Connected");
        _client.subscribe("zarzara/node/command");
    }
}
