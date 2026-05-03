#include <Arduino.h>
#include <SPI.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#include "config/pins.h"
#include "config/mqtt_config.h"
#include "config/wifi_config.h"

// Drivers
#include "drivers/doorlock/doorlock_driver.h"
#include "drivers/pir/pir_driver.h"
#include "drivers/ldr/ldr_driver.h"
#include "drivers/rfid/pn532.h"
#include "drivers/actuators/lighting.h"
#include "drivers/actuators/ir_projector.h"

// Services
#include "services/auth/access_service.h"
#include "services/auth/auth_proxy.h"
#include "services/auth/local_auth_service.h"
#include "services/attendance/attendance_manager.h"
#include "services/logging/log_manager.h"
#include "services/automation/environment_service.h"
#include "services/automation/presence_service.h"
#include "services/automation/light_service.h"
#include "services/automation/automation_controller.h"
#include "services/automation/lighting_logic.h"
#include "services/automation/projector_logic.h"
#include "services/network/command_handler.h"
#include "services/network/time_service.h"
#include "services/network/dashboard_service.h"
#include "services/system/heartbeat_service.h"

// Communication & System
#include "communication/wifi_manager.h"
#include "communication/mqtt_manager.h"
#include "system/state_machine.h"
#include "system/diagnostics.h"
#include "system/app_manager.h"

// --- Global Sync ---
SemaphoreHandle_t xMutex_SPIBus = NULL;
SemaphoreHandle_t xMutex_Serial = NULL;
SemaphoreHandle_t xMutex_StateMachine = NULL;
SemaphoreHandle_t xMutex_MQTT = NULL;

// --- Hardware Instances ---
DoorLockDriver doorLock(PIN_DOOR_LOCK);
PN532Driver rfidOutside(PIN_PN532_OUT_CS, PIN_PN532_OUT_IRQ, "OUTSIDE");
PN532Driver rfidInside(PIN_PN532_IN_CS, PIN_PN532_IN_IRQ, "INSIDE");
PIRDriver presenceSensor(PIN_PIR);
LDRDriver lightSensor(PIN_LDR);
Lighting lightingDriver(PIN_LIGHTING);
IRProjector projectorDriver(PIN_PROJECTOR);

// --- Networking ---
WiFiManager wifiManager(WIFI_SSID, WIFI_PASSWORD);
MQTTManager mqttManager(MQTT_BROKER_IP, MQTT_PORT);

// --- Logical Services ---
AuthProxy authProxy(&mqttManager);
LogManager logManager(&mqttManager);
StateMachine stateMachine(doorLock, 30000, &mqttManager);
LocalAuthService localAuth;
TimeService timeService;
CommandHandler commandHandler(&stateMachine, &timeService, &lightingDriver, &projectorDriver, &authProxy);
AttendanceManager attendanceManager(&logManager, &stateMachine);
HeartbeatService heartbeat(&mqttManager);

// Automation Components
PresenceService presenceService(&presenceSensor);
LightService lightService(&lightSensor);
AutomationController automationController(&presenceService, &lightService);
LightingLogic lightingLogic(lightingDriver, presenceService, lightSensor);
ProjectorLogic projectorLogic(projectorDriver, stateMachine, presenceService);

// Dashboard & UI
DashboardService dashboardService(&mqttManager, &stateMachine, &presenceService, &lightingDriver, &projectorDriver);

// --- High-Level Managers ---
AccessService accessService(&rfidOutside, &rfidInside, &authProxy, &localAuth, &stateMachine);
EnvironmentService environmentService(&presenceSensor, &lightSensor, &presenceService, &lightService, &automationController, &lightingLogic, &projectorLogic);
AppManager appManager(&wifiManager, &mqttManager, &accessService, &environmentService, &heartbeat, &dashboardService, &attendanceManager);

void setup() {
    Serial.begin(115200);
    delay(100);

    // 1. Initialize Sync Primitives
    xMutex_SPIBus = xSemaphoreCreateMutex();
    xMutex_Serial = xSemaphoreCreateMutex();
    xMutex_StateMachine = xSemaphoreCreateMutex();
    xMutex_MQTT = xSemaphoreCreateMutex();

    // 2. Initialize Hardware & HAL
    SPI.begin(PIN_SPI_SCK, PIN_SPI_MISO, PIN_SPI_MOSI);
    Diagnostics::init();
    Diagnostics::logEvent("Smart Classroom Node Starting...");

    doorLock.begin();
    lightingDriver.begin();
    projectorDriver.begin();
    
    // Safety: ensure actuators are off
    lightingDriver.turnOff();
    projectorDriver.turnOff();

    // 3. Initialize Domain Services
    authProxy.setStateMachine(&stateMachine);
    stateMachine.init();
    wifiManager.begin();
    mqttManager.begin();
    mqttManager.setCommandHandler(&commandHandler);

    // 4. Initialize Composite Services (Access & Environment)
    accessService.init();
    environmentService.init();

    // 5. Hand over control to AppManager
    appManager.start();

    Diagnostics::logEvent("System fully booted. Waiting for events.");
}

void loop() {
    // Under FreeRTOS, loop() runs as a priority-1 task on Core 1.
    // We handle the Serial Router logic here to keep it out of setup().
    char c = '\0';
    if (xSemaphoreTake(xMutex_Serial, 10 / portTICK_PERIOD_MS) == pdTRUE) {
        if (Serial.available()) {
            c = Serial.read();
        }
        xSemaphoreGive(xMutex_Serial);
    }

    if (c != '\0') {
        switch (c) {
            case 'r':
                Diagnostics::logEvent("[MAINT] System Rebooting...");
                delay(500);
                ESP.restart();
                break;
            case 'l':
                Diagnostics::logEvent("[MAINT] Manual Door Pulse Triggered");
                doorLock.unlock(); 
                delay(3000);
                doorLock.lock();
                break;
            case 'v':
                Diagnostics::setDashboardVisible(!Diagnostics::isDashboardVisible());
                break;
            case 's':
                Diagnostics::logEvent("[MAINT] Forcing State Sync");
                dashboardService.update(true);
                break;
        }
    }

    vTaskDelay(100 / portTICK_PERIOD_MS);
}