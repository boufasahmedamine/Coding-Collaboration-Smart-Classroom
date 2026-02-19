#ifndef PN532_H
#define PN532_H

#include <stdint.h>
#include "hal/i2c_bus.h"

class Pn532
{
public:
    enum class State
    {
        IDLE,
        READING,
        ERROR
    };

    Pn532(I2cBus* i2cBus, uint8_t address = 0x24);
    ~Pn532();

    bool init();
    bool configureIrq(uint8_t irqPin);
    bool startRead();
    bool getUid(uint8_t* uid, size_t& length);
    State getState() const;

private:
    I2cBus* _i2c;
    uint8_t _address;
    State _state;
    bool _initialized;

    bool sendCommand(const uint8_t* cmd, size_t cmdLen);
    bool receiveResponse(uint8_t* resp, size_t& respLen, uint16_t timeout = 1000);
    bool wakeUp();
    bool setParameters();
};

#endif // PN532_H