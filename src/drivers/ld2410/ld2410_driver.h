#ifndef LD2410_DRIVER_H
#define LD2410_DRIVER_H

class LD2410Driver
{
public:
    LD2410Driver(int rxPin, int txPin);

    void begin();
    void update();

    bool isPresenceDetected();

private:
    int _rxPin;
    int _txPin;

    bool _presence;
};

#endif
