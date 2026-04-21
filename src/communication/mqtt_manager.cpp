#include "communication/mqtt_manager.h"
#include "services/network/command_handler.h"
#include "config/mqtt_config.h"
#include "system/diagnostics.h"
#include <Arduino.h>
#include <WiFi.h>

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
    Diagnostics::setBrokerInfo(String(_broker) + ":" + String(_port));
    Diagnostics::setMQTTStatus("INITIALIZING");
}

void MQTTManager::update()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        Diagnostics::setMQTTStatus("WAITING FOR WIFI");
        return; 
    }

    if (!_client.connected())
    {
        Diagnostics::setMQTTStatus("DISCONNECTED");
        reconnect();
    }
    else
    {
        Diagnostics::setMQTTStatus("CONNECTED");
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
    char message[256]; // Increased buffer for JSON
    unsigned int len = (length < 255) ? length : 255;
    memcpy(message, payload, len);
    message[len] = '\0';

    if (_commandHandler)
    {
        _commandHandler->handleCommand(message, topic);
    }
}

void MQTTManager::reconnect()
{
    Diagnostics::setMQTTStatus("CONNECTING...");
    if (_client.connect(MQTT_CLIENT_ID))
    {
        Serial.println("[MQTT] Connected to Broker");
        
        // Subscribe to relevant topics per contract
        _client.subscribe(TOPIC_COMMANDS);
        _client.subscribe(TOPIC_ACCESS_RESPONSE);
        _client.subscribe(TOPIC_DELAY_RESPONSE);
    }
}
