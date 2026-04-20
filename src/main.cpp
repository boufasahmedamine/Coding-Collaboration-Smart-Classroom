#include <Arduino.h>
#include <SPI.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <freertos/queue.h>
#include "config/pins.h"
#include "drivers/doorlock/doorlock_driver.h"
#include "drivers/pir/pir_driver.h"
#include "drivers/ldr/ldr_driver.h"
#include "services/auth/auth_proxy.h"
#include "services/attendance/attendance_manager.h"
#include "services/logging/log_manager.h"
#include "services/automation/presence_service.h"
#include "services/automation/light_service.h"
#include "services/automation/automation_controller.h"
#include "services/automation/lighting_logic.h"
#include "services/automation/projector_logic.h"
#include "drivers/actuators/lighting.h"
#include "drivers/actuators/ir_projector.h"
#include "communication/wifi_manager.h"
#include "communication/mqtt_manager.h"
#include "services/network/command_handler.h"
#include "services/network/time_service.h"
#include "services/system/heartbeat_service.h"
#include "system/state_machine.h"
#include "system/diagnostics.h"
#include "drivers/rfid/pn532.h"

// --- FreeRTOS Globals ---
SemaphoreHandle_t xMutex_SPIBus = NULL;
SemaphoreHandle_t xMutex_Serial = NULL;
SemaphoreHandle_t xMutex_StateMachine = NULL;
SemaphoreHandle_t xMutex_MQTT = NULL;

QueueHandle_t xQueue_SimCommand_Main = NULL;
// ------------------------

DoorLockDriver doorLock(PIN_DOOR_LOCK);

WiFiManager wifiManager("SSID", "PASSWORD");
MQTTManager mqttManager("192.168.1.100", 1883);
LogManager logManager(&mqttManager);
StateMachine stateMachine(doorLock, 5400000); // 1.5 hours
AttendanceManager attendanceManager(&logManager, &stateMachine);
PN532Driver rfid(PIN_PN532_CS);
PIRDriver presenceSensor(PIN_PIR);
LDRDriver lightSensor(PIN_LDR);
PresenceService presenceService(&presenceSensor);
LightService lightService(&lightSensor);
AutomationController automationController(&presenceService, &lightService);
TimeService timeService;
AuthProxy authProxy(&mqttManager);

Lighting lightingDriver(PIN_LIGHTING);
IRProjector projectorDriver(PIN_PROJECTOR);

LightingLogic lightingLogic(lightingDriver, presenceService, lightSensor);
ProjectorLogic projectorLogic(projectorDriver, stateMachine, presenceService);

HeartbeatService heartbeat(&mqttManager);
CommandHandler commandHandler(&stateMachine, &timeService);

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
             if (c == 'c' || c == 'u' || c == 'm') {
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
            String uidStr = "UID:";
            for (int i = 0; i < uidLength; i++) {
                uidStr += String(uid[i], HEX);
            }

            Diagnostics::logEvent("RFID: Detected " + uidStr);
            
            // Request Authorization from Server
            authProxy.requestAuthorization(uid, uidLength);

            if (xSemaphoreTake(xMutex_StateMachine, portMAX_DELAY) == pdTRUE) {
                stateMachine.handleEvent(StateMachine::SystemEvent::RFID_READ, uid, uidLength);
                xSemaphoreGive(xMutex_StateMachine);
            }
        }
        vTaskDelay(100 / portTICK_PERIOD_MS); // Poll RFID at 10Hz
    }
}

void vTaskStatus(void *pvParameters) {
    for (;;) {
        // Attendance Manager updates session logs based on state machine
        attendanceManager.update();
        
        vTaskDelay(1000 / portTICK_PERIOD_MS); // Status updates at 1Hz
    }
}

