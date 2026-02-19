#include "i2c_bus.h"
#include <Wire.h>
#include <vector>

class ArduinoI2cBus : public I2cBus
{
public:
    bool init(uint8_t sdaPin, uint8_t sclPin, uint32_t frequency = 100000) override
    {
        Wire.begin(sdaPin, sclPin);
        Wire.setClock(frequency);
        return true;  // Wire.begin() doesn't return error status
    }

    bool writeByte(uint8_t address, uint8_t reg, uint8_t data) override
    {
        Wire.beginTransmission(address);
        Wire.write(reg);
        Wire.write(data);
        return Wire.endTransmission() == 0;
    }

    bool writeBytes(uint8_t address, uint8_t reg, const uint8_t* data, size_t length) override
    {
        Wire.beginTransmission(address);
        Wire.write(reg);
        for (size_t i = 0; i < length; ++i) {
            Wire.write(data[i]);
        }
        return Wire.endTransmission() == 0;
    }

    bool readByte(uint8_t address, uint8_t reg, uint8_t& data) override
    {
        Wire.beginTransmission(address);
        Wire.write(reg);
        if (Wire.endTransmission() != 0) return false;

        Wire.requestFrom(address, (uint8_t)1);
        if (Wire.available() == 1) {
            data = Wire.read();
            return true;
        }
        return false;
    }

    bool readBytes(uint8_t address, uint8_t reg, uint8_t* data, size_t length) override
    {
        Wire.beginTransmission(address);
        Wire.write(reg);
        if (Wire.endTransmission() != 0) return false;

        Wire.requestFrom(address, (uint8_t)length);
        for (size_t i = 0; i < length; ++i) {
            if (Wire.available()) {
                data[i] = Wire.read();
            } else {
                return false;
            }
        }
        return true;
    }

    std::vector<uint8_t> scanDevices() override
    {
        std::vector<uint8_t> devices;
        for (uint8_t address = 1; address < 127; ++address) {
            Wire.beginTransmission(address);
            if (Wire.endTransmission() == 0) {
                devices.push_back(address);
            }
        }
        return devices;
    }
};

I2cBus* createI2cBus()
{
    static ArduinoI2cBus instance;
    return &instance;
}