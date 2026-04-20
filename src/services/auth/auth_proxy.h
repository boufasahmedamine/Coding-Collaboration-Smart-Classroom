#ifndef AUTH_PROXY_H
#define AUTH_PROXY_H

#include <stdint.h>
#include "communication/mqtt_manager.h"

class AuthProxy {
public:
    AuthProxy(MQTTManager* mqtt);

    /**
     * Publishes the detected UID to the server for validation.
     * The response will be handled via CommandHandler in a separate flow.
     */
    void requestAuthorization(const uint8_t* uid, uint8_t uidLength);

private:
    MQTTManager* _mqtt;
    
    // Descriptive placeholder topic
    const char* AUTH_REQUEST_TOPIC = "zarzara/node/auth/request";
};

#endif