void vTaskDiagnostics(void *pvParameters) {
    for (;;) {
#if ENABLE_DIAGNOSTICS_DASHBOARD
        Diagnostics::updateTable();
#endif
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void vTaskCoreLogic(void *pvParameters) {
    for (;;) {
        presenceSensor.update();
        lightSensor.update();
        presenceService.update();
        lightService.update();
        automationController.update();
        lightingLogic.update();
        projectorLogic.update();
        
        // Safety lock around state machine updates if network task commands it concurrently
        if (xSemaphoreTake(xMutex_StateMachine, portMAX_DELAY) == pdTRUE) {
            stateMachine.update();
            xSemaphoreGive(xMutex_StateMachine);
        }

        if (presenceService.justBecameOccupied()) {
            Diagnostics::logEvent("PRESENCE: Room became occupied");
        }
        if (presenceService.justBecameEmpty()) {
            Diagnostics::logEvent("PRESENCE: Room became empty");
        }
        if (lightService.isDark()) {
            // Can be noisy, omitted or log specific changes
        }

        // ---- Simulated Input Handler (from Queue) ----
        char c;
        if (xQueueReceive(xQueue_SimCommand_Main, &c, 0) == pdTRUE) {
            if (c == 'c') {
                rfid.simulateCardDetected();
            } else if (c == 'u') {
                Diagnostics::logEvent("SIM: Session Start Requested");
                if (xSemaphoreTake(xMutex_StateMachine, portMAX_DELAY) == pdTRUE) {
                    stateMachine.handleEvent(StateMachine::SystemEvent::ACCESS_GRANTED);
                    xSemaphoreGive(xMutex_StateMachine);
                }
            } else if (c == 'm') {
                Diagnostics::logEvent("SIM: MQTT unlock command");
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

    // Initialize global SPI Bus
    SPI.begin(PIN_SPI_SCK, PIN_SPI_MISO, PIN_SPI_MOSI);

    Diagnostics::init();
    Diagnostics::logEvent("Smart Classroom Node Starting...");

    rfid.init();

    doorLock.begin();
    lightingDriver.begin();
    projectorDriver.begin();
    presenceSensor.begin();
    lightSensor.begin();
    
    // Safety: ensure actuators are off
    lightingDriver.turnOff();
    projectorDriver.turnOff();
    
    stateMachine.init();
    wifiManager.begin();
    mqttManager.begin();
    mqttManager.setCommandHandler(&commandHandler);

    // --- FreeRTOS Initialization ---
    xMutex_SPIBus = xSemaphoreCreateMutex();
    xMutex_Serial = xSemaphoreCreateMutex();
    xMutex_StateMachine = xSemaphoreCreateMutex();
    xMutex_MQTT = xSemaphoreCreateMutex();

    xQueue_SimCommand_Main = xQueueCreate(10, sizeof(char));

    xTaskCreatePinnedToCore(vTaskNetwork, "NetworkTask", 8192, NULL, 2, NULL, 0);
    xTaskCreatePinnedToCore(vTaskSerialRouter, "SerialRouter", 2048, NULL, 1, NULL, 0);

    // Core 1 Tasks (Application Logic + Sensors)
    xTaskCreatePinnedToCore(vTaskRFID, "RFIDTask", 4096, NULL, 3, NULL, 1);
    xTaskCreatePinnedToCore(vTaskStatus, "StatusTask", 6144, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(vTaskCoreLogic, "CoreLogic", 8192, NULL, 2, NULL, 1);
#if ENABLE_DIAGNOSTICS_DASHBOARD
    xTaskCreatePinnedToCore(vTaskDiagnostics, "Diagnostics", 4096, NULL, 1, NULL, 1);
#endif

    Diagnostics::logEvent("System fully booted. waiting for events.");
}

void loop() {
    // The main loop is intentionally left empty.
    // Under FreeRTOS, Arduino's loop() runs as a Core 1 task.
    // We delay it infinitely so the scheduler fully yields to our custom tasks.
    vTaskDelay(portMAX_DELAY);
}