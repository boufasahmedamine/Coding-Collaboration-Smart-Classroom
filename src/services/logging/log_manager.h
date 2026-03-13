#ifndef LOG_MANAGER_H
#define LOG_MANAGER_H

#include <stdint.h>
#include "system/session_record.h"
#include "services/logging/sd_logger.h"
#include "communication/mqtt_manager.h"

class LogManager {
public:
    LogManager(MQTTManager* mqtt, SDLogger* sd);

    void logSessionStart(const SessionRecord& session);
    void logSessionEnd(const SessionRecord& session);
    
private:
    MQTTManager* _mqtt;
    SDLogger* _sd;
};

#endif
