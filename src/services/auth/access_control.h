#ifndef ACCESS_CONTROL_H
#define ACCESS_CONTROL_H

#include <Arduino.h>

class AccessControl {
public:
    AccessControl();

    bool isAuthorized(uint8_t* uid, uint8_t uidLength);

private:
    static const uint8_t AUTH_UID_COUNT = 3;

    const uint8_t _authorizedUIDs[AUTH_UID_COUNT][4] = {
        {0x04, 0x5F, 0x32, 0x1A}, // Teacher 1
        {0x12, 0x3B, 0x4C, 0xD5}, // Teacher 2
        {0xAA, 0xBB, 0xCC, 0xDD}  // Admin Master Card
    };
};

#endif
