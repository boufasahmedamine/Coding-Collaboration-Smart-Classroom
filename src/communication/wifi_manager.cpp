#include "communication/wifi_manager.h"
#include <Arduino.h>
#include <WiFi.h>

WiFiManager::WiFiManager(const char* ssid, const char* password)
    : _ssid(ssid), _password(password)
{
}

void WiFiManager::begin()
{
    Serial.println("[WIFI] Connecting...");

    WiFi.begin(_ssid, _password);
}

void WiFiManager::update()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        return;
    }
}

bool WiFiManager::isConnected()
{
    return WiFi.status() == WL_CONNECTED;
}
