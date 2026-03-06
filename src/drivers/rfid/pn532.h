#ifndef PN532_DRIVER_H
#define PN532_DRIVER_H

#include <Arduino.h>

class PN532Driver {
public:
    PN532Driver();

    bool init();

    bool readCard(uint8_t* uidBuffer, uint8_t* uidLength);

private:
    bool _initialized;
};

#endif
