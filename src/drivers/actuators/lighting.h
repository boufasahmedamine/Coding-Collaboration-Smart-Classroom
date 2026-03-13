#ifndef LIGHTING_H
#define LIGHTING_H

class Lighting
{
public:
    Lighting(int pin);
    void begin();
    void turnOn();
    void turnOff();

private:
    int _pin;
};

#endif