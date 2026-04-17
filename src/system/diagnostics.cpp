#include "system/diagnostics.h"
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

extern SemaphoreHandle_t xMutex_Serial;

namespace Diagnostics {

    static String _sysState = "INITIALIZING";
    static String _rfidStatus = "UNKNOWN";
    static String _radarStatus = "UNKNOWN";
    static String _doorStatus = "UNKNOWN";
    static int _ldrValue = 0;
    static String _lightStatus = "UNKNOWN";
    static String _lastEvent = "Node Booting...";

    void init() {
    }

    void setSystemState(const String& state) { _sysState = state; }
    void setRFIDStatus(const String& status) { _rfidStatus = status; }
    void setRadarStatus(const String& status) { _radarStatus = status; }
    void setDoorStatus(const String& status) { _doorStatus = status; }
    void setLDRValue(int value) { _ldrValue = value; }
    void setLightingStatus(const String& status) { _lightStatus = status; }
    void logEvent(const String& eventStr) { _lastEvent = eventStr; }

    void updateTable() {
#if ENABLE_DIAGNOSTICS_DASHBOARD
        if (xMutex_Serial && xSemaphoreTake(xMutex_Serial, portMAX_DELAY) == pdTRUE) {
            // ANSI escape codes: \e[2J (Clear Screen) \e[H (Cursor to home)
            Serial.print("\e[2J\e[H");
            
            Serial.println("==================================================");
            Serial.println("           SYSTEM DIAGNOSTICS DASHBOARD");
            Serial.println("==================================================");
            
            Serial.printf("[CORE] State Machine: %s\n", _sysState.c_str());
            Serial.printf("[RFID] PN532 Status:  %s\n", _rfidStatus.c_str());
            Serial.printf("[RADR] LD2410 State:  %s\n", _radarStatus.c_str());
            Serial.printf("[DOOR] Maglock:       %s\n", _doorStatus.c_str());
            Serial.printf("[LITE] LDR Value:     %d\n", _ldrValue);
            Serial.printf("[LITE] Relays:        %s\n", _lightStatus.c_str());
            
            Serial.println("==================================================");
            Serial.printf("[EVENT] Last Action: %s\n", _lastEvent.c_str());
            Serial.println("==================================================");
            
            xSemaphoreGive(xMutex_Serial);
        }
#endif
    }
}
