# ZARZARA Smart Classroom: Comprehensive Architectural & Conceptual Deep-Dive

This document provides a detailed technical and conceptual analysis of the Smart Classroom Node firmware. It is intended to serve as the primary source of context for the "System Architecture," "Design Decisions," and "Implementation" chapters of the professional engineering mémoire.

---

## 1. System Vision: The "Autonomous Classroom"
The ZARZARA project moves beyond simple access control. It envisions the classroom as an active participant in the educational environment. The firmware is designed to transition the room between three primary lifecycle phases:
1.  **Secured Dormancy:** The room is locked, lights are off, and resources are conserved.
2.  **Active Session:** Triggered by authorized entry. The room "wakes up," authorizing equipment (Projector) and tracking attendance.
3.  **Presence-Aware Grace Period:** The room remains active as long as human activity is detected, preventing abrupt shutdowns during lectures or exams.

---

## 2. Strict Layered Architecture: Implementation Details

The project enforces a **Strict Layered Architecture**. Unlike traditional Arduino sketches, this code prohibits "Short-Circuiting" (e.g., a Service calling a GPIO pin directly).

### A. HAL Layer (Hardware Abstraction)
- **Role:** Direct interaction with ESP32 peripherals (SPI, I2C, UART, GPIO).
- **Files:** `src/hal/gpio_hal.cpp`, `src/hal/spi_bus.cpp`.
- **Engineering Intent:** By wrapping the ESP-IDF/Arduino functions, we create a "Swappable" layer. If the project migrated to a different MCU (e.g., STM32), only this layer would need modification.

### B. Driver Layer (Device Logic)
- **Role:** Encapsulates the protocol of specific ICs and sensors.
- **Key Classes:** 
    - `RDM6300Driver`: Handles the 125kHz RFID protocol over UART.
    - `DoorLockDriver`: Manages the MOSFET pulse-logic for maglocks.
    - `MainsPIRInput`: Debounces and filters signals from the commercial occupancy sensor.
- **Pattern:** All RFID readers implement the `IRFIDReader` interface, allowing the application to use `PN532` or `RDM6300` interchangeably without changing the business logic.

### C. Service Layer (Domain Logic)
- **Role:** Executes the "Business Rules."
- **Key Classes:**
    - `AuthProxy`: Manages the "Correlation" of MQTT requests. It remembers which UID was scanned and waits for a specific server response.
    - `PresenceService`: A logical filter. It converts raw PIR pulses into a stable "Occupied" boolean.
    - `LightingLogic`: Decides if lights should be SSR-triggered based on both `PresenceService` and the `StateMachine` state.

### D. System Layer (The "Brain")
- **Role:** High-level orchestration and state management.
- **Key Classes:**
    - `StateMachine`: The ultimate authority. It transitions between `LOCKED`, `WAITING_FOR_AUTH`, and `UNLOCKED`.
    - `AppManager`: The "Grand Orchestrator." It owns the FreeRTOS tasks and injects dependencies between all other services.

---

## 3. Distributed System Communication (MQTT & JSON)

The node acts as an **Edge Device** in a distributed system. It communicates with a central server via MQTT using structured JSON.

### A. Topic Hierarchy
- `smartclass/node/[ID]/access/request`: Node sends UID for validation.
- `smartclass/node/[ID]/access/response`: Server returns "granted" or "denied."
- `smartclass/node/[ID]/commands`: Remote overrides for lights, projector, or door.
- `smartclass/node/[ID]/status/heartbeat`: Periodic telemetry (Uptime, RSSI, Memory).

### B. Command Evolution & Security
The `CommandHandler` class reveals an ongoing transition from **Legacy String Commands** (e.g., "unlock") to **Structured JSON Commands**. 
- **Legacy:** Simple, but prone to replay attacks and lacks metadata.
- **Modern:** JSON-based, requiring semantic validation (e.g., checking if a value is a boolean before applying it). This is a critical point for the "Security" section of the thesis.

---

## 4. Multiprocessing & Real-Time Performance

