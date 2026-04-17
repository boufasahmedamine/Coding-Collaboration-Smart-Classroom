#ifndef DIAGNOSTICS_H
#define DIAGNOSTICS_H

#include <Arduino.h>

// Set to 0 to disable the dashboard globally
#define ENABLE_DIAGNOSTICS_DASHBOARD 1

namespace Diagnostics {

    void init();
    void updateTable();
    
    void setSystemState(const String& state);
    void setRFIDStatus(const String& status);
    void setRadarStatus(const String& status);
    void setDoorStatus(const String& status);
    void setLDRValue(int value);
    void setLightingStatus(const String& status);
    void logEvent(const String& eventStr);

}

#endif
