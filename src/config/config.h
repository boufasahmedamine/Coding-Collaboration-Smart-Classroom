#ifndef CONFIG_H
#define CONFIG_H

// =============================================================================
// GPIO Pin Assignments (ESP32)
// =============================================================================

// Actuators
#define DOOR_LOCK_PIN          5   // MOSFET gate for 12V mag-lock
#define IR_LED_PIN             18  // IR transmitter for projector
#define LIGHTING_RELAY_PIN     19  // SSR control for classroom lights
#define BUZZER_PIN             23  // Piezo buzzer for access feedback
#define RGB_LED_R_PIN          25  // RGB LED red
#define RGB_LED_G_PIN          26  // RGB LED green
#define RGB_LED_B_PIN          27  // RGB LED blue

// Sensors/Inputs
#define RFID_IRQ_PIN           4   // PN532 interrupt pin
#define DOOR_CONTACT_PIN       12  // Reed switch for door status
#define EXIT_BUTTON_PIN        13  // Request-to-exit button
#define MANUAL_LIGHT_SWITCH1_PIN 14 // Wall switch 1
#define MANUAL_LIGHT_SWITCH2_PIN 15 // Wall switch 2
#define PROJECTOR_BUTTON_PIN   2   // Momentary button for projector

// Communication Buses
#define I2C_SDA_PIN            21  // I2C SDA (PN532, BH1750, DS3231)
#define I2C_SCL_PIN            22  // I2C SCL
#define SPI_MOSI_PIN           23  // SPI MOSI (if PN532 uses SPI)
#define SPI_MISO_PIN           19  // SPI MISO
#define SPI_SCK_PIN            18  // SPI SCK
#define SPI_CS_PIN             5   // SPI CS for PN532 (if SPI)
#define UART_RADAR_TX_PIN      17  // UART TX for LD2410 radar
#define UART_RADAR_RX_PIN      16  // UART RX for LD2410 radar

// =============================================================================
// Device Addresses (I2C)
// =============================================================================
#define PN532_I2C_ADDRESS      0x24  // PN532 default I2C address
#define BH1750_I2C_ADDRESS     0x23  // BH1750 lux sensor address
#define DS3231_I2C_ADDRESS     0x68  // DS3231 RTC address

// =============================================================================
// WiFi Configuration (Placeholders - Update for deployment)
// =============================================================================
#define WIFI_SSID              "Your_WiFi_SSID"
#define WIFI_PASSWORD          "Your_WiFi_Password"

// =============================================================================
// MQTT Configuration (Placeholders - Update for deployment)
// =============================================================================
#define MQTT_BROKER_IP         "192.168.1.100"  // Broker IP address
#define MQTT_BROKER_PORT       1883             // MQTT port
#define MQTT_CLIENT_ID         "smart_classroom_node_01"
#define MQTT_USERNAME          ""               // Leave empty if no auth
#define MQTT_PASSWORD          ""               // Leave empty if no auth

// MQTT Topics
#define MQTT_TOPIC_STATUS      "classroom/status"
#define MQTT_TOPIC_ACCESS      "classroom/access"
#define MQTT_TOPIC_PRESENCE    "classroom/presence"
#define MQTT_TOPIC_LIGHTING    "classroom/lighting"
#define MQTT_TOPIC_PROJECTOR   "classroom/projector"
#define MQTT_TOPIC_COMMAND     "classroom/command"

// =============================================================================
// Timing and Intervals (ms)
// =============================================================================
#define WIFI_RECONNECT_INTERVAL    5000  // WiFi reconnection attempt
#define MQTT_RECONNECT_INTERVAL    3000  // MQTT reconnection attempt
#define SENSOR_POLL_INTERVAL       2000  // Lux/radar polling
#define HEARTBEAT_INTERVAL         30000 // Status publish interval
#define DOOR_UNLOCK_DURATION       5000  // Auto-relock after unlock (ms)
#define RFID_READ_TIMEOUT          1000  // UID read timeout

// =============================================================================
// Queue and Buffer Sizes
// =============================================================================
#define MQTT_QUEUE_SIZE            10    // MQTT message queue
#define ACCESS_QUEUE_SIZE          5     // Access control queue
#define SENSOR_QUEUE_SIZE          5     // Sensor data queue
#define COMMAND_QUEUE_SIZE         5     // Incoming command queue

// =============================================================================
// Security and Safety
// =============================================================================
#define MAX_UID_LENGTH             7     // PN532 UID max length (bytes)
#define AUTHORIZED_UID_COUNT       10    // Max authorized UIDs in list

#endif // CONFIG_H