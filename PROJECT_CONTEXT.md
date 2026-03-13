# ZARZARA Smart Classroom - Firmware Context

## Project Overview
Firmware for ESP32-WROOM-32U node handling RFID Access, mmWave Presence (LD2410), Lux Sensing (simple LDRs), IR Control, and MQTT Comm.

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
- **Inputs:** 2x PN532 (SPI), LD2410 (UART), LDRs (Analog), DS3231 (I2C), Reed Switch, Exit Button.
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


ZARZARA Smart Classroom – Firmware AI Context
Project Summary :
Firmware for a Smart Classroom Node built on ESP32-WROOM-32U.

Main capabilities:

RFID access control (PN532)

Attendance tracking

Presence detection (LD2410 mmWave)

Ambient light sensing (LDRs)

Door lock control (MOSFET maglock)

Lighting automation

Projector IR control

WiFi + MQTT communication

Optional RTC (DS3231) and MicroSD logging

The firmware must remain modular, safe, and maintainable.

Strict Architecture

The project follows a strict layered architecture.

Dependency direction (MUST NOT be violated):

config
  ↓
hal
  ↓
drivers
  ↓
services
  ↓
system
  ↓
main

communication runs parallel and interacts only through services or system.

Layer Responsibilities

config

compile-time configuration

pins

thresholds

feature flags

hal

ESP32 hardware abstraction

GPIO, I2C, SPI, UART

wraps ESP-IDF / Arduino functions

contains NO device logic

drivers

control specific devices

examples: PN532, LD2410, LDRs

may call HAL

MUST NOT contain business logic

services

application logic

authentication

automation

attendance

NEVER access hardware directly

communication

WiFi connection

MQTT client

reconnect logic

NO actuator control

system

scheduler

state machine

orchestrates services

main

initializes modules

injects dependencies

starts scheduler

NO logic allowed

Forbidden Dependencies

Services MUST NOT include:

hal/*
Arduino.h
Wire.h
SPI.h

Drivers MUST NOT include:

services/*
system/*
communication/*

Communication MUST NOT include:

drivers/actuators/*

main.cpp MUST NOT contain:

business logic
authorization checks
hardware manipulation
Coding Rules
Non-Blocking

Never use:

delay()
while(true) blocking loops

Use:

FreeRTOS tasks
non-blocking timers
scheduler ticks
Safety Rules

Actuators must default to SAFE state.

Examples:

door lock → LOCKED
lighting → OFF
projector IR → OFF

Never unlock a door before authentication is confirmed.

On system failure:

door remains locked
Memory

Avoid dynamic allocation.

Prefer:

static allocation
stack buffers

Avoid:

String
large heap allocations

Use:

char buffers
fixed arrays
Hardware Overview

MCU
ESP32-WROOM-32U

Inputs

2× PN532 RFID (SPI)
LD2410 presence radar (UART)
LDRs (Analog)
DS3231 RTC (I2C)
Reed switch
Exit push button

Outputs

Maglock via MOSFET
Lighting via SSR
IR LED via transistor
Buzzers
RGB LEDs

Storage

MicroSD (SPI)
Repository Structure
src/

config/
    pins.h
    system_config.h

hal/
    gpio_hal.*
    spi_bus.*
    i2c_bus.*
    uart_bus.*

drivers/
    rfid/pn532.*
    sensors/ld2410.*
    sensors/LDR.*
    storage/sdcard.*
    storage/rtc_ds3231.*
    actuators/door_lock.*
    actuators/lighting.*
    actuators/ir_projector.*

services/
    auth/access_control.*
    attendance/attendance_manager.*
    automation/occupancy_logic.*
    automation/lighting_logic.*
    automation/projector_logic.*
    logging/log_manager.*

communication/
    wifi_manager.*
    mqtt_client.*

system/
    scheduler.*
    state_machine.*

main.cpp

Tests:

tests/

Contains simulated drivers and hardware mocks.

Data Flow Examples
RFID Access
PN532 Driver
   ↓
AccessControl Service
   ↓
StateMachine
   ↓
DoorLock Driver
Occupancy Automation
LD2410 Driver
   ↓
OccupancyLogic Service
   ↓
LightingLogic Service
   ↓
Lighting Driver
Attendance Logging
RFID Driver
   ↓
AttendanceManager
   ↓
LogManager
   ↓
SD Card Driver
   ↓
MQTT publish
State Machine

System states:

BOOT
IDLE
OCCUPIED
ACCESS_GRANTED
ERROR
OFFLINE

State machine controls global behavior.

Drivers must never change system state directly.

Driver Template

Example structure for a driver:

class PN532 {
public:
    bool begin();
    bool readUID(uint8_t* uid, uint8_t* length);

private:
    SPIBus& spi;
};

Drivers interact only with HAL interfaces.

Service Template

Example service structure:

class AccessControl {
public:
    bool authenticate(const uint8_t* uid, uint8_t length);

private:
    DoorLock& doorLock;
};

Services combine multiple drivers.

Important Principle

Hardware → Drivers → Services → System → Actuators

Never bypass layers.

Example of correct flow:

RFID → AccessControl → StateMachine → DoorLock

Incorrect flow:

RFID → DoorLock (forbidden)
Final Goal

The firmware must remain:

safe
modular
testable
scalable
maintainable

All code generation must respect the architecture rules above.