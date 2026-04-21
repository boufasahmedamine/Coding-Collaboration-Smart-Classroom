#include "system/diagnostics.h"
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

extern SemaphoreHandle_t xMutex_Serial;

namespace Diagnostics {

    static String _sysState = "INITIALIZING";
    static String _rfidStatusOut = "UNKNOWN";
    static String _rfidStatusIn = "UNKNOWN";
    static String _doorStatus = "UNKNOWN";
    static String _wifiStatus = "DISCONNECTED";
    static String _localIP = "0.0.0.0";
    static String _mqttStatus = "OFFLINE";
    static String _brokerInfo = "N/A";
    static int _ldrValue = 0;
    static String _lightStatus = "UNKNOWN";
    static String _lastEvent = "Node Booting...";
    static bool _visible = true;

    void init() {
    }

    void setSystemState(const String& state) { _sysState = state; }
    void setRFIDStatusOut(const String& status) { _rfidStatusOut = status; }
    void setRFIDStatusIn(const String& status) { _rfidStatusIn = status; }
    void setDoorStatus(const String& status) { _doorStatus = status; }
    void setWiFiStatus(const String& status) { _wifiStatus = status; }
    void setLocalIP(const String& ip) { _localIP = ip; }
    void setMQTTStatus(const String& status) { _mqttStatus = status; }
    void setBrokerInfo(const String& info) { _brokerInfo = info; }
    void setDashboardVisible(bool visible) { _visible = visible; }
    bool isDashboardVisible() { return _visible; }
    void setLDRValue(int value) { _ldrValue = value; }
    void setLightingStatus(const String& status) { _lightStatus = status; }
    void logEvent(const String& eventStr) { _lastEvent = eventStr; }

    void updateTable() {
#if ENABLE_DIAGNOSTICS_DASHBOARD
        if (!_visible) return;
        if (xMutex_Serial && xSemaphoreTake(xMutex_Serial, portMAX_DELAY) == pdTRUE) {
            // ANSI escape codes: \e[2J (Clear Screen) \e[H (Cursor to home)
            Serial.print("\e[2J\e[H");
            
            Serial.println("==================================================");
            Serial.println("           SYSTEM DIAGNOSTICS DASHBOARD");
            Serial.println("==================================================");
            
            Serial.printf("[CORE] State Machine: %s\n", _sysState.c_str());
            Serial.printf("[RFID] OUT (Entry):   %s\n", _rfidStatusOut.c_str());
            Serial.printf("[RFID] IN (Attend):   %s\n", _rfidStatusIn.c_str());
            Serial.printf("[DOOR] Maglock:       %s\n", _doorStatus.c_str());
            Serial.printf("[LITE] LDR Value:     %d\n", _ldrValue);
            Serial.printf("[LITE] Relays:        %s\n", _lightStatus.c_str());
            
            Serial.println("---------------- Networking ----------------------");
            Serial.printf("[NETW] WiFi Status:   %s\n", _wifiStatus.c_str());
            Serial.printf("[NETW] Local IP:      %s\n", _localIP.c_str());
            Serial.printf("[NETW] MQTT Status:   %s\n", _mqttStatus.c_str());
            Serial.printf("[NETW] Broker:        %s\n", _brokerInfo.c_str());
            
            Serial.println("==================================================");
            Serial.printf("[EVENT] Last Action: %s\n", _lastEvent.c_str());
            Serial.println("==================================================");
            
            xSemaphoreGive(xMutex_Serial);
        }
#endif
    }
}
