#ifndef IR_PROJECTOR_H
#define IR_PROJECTOR_H

class IRProjector
{
public:
    IRProjector(int pin);
    void begin();
    void turnOn();
    void turnOff();
    bool isOn() const { return _isOn; }

private:
    int _pin;
    bool _isOn;
};

#endif