#ifndef LDR_DRIVER_H
#define LDR_DRIVER_H

class LDRDriver
{
public:
    LDRDriver(int pin);

    void begin();
    void update();

    int getLightLevel();

private:
    int _pin;
    int _lightLevel;
};

#endif
