#ifndef PIR_DRIVER_H
#define PIR_DRIVER_H

class PIRDriver
{
public:
    PIRDriver(int pin);

    void begin();
    void update();

    bool isMotionDetected();

private:
    int _pin;
    bool _motion;
};

#endif
