#include "communication/wifi_manager.h"
#include "system/diagnostics.h"
#include <Arduino.h>
#include <WiFi.h>

WiFiManager::WiFiManager(const char* ssid, const char* password)
    : _ssid(ssid), _password(password)
{
}

void WiFiManager::begin()
{
    Serial.println("[WIFI] Connecting...");
    Diagnostics::setWiFiStatus("CONNECTING...");

    WiFi.mode(WIFI_STA);
    WiFi.begin(_ssid, _password);
}

void WiFiManager::update()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        Diagnostics::setWiFiStatus("CONNECTED");
        Diagnostics::setLocalIP(WiFi.localIP().toString());
    }
    else
    {
        Diagnostics::setWiFiStatus("DISCONNECTED");
        Diagnostics::setLocalIP("0.0.0.0");
    }
}

bool WiFiManager::isConnected()
{
    return WiFi.status() == WL_CONNECTED;
}