The system leverages the ESP32 Dual-Core architecture via FreeRTOS, ensuring that high-priority hardware events are never delayed by network latency.

| Task Name | Core | Priority | Responsibility |
| :--- | :--- | :--- | :--- |
| `NetworkTask` | 0 | 2 | WiFi/MQTT maintenance. Keeps the connection alive. |
| `RFIDTask` | 1 | 3 | **Highest Priority.** Continuous polling of the UART readers to ensure zero missed scans. |
| `CoreLogic` | 1 | 2 | Automation (PIR processing, Lighting logic). |
| `StatusTask` | 1 | 1 | Background telemetry and Dashboard updates. |

**Synchronization:** The system uses `SemaphoreHandle_t` (Mutexes) to protect shared resources like the `StateMachine` and `MQTTManager`. This prevents "Race Conditions" where a remote command and a local RFID scan might try to change the door state simultaneously.

---

## 5. The Lifecycle of an Access Event (Packet Trace)

1.  **Detection:** `RFIDTask` (Core 1) sees a UID on `rdmOutsideSerial`.
2.  **Service Processing:** `AccessService` identifies it as an "Outside Scan."
3.  **Authentication Request:** `AuthProxy` generates a JSON packet and puts it on the `MQTTManager` queue.
4.  **Network Transmission:** `NetworkTask` (Core 0) picks up the packet and sends it to the Broker.
5.  **State Transition:** `StateMachine` moves to `WAITING_FOR_AUTH` and starts a local timeout (safety).
6.  **Response:** Server sends `ACCESS_GRANTED`. `CommandHandler` parses it and signals the `StateMachine`.
7.  **Actuation:** `StateMachine` triggers `DoorLockDriver`, which pulses the GPIO for the Maglock.

---

## 6. Resilience & Defensive Engineering

- **Driver Heartbeats:** The `AccessService` periodically checks `isAlive()` on the RFID drivers. If an SPI/UART hang occurs (common in noisy classroom environments), it executes `resetCommunication()`.
- **Power-On Safety:** In `main.cpp`, all actuators are explicitly forced to their "Safe" (OFF/LOCKED) states before any logic starts.
- **Auth Timeout:** If the server or network fails during an auth request, the `StateMachine` automatically reverts to `LOCKED` after a configurable timeout (preventing the door from getting stuck in a "Waiting" state).

---

## 7. Critical Analysis for the Mémoire

### Strengths to Emphasize:
1.  **Asynchronous Mastery:** Using FreeRTOS ensures that a slow WiFi connection doesn't make the door feel "laggy" to a user standing in front of it.
2.  **Interface-Driven Design:** The use of `IRFIDReader` is a textbook example of the **Dependency Inversion Principle**, making the code future-proof.
3.  **State-Driven Reliability:** Every system action is a consequence of a state transition, making the behavior deterministic and easy to debug.

### Identified Challenges & Engineering Compromises:
1.  **Memory Management:** The project avoids `String` but uses `ArduinoJson`, which requires careful buffer sizing (`JsonDocument`).
2.  **Hardware Discrepancy:** The docs mention `PN532`, but the code uses `RDM6300`. This suggests an engineering pivot, likely due to the robustness of UART vs SPI over long cable runs in a classroom.
3.  **Local Auth Gap:** While "Admin" UIDs are hardcoded for local bypass, there is currently no mechanism for a "Local Student Cache" if the server is offline. This is a perfect candidate for the "Future Work" chapter.

---

## 8. Suggested Clarification Questions for the Final Defense

1.  **How was the "Session Timeout" (1.5 hours) determined?** Is it based on standard university lecture blocks?
2.  **What is the "Mains PIR" integration strategy?** Is it using a 220V optoisolator or a low-voltage relay?
3.  **The `Diagnostics` Dashboard:** How much overhead does the 1-second update cycle add to the CPU, and is it intended only for the prototype phase?

---
*Comprehensive Analysis generated by Antigravity AI (Gemini 3 Flash) for the ZARZARA Master's Project.*
