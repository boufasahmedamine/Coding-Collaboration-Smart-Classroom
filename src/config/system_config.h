#ifndef SYSTEM_CONFIG_H
#define SYSTEM_CONFIG_H

// =============================================================================
// FreeRTOS Task Configuration
// =============================================================================

// Task Priorities (Higher number = higher priority)
#define TASK_PRIORITY_COMMUNICATION  3  // WiFi/MQTT (Core 0, high priority)
#define TASK_PRIORITY_IO             2  // RFID/Sensors (Core 1)
#define TASK_PRIORITY_ACTUATOR       2  // Door/IR/Lighting (Core 1)
#define TASK_PRIORITY_LOW            1  // Future low-priority tasks

// Task Stack Sizes (words, 1 word = 4 bytes on ESP32)
#define TASK_STACK_COMMUNICATION     4096  // 16KB for WiFi/MQTT
#define TASK_STACK_IO                3072  // 12KB for sensors/RFID
#define TASK_STACK_ACTUATOR          2048  // 8KB for actuators

// Task Core Affinity
#define TASK_CORE_COMMUNICATION      0    // Core 0 for network
#define TASK_CORE_IO                 1    // Core 1 for I/O
#define TASK_CORE_ACTUATOR           1    // Core 1 for actuators

// =============================================================================
// Queue Configuration
// =============================================================================

// Queue Item Sizes (bytes)
#define QUEUE_ITEM_SIZE_MQTT         sizeof(MqttMessage)  // Define in communication
#define QUEUE_ITEM_SIZE_ACCESS       sizeof(AccessEvent)  // Define in services
#define QUEUE_ITEM_SIZE_SENSOR       sizeof(SensorData)   // Define in drivers
#define QUEUE_ITEM_SIZE_COMMAND      sizeof(Command)      // Define in communication

// =============================================================================
// System Limits and Timeouts
// =============================================================================
#define SYSTEM_INIT_TIMEOUT          10000  // Max init time (ms)
#define WATCHDOG_TIMEOUT             30000  // ESP32 watchdog (ms)
#define SERIAL_BAUD_RATE             115200 // Serial monitor

// =============================================================================
// Logging and Debugging
// =============================================================================
#define ENABLE_SERIAL_LOGGING        1  // 1 = enable, 0 = disable
#define LOG_LEVEL                    2  // 0=error, 1=warn, 2=info, 3=debug

// =============================================================================
// Power and Safety
// =============================================================================
#define ENABLE_WATCHDOG              1  // Enable ESP32 task watchdog
#define DEFAULT_DOOR_STATE           1  // 1=locked (safe default)

// =============================================================================
// Feature Flags (Enable/Disable for testing)
// =============================================================================
#define FEATURE_WIFI                 1
#define FEATURE_MQTT                 1
#define FEATURE_RFID                 1
#define FEATURE_DOOR_LOCK            1
#define FEATURE_LIGHTING             0  // Enable in Phase 2
#define FEATURE_PROJECTOR            0  // Enable in Phase 2
#define FEATURE_SENSORS              0  // Enable in Phase 2

#endif // SYSTEM_CONFIG_H