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
    static String _lightStatus = "UNKNOWN";
    static String _presenceStatus = "EMPTY";
    static String _lastEvent = "Node Booting...";
    static bool _visible = true;
    static DashboardMode _mode = DashboardMode::SYSTEM;

    struct MqttLogEntry {
        char direction[4]; // "IN ", "OUT", "ERR"
        char topic[33];
        char payload[65];
        bool active = false;
    };

    static MqttLogEntry _mqttLogs[8];
    static int _mqttLogHead = 0;

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
    void setPresenceStatus(const String& status) { _presenceStatus = status; }
    void setLightingStatus(const String& status) { _lightStatus = status; }
    void logEvent(const String& eventStr) { _lastEvent = eventStr; }

    void toggleMode() {
        if (_mode == DashboardMode::SYSTEM) _mode = DashboardMode::MQTT_SNIFFER;
        else _mode = DashboardMode::SYSTEM;
    }

    void logMqttMessage(const char* topic, const char* payload, bool isIncoming, bool isError) {
        _mqttLogHead = (_mqttLogHead + 1) % 8;
        
        if (isError) strncpy(_mqttLogs[_mqttLogHead].direction, "ERR", 4);
        else if (isIncoming) strncpy(_mqttLogs[_mqttLogHead].direction, "IN ", 4);
        else strncpy(_mqttLogs[_mqttLogHead].direction, "OUT", 4);

        strncpy(_mqttLogs[_mqttLogHead].topic, topic, 32);
        _mqttLogs[_mqttLogHead].topic[32] = '\0';

        strncpy(_mqttLogs[_mqttLogHead].payload, payload, 64);
        _mqttLogs[_mqttLogHead].payload[64] = '\0';
        
        _mqttLogs[_mqttLogHead].active = true;
    }

    static void drawSystemDashboard() {
        Serial.println("==================================================");
        Serial.println("           SYSTEM DIAGNOSTICS DASHBOARD");
        Serial.println("==================================================");
        
        Serial.printf("[CORE] State Machine: %s\n", _sysState.c_str());
        Serial.printf("[RFID] OUT (Entry):   %s\n", _rfidStatusOut.c_str());
        Serial.printf("[RFID] IN (Attend):   %s\n", _rfidStatusIn.c_str());
        Serial.printf("[DOOR] Maglock:       %s\n", _doorStatus.c_str());
        Serial.printf("[ENVI] Presence:      %s\n", _presenceStatus.c_str());
        Serial.printf("[LITE] Relays:        %s\n", _lightStatus.c_str());
        
        Serial.println("---------------- Networking ----------------------");
        Serial.printf("[NETW] WiFi Status:   %s\n", _wifiStatus.c_str());
        Serial.printf("[NETW] Local IP:      %s\n", _localIP.c_str());
        Serial.printf("[NETW] MQTT Status:   %s\n", _mqttStatus.c_str());
        Serial.printf("[NETW] Broker:        %s\n", _brokerInfo.c_str());
        
        Serial.println("==================================================");
        Serial.printf("[EVENT] Last Action: %s\n", _lastEvent.c_str());
        Serial.println("==================================================");
        
        Serial.println("----------- Hardware Pin Monitor -----------------");
        Serial.printf("PIR (18): %-4s | DOOR (25): %s\n", 
            (digitalRead(18) == HIGH ? "HIGH" : "LOW"), (digitalRead(25) == HIGH ? "HIGH" : "LOW"));
        Serial.printf("RDM_O(16): %-4s | RDM_I(17): %-4s\n",
            (digitalRead(16) == HIGH ? "HIGH" : "LOW"), (digitalRead(17) == HIGH ? "HIGH" : "LOW"));
        Serial.println("==================================================");
        Serial.println(" (Press 'm' to switch to MQTT Sniffer Mode) ");
    }

    static void drawMqttDashboard() {
        Serial.println("==================================================");
        Serial.println("           MQTT COMMUNICATION SNIFFER");
        Serial.println("==================================================");
        Serial.printf("[NETW] Broker: %s\n", _brokerInfo.c_str());
        Serial.printf("[NETW] Status: %s\n", _mqttStatus.c_str());
        Serial.println("--------------------------------------------------");
        
        for (int i = 0; i < 8; i++) {
            int idx = (_mqttLogHead - i + 8) % 8;
            if (_mqttLogs[idx].active) {
                Serial.printf("[%s] %-20s -> %s\n", 
                    _mqttLogs[idx].direction, 
                    _mqttLogs[idx].topic, 
                    _mqttLogs[idx].payload);
            }
        }
        
        Serial.println("==================================================");
        Serial.println(" (Press 'm' to switch to System Status Mode) ");
    }

    void updateTable() {
#if ENABLE_DIAGNOSTICS_DASHBOARD
        if (!_visible) return;
        if (xMutex_Serial && xSemaphoreTake(xMutex_Serial, portMAX_DELAY) == pdTRUE) {
            Serial.print("\e[2J\e[H");
            
            if (_mode == DashboardMode::SYSTEM) drawSystemDashboard();
            else drawMqttDashboard();
            
            xSemaphoreGive(xMutex_Serial);
        }
#endif
    }
}
