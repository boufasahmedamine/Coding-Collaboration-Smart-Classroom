#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <freertos/queue.h>
#include "config/pins.h"
#include "drivers/doorlock/doorlock_driver.h"
#include "drivers/rfid/pn532.h"
#include "drivers/ld2410/ld2410_driver.h"
#include "drivers/ldr/ldr_driver.h"
#include "services/auth/access_control.h"
#include "services/attendance/attendance_manager.h"
#include "services/logging/log_manager.h"
#include "services/logging/sd_logger.h"
#include "services/automation/presence_service.h"
#include "services/automation/light_service.h"
#include "services/automation/automation_controller.h"
#include "services/automation/occupancy_logic.h"
#include "services/automation/lighting_logic.h"
#include "services/automation/projector_logic.h"
#include "drivers/actuators/lighting.h"
#include "drivers/actuators/ir_projector.h"
#include "communication/wifi_manager.h"
#include "communication/mqtt_manager.h"
#include "services/network/command_handler.h"
#include "services/system/heartbeat_service.h"
#include "system/state_machine.h"

// --- FreeRTOS Globals ---
SemaphoreHandle_t xMutex_SPIBus = NULL;
SemaphoreHandle_t xMutex_Serial = NULL;
SemaphoreHandle_t xMutex_StateMachine = NULL;
SemaphoreHandle_t xMutex_MQTT = NULL;

QueueHandle_t xQueue_SimCommand_LD2410 = NULL;
QueueHandle_t xQueue_SimCommand_Main = NULL;
// ------------------------

DoorLockDriver doorLock(PIN_DOOR_LOCK);

WiFiManager wifiManager("SSID", "PASSWORD");
MQTTManager mqttManager("192.168.1.100", 1883);
SDLogger sdLogger(5);
LogManager logManager(&mqttManager, &sdLogger);
StateMachine stateMachine(doorLock, 5400000); // 1.5 hours
AttendanceManager attendanceManager(&logManager, &stateMachine);
PN532Driver rfid;
LD2410Driver presenceSensor(16, 17);
LDRDriver lightSensor(34); // using pin 34 for analog input
PresenceService presenceService(&presenceSensor);
LightService lightService(&lightSensor);
AutomationController automationController(&presenceService, &lightService);

Lighting lightingDriver(PIN_LIGHTING);
IRProjector projectorDriver(PIN_PROJECTOR);

OccupancyLogic occupancy(presenceSensor);
LightingLogic lightingLogic(lightingDriver, occupancy, lightSensor);
ProjectorLogic projectorLogic(projectorDriver, stateMachine, occupancy);

HeartbeatService heartbeat(&mqttManager);
CommandHandler commandHandler(&stateMachine);
AccessControl accessControl;

