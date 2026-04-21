#ifndef DIAGNOSTICS_H
#define DIAGNOSTICS_H

#include <Arduino.h>

// Set to 0 to disable the dashboard globally
#define ENABLE_DIAGNOSTICS_DASHBOARD 1

namespace Diagnostics {

    void init();
    void updateTable();
    
    void setSystemState(const String& state);
    void setRFIDStatusOut(const String& status);
    void setRFIDStatusIn(const String& status);
    void setDoorStatus(const String& status);
    
    void setWiFiStatus(const String& status);
    void setLocalIP(const String& ip);
    void setMQTTStatus(const String& status);
    void setBrokerInfo(const String& info);

    void setDashboardVisible(bool visible);
    bool isDashboardVisible();
    void setLDRValue(int value);
    void setLightingStatus(const String& status);
    void logEvent(const String& eventStr);

}

#endif
