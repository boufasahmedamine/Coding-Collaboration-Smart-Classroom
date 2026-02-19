#include "gpio_hal.h"
#include <Arduino.h>

class ArduinoGpioHal : public GpioHal
{
public:
    void configure(uint8_t pin, Direction dir) override
    {
        pinMode(pin, dir == Direction::Out ? OUTPUT : INPUT);
    }

    void write(uint8_t pin, bool level) override
    {
        digitalWrite(pin, level ? HIGH : LOW);
    }

    bool read(uint8_t pin) override
    {
        return digitalRead(pin) == HIGH;
    }
};

/*
    Factory function.
    Keeps main.cpp clean and avoids global objects.
*/
GpioHal* createGpioHal()
{
    static ArduinoGpioHal instance;
    return &instance;
}
