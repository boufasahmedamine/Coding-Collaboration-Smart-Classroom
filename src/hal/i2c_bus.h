#ifndef I2C_BUS_H
#define I2C_BUS_H

#include <stdint.h>
#include <vector>

class I2cBus
{
public:
    virtual ~I2cBus() = default;

    virtual bool init(uint8_t sdaPin, uint8_t sclPin, uint32_t frequency = 100000) = 0;
    virtual bool writeByte(uint8_t address, uint8_t reg, uint8_t data) = 0;
    virtual bool writeBytes(uint8_t address, uint8_t reg, const uint8_t* data, size_t length) = 0;
    virtual bool readByte(uint8_t address, uint8_t reg, uint8_t& data) = 0;
    virtual bool readBytes(uint8_t address, uint8_t reg, uint8_t* data, size_t length) = 0;
    virtual std::vector<uint8_t> scanDevices() = 0;  // Returns list of found addresses
};

I2cBus* createI2cBus();

#endif // I2C_BUS_H