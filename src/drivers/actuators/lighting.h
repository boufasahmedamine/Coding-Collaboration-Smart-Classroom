#ifndef LIGHTING_H
#define LIGHTING_H

class Lighting
{
public:
    Lighting(int pin);
    void begin();
    void turnOn();
    void turnOff();
    bool isOn() const { return _isOn; }

private:
    int _pin;
    bool _isOn;
};

#endif