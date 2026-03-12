#ifndef LOG_MANAGER_H
#define LOG_MANAGER_H

#include <stdint.h>
#include "system/session_record.h"
#include "communication/mqtt_manager.h"

class LogManager {
public:
    LogManager(MQTTManager* mqtt);

    void logSessionStart(const SessionRecord& session);
    void logSessionEnd(const SessionRecord& session);
    
private:
    MQTTManager* _mqtt;
};

#endif
