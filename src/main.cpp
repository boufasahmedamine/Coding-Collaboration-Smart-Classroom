#include <Arduino.h>
#include "hal/gpio_hal.h"
#include "drivers/actuators/door_lock.h"

static const uint8_t DOOR_PIN = 5;     // placeholder
static const bool ACTIVE_HIGH = true;  // adjust after hardware test

GpioHal* gpio = nullptr;
DoorLock* door = nullptr;

unsigned long lastToggle = 0;
bool unlocked = false;

void setup()
{
    Serial.begin(115200);

    gpio = createGpioHal();
    door = new DoorLock(gpio, DOOR_PIN, ACTIVE_HIGH);

    if (!door->init())
    {
        Serial.println("Door init failed");
        while (true); // Fail-safe halt
    }

    Serial.println("Door initialized. Starting simulation.");
}

void loop()
{
    unsigned long now = millis();

    if (now - lastToggle > 3000)
    {
        lastToggle = now;

        if (unlocked)
        {
            door->lock();
            Serial.println("Door LOCKED");
        }
        else
        {
            door->unlock();
            Serial.println("Door UNLOCKED");
        }

        unlocked = !unlocked;
    }
}
