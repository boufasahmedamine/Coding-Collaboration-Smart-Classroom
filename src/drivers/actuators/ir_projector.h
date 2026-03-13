#ifndef IR_PROJECTOR_H
#define IR_PROJECTOR_H

class IRProjector
{
public:
    IRProjector(int pin);
    void begin();
    void turnOn();
    void turnOff();

private:
    int _pin;
};

#endif