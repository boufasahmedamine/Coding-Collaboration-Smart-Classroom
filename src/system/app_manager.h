#ifndef APP_MANAGER_H
#define APP_MANAGER_H

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "services/auth/access_service.h"
#include "services/automation/environment_service.h"
#include "communication/wifi_manager.h"
#include "communication/mqtt_manager.h"
#include "services/system/heartbeat_service.h"
#include "services/network/dashboard_service.h"
#include "services/attendance/attendance_manager.h"
#include "drivers/sensors/button_driver.h"
#include "system/state_machine.h"
#include "services/automation/lighting_logic.h"

class AppManager {
public:
    AppManager(WiFiManager* wifi, MQTTManager* mqtt, 
               AccessService* access, EnvironmentService* env,
               HeartbeatService* hb, DashboardService* ds, 
               AttendanceManager* am, StateMachine* sm, LightingLogic* ll);

    void start();

private:
    WiFiManager* _wifi;
    MQTTManager* _mqtt;
    AccessService* _access;
    EnvironmentService* _env;
    HeartbeatService* _hb;
    DashboardService* _ds;
    AttendanceManager* _am;
    StateMachine* _stateMachine;
    LightingLogic* _lightingLogic;

    ButtonDriver _exitBtn;
    ButtonDriver _lightBtn;

    static void vTaskNetwork(void* pvParameters);
    static void vTaskRFID(void* pvParameters);
    static void vTaskCoreLogic(void* pvParameters);
    static void vTaskStatus(void* pvParameters);
    static void vTaskButtons(void* pvParameters);
    static void vTaskDiagnostics(void* pvParameters);
};

#endif
