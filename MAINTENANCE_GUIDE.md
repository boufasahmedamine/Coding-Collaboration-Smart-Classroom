# Maintenance & Diagnostic Guide

This guide describes the built-in diagnostic tools and maintenance commands available for the **ZARZARA Smart Classroom Node**. 

## 1. Hardware Maintenance Interface (via Serial)
The following commands can be sent via the Serial Monitor (115200 baud) for on-site hardware verification and recovery:

| Key | Action | Purpose |
|-----|--------|---------|
| **`r`** | System Reboot | Perform a cold restart of the ESP32 to clear stuck states or reset networking. |
| **`l`** | Manual Door Pulse | Momentarily triggers the door lock relay. Used to verify physical lock wiring. |
| **`s`** | Force State Sync | Immediately publishes the current classroom state (occupied, lights, etc.) to the dashboard via MQTT. |
| **`v`** | Toggle Dashboard | Toggles the visibility of the ANSI Diagnostics Dashboard in the serial console. |

## 2. Diagnostics Dashboard
The node features a live dashboard that prints to the serial console every second. 
- **[CORE]**: Shows the current State Machine state (e.g., IDLE, OCCUPIED).
- **[RFID]**: Shows the status of the Inside and Outside readers.
- **[DOOR]**: Shows current maglock relay state.
- **[LITE]**: Shows real-time LDR analog values and Relay status.
- **[EVENT]**: Shows the most recent system action (e.g., "Outside scan detected").

## 3. Separation of Concerns
This firmware follows a strict **Production-First** philosophy:
- **Deployment Code**: All code in `src/` (excluding `tests/`) is the exact version running in production. No simulation hooks or "faked" logic exist in these files.
- **Isolated Testing**: Simulation and mocking logic (e.g., faking card reads) is strictly isolated in the `src/tests/` directory. This allows for rigorous logic verification in development without polluting the production executable.

---
*For remote monitoring, refer to the MQTT Documentation in PROJECT_CONTEXT.md.*
