#ifndef BUILD_FLAGS_H
#define BUILD_FLAGS_H

// =============================================================================
// Compiler Build Flags (PlatformIO integration)
// =============================================================================
// Note: Add these to platformio.ini under [env:esp32doit-devkit-v1] > build_flags
// Example:
// build_flags =
//     -D CORE_DEBUG_LEVEL=2
//     -D CONFIG_FREERTOS_HZ=1000
//     -D CONFIG_ARDUINO_LOOP_STACK_SIZE=8192

// FreeRTOS Configuration
#define CONFIG_FREERTOS_HZ              1000  // Tick rate (Hz)
#define CONFIG_ARDUINO_LOOP_STACK_SIZE  8192  // Arduino loop stack (bytes)

// ESP32 Specific
#define CORE_DEBUG_LEVEL                2     // 0=none, 1=error, 2=warn, 3=info, 4=debug
#define CONFIG_ASYNC_TCP_RUNNING_CORE   1     // Async TCP on Core 1

// Memory and Optimization
#define CONFIG_MBEDTLS_INTERNAL_MEM_ALLOC 1   // Internal memory alloc for TLS

// WiFi and Network
#define CONFIG_ESP32_WIFI_STATIC_RX_BUFFER_NUM 16
#define CONFIG_ESP32_WIFI_DYNAMIC_RX_BUFFER_NUM 32
#define CONFIG_ESP32_WIFI_STATIC_TX_BUFFER_NUM 16
#define CONFIG_ESP32_WIFI_DYNAMIC_TX_BUFFER_NUM 32

#endif // BUILD_FLAGS_H