#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

#include "config/config.h"
#include "config/system_config.h"
#include "hal/gpio_hal.h"
#include "hal/i2c_bus.h"
#include "drivers/actuators/door_lock.h"
#include "drivers/rfid/pn532.h"
#include "services/auth/access_control.h"

// Global HAL instances
GpioHal* gpio = nullptr;
I2cBus* i2c = nullptr;

// Drivers
DoorLock* door = nullptr;
Pn532* rfid = nullptr;

// Services
AccessControlService* accessControl = nullptr;

// Queues
QueueHandle_t accessQueue = nullptr;
QueueHandle_t commandQueue = nullptr;

// Task Handles
TaskHandle_t communicationTaskHandle = nullptr;
TaskHandle_t ioTaskHandle = nullptr;
TaskHandle_t actuatorTaskHandle = nullptr;

// ISR for RFID
void IRAM_ATTR rfidIsr()
{
    // Signal IO Task (simplified; use semaphore in real impl)
    xTaskNotifyFromISR(ioTaskHandle, 0, eNoAction, NULL);
}

// Communication Task (Core 0)
void communicationTask(void* pvParameters)
{
    // Placeholder: WiFi and MQTT loop
    while (true) {
        // Connect WiFi
        // Connect MQTT
        // Publish status
        // Receive commands
        vTaskDelay(pdMS_TO_TICKS(HEARTBEAT_INTERVAL));
    }
}

// IO Task (Core 1)
void ioTask(void* pvParameters)
{
    while (true) {
        // Wait for RFID interrupt
        uint32_t notification;
        if (xTaskNotifyWait(0, 0, &notification, portMAX_DELAY)) {
            // Read UID
            if (rfid->startRead()) {
                uint8_t uid[10];
                size_t length;
                if (rfid->getUid(uid, length)) {
                    // Send to AccessControlService
                    AccessEvent event = accessControl->processUid(uid, length);
                    xQueueSend(accessQueue, &event, 0);
                }
            }
        }
    }
}

// Actuator Task (Core 1)
void actuatorTask(void* pvParameters)
{
    while (true) {
        AccessEvent event;
        if (xQueueReceive(accessQueue, &event, portMAX_DELAY)) {
            if (event.authenticated) {
                door->unlock();
                vTaskDelay(pdMS_TO_TICKS(DOOR_UNLOCK_DURATION));
                door->lock();
            }
            // Feedback: buzzer, LED
        }
    }
}

void setup()
{
    Serial.begin(SERIAL_BAUD_RATE);

    // Init HALs
    gpio = createGpioHal();
    i2c = createI2cBus();
    i2c->init(I2C_SDA_PIN, I2C_SCL_PIN);

    // Init Drivers
    door = new DoorLock(gpio, DOOR_LOCK_PIN, true);  // Active high
    if (!door->init()) {
        Serial.println("Door init failed");
        while (true);
    }

    rfid = new Pn532(i2c, PN532_I2C_ADDRESS);
    if (!rfid->init()) {
        Serial.println("RFID init failed");
        while (true);
    }
    rfid->configureIrq(RFID_IRQ_PIN);
    attachInterrupt(digitalPinToInterrupt(RFID_IRQ_PIN), rfidIsr, FALLING);

    // Init Services
    accessControl = new AccessControlService();
    if (!accessControl->init()) {
        Serial.println("AccessControl init failed");
        while (true);
    }

    // Create Queues
    accessQueue = xQueueCreate(ACCESS_QUEUE_SIZE, sizeof(AccessEvent));
    commandQueue = xQueueCreate(COMMAND_QUEUE_SIZE, sizeof(int));  // Placeholder

    // Create Tasks
    xTaskCreatePinnedToCore(communicationTask, "Comm", TASK_STACK_COMMUNICATION, NULL,
                            TASK_PRIORITY_COMMUNICATION, &communicationTaskHandle, TASK_CORE_COMMUNICATION);
    xTaskCreatePinnedToCore(ioTask, "IO", TASK_STACK_IO, NULL,
                            TASK_PRIORITY_IO, &ioTaskHandle, TASK_CORE_IO);
    xTaskCreatePinnedToCore(actuatorTask, "Actuator", TASK_STACK_ACTUATOR, NULL,
                            TASK_PRIORITY_ACTUATOR, &actuatorTaskHandle, TASK_CORE_ACTUATOR);

    Serial.println("System initialized. Tasks running.");
}

void loop()
{
    // Empty: All logic in tasks
    vTaskDelay(portMAX_DELAY);
}