// --- FreeRTOS Tasks ---
void vTaskNetwork(void *pvParameters) {
    for (;;) {
        wifiManager.update();
        
        if (xSemaphoreTake(xMutex_MQTT, portMAX_DELAY) == pdTRUE) {
            mqttManager.update();
            xSemaphoreGive(xMutex_MQTT);
        }
        
        heartbeat.update();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void vTaskSerialRouter(void *pvParameters) {
    for (;;) {
        char c = '\0';
        if (xSemaphoreTake(xMutex_Serial, portMAX_DELAY) == pdTRUE) {
            if (Serial.available()) {
                c = Serial.read();
            }
            xSemaphoreGive(xMutex_Serial);
        }

        if (c != '\0') {
            if (c == 'p' || c == 'n') {
                xQueueSend(xQueue_SimCommand_LD2410, &c, 0);
            } else if (c == 'c' || c == 'u' || c == 'm') {
                xQueueSend(xQueue_SimCommand_Main, &c, 0);
            }
        }
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}
void vTaskRFID(void *pvParameters) {
    uint8_t uid[7];
    uint8_t uidLength;
    for (;;) {
        bool cardRead = false;
        if (xSemaphoreTake(xMutex_SPIBus, portMAX_DELAY) == pdTRUE) {
            cardRead = rfid.readCard(uid, &uidLength);
            xSemaphoreGive(xMutex_SPIBus);
        }

        if (cardRead) {
            if (xSemaphoreTake(xMutex_Serial, portMAX_DELAY) == pdTRUE) {
                Serial.print("Card detected UID: ");
                for (int i = 0; i < uidLength; i++) {
                    Serial.print(uid[i], HEX);
                    Serial.print(" ");
                }
                Serial.println();
                xSemaphoreGive(xMutex_Serial);
            }

            if (accessControl.isAuthorized(uid, uidLength)) {
                if (xSemaphoreTake(xMutex_Serial, portMAX_DELAY) == pdTRUE) { Serial.println("[AUTH] ACCESS GRANTED"); xSemaphoreGive(xMutex_Serial); }
                if (xSemaphoreTake(xMutex_StateMachine, portMAX_DELAY) == pdTRUE) {
                    stateMachine.handleEvent(StateMachine::SystemEvent::ACCESS_GRANTED, uid, uidLength);
                    xSemaphoreGive(xMutex_StateMachine);
                }
            } else {
                if (xSemaphoreTake(xMutex_Serial, portMAX_DELAY) == pdTRUE) { Serial.println("[AUTH] ACCESS DENIED"); xSemaphoreGive(xMutex_Serial); }
            }
        }
        vTaskDelay(100 / portTICK_PERIOD_MS); // Poll RFID at 10Hz
    }
}

void vTaskLogging(void *pvParameters) {
    for (;;) {
        // Attendance Manager uses LogManager, which uses SDLogger (SPI).
        // Wrapping in SPI Mutex ensures we don't collide with the RFID SPI transaction.
        if (xSemaphoreTake(xMutex_SPIBus, portMAX_DELAY) == pdTRUE) {
            attendanceManager.update();
            xSemaphoreGive(xMutex_SPIBus);
        }
        vTaskDelay(500 / portTICK_PERIOD_MS); // Background logging checks at 2Hz
    }
}

void vTaskCoreLogic(void *pvParameters) {
    for (;;) {
        presenceSensor.update();
        lightSensor.update();
        presenceService.update();
        lightService.update();
        automationController.update();
        occupancy.update();
        lightingLogic.update();
        projectorLogic.update();
        
        // Safety lock around state machine updates if network task commands it concurrently
        if (xSemaphoreTake(xMutex_StateMachine, portMAX_DELAY) == pdTRUE) {
            stateMachine.update();
            xSemaphoreGive(xMutex_StateMachine);
        }

        if (presenceService.justBecameOccupied()) {
            if (xSemaphoreTake(xMutex_Serial, portMAX_DELAY) == pdTRUE) { Serial.println("[PRESENCE] Room became occupied"); xSemaphoreGive(xMutex_Serial); }
        }
        if (presenceService.justBecameEmpty()) {
            if (xSemaphoreTake(xMutex_Serial, portMAX_DELAY) == pdTRUE) { Serial.println("[PRESENCE] Room became empty"); xSemaphoreGive(xMutex_Serial); }
        }
        if (lightService.isDark()) {
            if (xSemaphoreTake(xMutex_Serial, portMAX_DELAY) == pdTRUE) { Serial.println("[LIGHT] Room is dark"); xSemaphoreGive(xMutex_Serial); }
        }

        // ---- Simulated Input Handler (from Queue) ----
        char c;
        if (xQueueReceive(xQueue_SimCommand_Main, &c, 0) == pdTRUE) {
            if (c == 'c') {
                rfid.simulateCardDetected();
            } else if (c == 'u') {
                if (xSemaphoreTake(xMutex_Serial, portMAX_DELAY) == pdTRUE) { Serial.println("[EVENT] Session Start Requested"); xSemaphoreGive(xMutex_Serial); }
                if (xSemaphoreTake(xMutex_StateMachine, portMAX_DELAY) == pdTRUE) {
                    stateMachine.handleEvent(StateMachine::SystemEvent::ACCESS_GRANTED);
                    xSemaphoreGive(xMutex_StateMachine);
                }
            } else if (c == 'm') {
                if (xSemaphoreTake(xMutex_Serial, portMAX_DELAY) == pdTRUE) { Serial.println("[SIM] Simulating MQTT 'unlock' command"); xSemaphoreGive(xMutex_Serial); }
                if (xSemaphoreTake(xMutex_StateMachine, portMAX_DELAY) == pdTRUE) {
                    commandHandler.handleCommand("unlock");
                    xSemaphoreGive(xMutex_StateMachine);
                }
            }
        }
        
        vTaskDelay(50 / portTICK_PERIOD_MS); // Logic engine runs at 20Hz
    }
}
// ----------------------

void setup() {
    Serial.begin(115200);
    delay(100);
    Serial.println("[BOOT] Smart Classroom Node Starting...");

    if (!rfid.init()) {
        Serial.println("[ERROR] PN532 initialization failed");
    } else {
        Serial.println("[INFO] PN532 RFID reader initialized");
    }

    doorLock.begin();
    lightingDriver.begin();
    projectorDriver.begin();
    
    // Safety: ensure actuators are off
    lightingDriver.turnOff();
    projectorDriver.turnOff();
    
    stateMachine.init();
    sdLogger.begin();

    // --- FreeRTOS Initialization ---
    xMutex_SPIBus = xSemaphoreCreateMutex();
    xMutex_Serial = xSemaphoreCreateMutex();
    xMutex_StateMachine = xSemaphoreCreateMutex();
    xMutex_MQTT = xSemaphoreCreateMutex();

    xQueue_SimCommand_LD2410 = xQueueCreate(10, sizeof(char));
    xQueue_SimCommand_Main = xQueueCreate(10, sizeof(char));

    presenceSensor.setSimQueue(xQueue_SimCommand_LD2410);

    xTaskCreatePinnedToCore(vTaskNetwork, "NetworkTask", 8192, NULL, 2, NULL, 0);
    xTaskCreatePinnedToCore(vTaskSerialRouter, "SerialRouter", 2048, NULL, 1, NULL, 0);

    // Core 1 Tasks (Application Logic + Sensors)
    xTaskCreatePinnedToCore(vTaskRFID, "RFIDTask", 4096, NULL, 3, NULL, 1);
    xTaskCreatePinnedToCore(vTaskLogging, "LoggingTask", 6144, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(vTaskCoreLogic, "CoreLogic", 8192, NULL, 2, NULL, 1);

    if (xSemaphoreTake(xMutex_Serial, portMAX_DELAY) == pdTRUE) {
        Serial.println("[INFO] Type 'u' to request unlock");
        xSemaphoreGive(xMutex_Serial);
    }
}

void loop() {
    // The main loop is intentionally left empty.
    // Under FreeRTOS, Arduino's loop() runs as a Core 1 task.
    // We delay it infinitely so the scheduler fully yields to our custom tasks.
    vTaskDelay(portMAX_DELAY);
}