#ifndef LD2410_DRIVER_H
#define LD2410_DRIVER_H

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <ld2410.h>

class LD2410Driver
{
public:
    LD2410Driver(int rxPin, int txPin);

    void begin();
    void update();

    bool isPresenceDetected();
    void setSimQueue(QueueHandle_t q);

private:
    int _rxPin;
    int _txPin;

    bool _presence;
    bool _initialized;
    
    QueueHandle_t _simQueue;
    ld2410 _radar;
};

#endif
