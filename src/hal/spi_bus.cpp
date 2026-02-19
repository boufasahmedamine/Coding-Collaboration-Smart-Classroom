#include "spi_bus.h"
#include <SPI.h>

class ArduinoSpiBus : public SpiBus
{
private:
    SPIClass* _spi;

public:
    ArduinoSpiBus() : _spi(&SPI) {}

    bool init(uint8_t mosiPin, uint8_t misoPin, uint8_t sckPin, uint32_t frequency = 1000000) override
    {
        _spi->begin(sckPin, misoPin, mosiPin, -1);  // -1 for no CS pin
        _spi->setFrequency(frequency);
        return true;
    }

    uint8_t transfer(uint8_t data) override
    {
        return _spi->transfer(data);
    }

    void transferBytes(const uint8_t* txData, uint8_t* rxData, size_t length) override
    {
        _spi->transferBytes(txData, rxData, length);
    }

    void beginTransaction() override
    {
        _spi->beginTransaction(SPISettings(_spi->getClockDivider(), MSBFIRST, SPI_MODE0));
    }

    void endTransaction() override
    {
        _spi->endTransaction();
    }
};

SpiBus* createSpiBus()
{
    static ArduinoSpiBus instance;
    return &instance;
}