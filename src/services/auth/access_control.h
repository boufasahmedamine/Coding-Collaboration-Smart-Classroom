#ifndef ACCESS_CONTROL_H
#define ACCESS_CONTROL_H

#include <Arduino.h>

class AccessControl {
public:
    AccessControl();

    bool isAuthorized(uint8_t* uid, uint8_t uidLength);

private:
    static const uint8_t AUTH_UID_COUNT = 1;

    const uint8_t _authorizedUIDs[AUTH_UID_COUNT][4] = {
        {0xDE, 0xAD, 0xBE, 0xEF}  // simulated authorized card
    };
};

#endif
