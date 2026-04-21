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
#include "services/network/dashboard_service.h"
#include "config/mqtt_config.h"

// --- FreeRTOS Globals ---
SemaphoreHandle_t xMutex_SPIBus = NULL;
SemaphoreHandle_t xMutex_Serial = NULL;
SemaphoreHandle_t xMutex_StateMachine = NULL;
SemaphoreHandle_t xMutex_MQTT = NULL;

// ------------------------

DoorLockDriver doorLock(PIN_DOOR_LOCK);

WiFiManager wifiManager("SSID", "PASSWORD");
MQTTManager mqttManager(MQTT_BROKER_IP, MQTT_PORT);
LogManager logManager(&mqttManager);
StateMachine stateMachine(doorLock, 5400000); // 1.5 hours
AttendanceManager attendanceManager(&logManager, &stateMachine);
PN532Driver rfidOutside(PIN_PN532_OUT_CS, "OUTSIDE");
PN532Driver rfidInside(PIN_PN532_IN_CS, "INSIDE");
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
DashboardService dashboardService(&mqttManager, &stateMachine, &presenceService, &lightingDriver, &projectorDriver);
CommandHandler commandHandler(&stateMachine, &timeService, &lightingDriver, &projectorDriver);

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
            switch (c) {
                case 'r':
                    Diagnostics::logEvent("[MAINT] System Rebooting...");
                    vTaskDelay(500 / portTICK_PERIOD_MS);
                    ESP.restart();
                    break;
                case 'l':
                    Diagnostics::logEvent("[MAINT] Manual Door Pulse Triggered");
                    doorLock.unlock(); 
                    break;
                case 'v':
                    Diagnostics::setDashboardVisible(!Diagnostics::isDashboardVisible());
                    break;
                case 's':
                    Diagnostics::logEvent("[MAINT] Forcing State Sync");
                    dashboardService.update(true); // Force broadcast
                    break;
            }
        }
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}
void vTaskRFID(void *pvParameters) {
    uint8_t uid[7];
    uint8_t uidLength;
    for (;;) {
        // --- 1. Poll Outside Reader (Access Control) ---
        bool outsideRead = false;
        if (xSemaphoreTake(xMutex_SPIBus, portMAX_DELAY) == pdTRUE) {
            outsideRead = rfidOutside.readCard(uid, &uidLength);
            xSemaphoreGive(xMutex_SPIBus);
        }

        if (outsideRead) {
            Diagnostics::logEvent("[RFID] Outside scan detected");
            authProxy.requestAuthorization(uid, uidLength);
            if (xSemaphoreTake(xMutex_StateMachine, portMAX_DELAY) == pdTRUE) {
                stateMachine.handleEvent(StateMachine::SystemEvent::RFID_READ, uid, uidLength);
                xSemaphoreGive(xMutex_StateMachine);
            }
        }

        // --- 2. Poll Inside Reader (Attendance / Control) ---
        bool insideRead = false;
        vTaskDelay(20 / portTICK_PERIOD_MS); // Brief yield to allow other SPI users
        if (xSemaphoreTake(xMutex_SPIBus, portMAX_DELAY) == pdTRUE) {
            insideRead = rfidInside.readCard(uid, &uidLength);
            xSemaphoreGive(xMutex_SPIBus);
        }

        if (insideRead) {
            Diagnostics::logEvent("[RFID] Inside scan detected");
            authProxy.requestAttendance(uid, uidLength);
            // Inside scans don't usually change the entry state machine but log events
        }

        vTaskDelay(100 / portTICK_PERIOD_MS); 
    }
}

void vTaskStatus(void *pvParameters) {
    for (;;) {
        attendanceManager.update();
        dashboardService.update(); // Synchronous update with status task
        
        vTaskDelay(1000 / portTICK_PERIOD_MS); 
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

    bool outReady = rfidOutside.init();
    bool inReady = rfidInside.init();
    
    Diagnostics::setRFIDStatusOut(outReady ? "READY" : "HW NOT FOUND");
    Diagnostics::setRFIDStatusIn(inReady ? "READY" : "HW NOT FOUND");

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