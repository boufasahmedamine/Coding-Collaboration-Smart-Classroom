# ZARZARA Smart Classroom - Firmware Context

## Project Overview
Firmware for ESP32-WROOM-32U node handling RFID Access, mmWave Presence (LD2410), Lux Sensing (VEML7700), IR Control, and MQTT Comm.

## Architecture Laws (Strict Enforcement)
This project follows a **Strict Layered Architecture**. Do not allow cross-layer contamination.
1. **HAL**: Hardware abstraction only. Wraps ESP32 SDK. No logic.
2. **Drivers**: Device-specific (PN532, LD2410, etc.). No business rules or auth decisions.
3. **Services**: Business logic (Auth, Automation). Never access GPIO/I2C/SPI directly.
4. **Communication**: WiFi/MQTT only. No actuator control.
5. **System**: Scheduler and State Machine. Orchestrates services.
6. **Main**: Initialization and task registration only. No logic.

## Non-Negotiable Rules
- **No Blocking:** Never use `delay()`. Use non-blocking design/FreeRTOS tasks.
- **Safety First:** Actuators must default to SAFE (locked/off) states.
- **No Globals:** Avoid global variables; use dependency injection or structured singletons.
- **No Hardcoding:** Secrets (WiFi/MQTT) must be in a separate, untracked config.
- **Memory:** Prefer static allocation. Avoid `String` class; use `char` buffers.

## Hardware Mapping
- **MCU:** ESP32-WROOM-32U
- **Inputs:** 2x PN532 (SPI), LD2410 (UART), VEML7700 (I2C), DS3231 (I2C), Reed Switch, Exit Button.
- **Outputs:** Maglock (MOSFET), SSR (Lighting), IR LED (Transistor), Buzzers, RGB LEDs.
- **Storage:** MicroSD (SPI).

## Directory Structure
Follow this structure for all new files:
- `src/config/`: Pins and thresholds.
- `src/hal/`: Bus abstractions (I2C, SPI, UART).
- `src/drivers/`: Device-specific classes (rfid, sensors, actuators).
- `src/services/`: Logic (auth, attendance, automation, logging).
- `src/communication/`: Networking stack.
- `src/system/`: Task scheduler and global state machine.
- `tests/`: Hardware simulation and unit tests.

## State Machine States
- `BOOT`, `IDLE`, `OCCUPIED`, `ACCESS_GRANTED`, `ERROR`, `OFFLINE`.