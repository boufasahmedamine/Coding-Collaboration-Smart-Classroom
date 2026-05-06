#ifndef PINS_H
#define PINS_H

// --- UART SENSORS (RDM6300) ---
#define PIN_RDM6300_OUT_RX 16  // UART1 RX for Outside RFID
#define PIN_RDM6300_IN_RX  17  // UART2 RX for Inside RFID

// --- DIGITAL SENSORS ---
#define PIN_MAINS_PIR_INPUT 4  // Opto-isolated 220V PIR (Active LOW)

// --- ANALOG SENSORS ---
// Photoresistor
#define PIN_LDR        34

// --- RELAYS & ACTUATORS ---
#define PIN_DOOR_LOCK  25
#define DOOR_ACTIVE_HIGH true

#define PIN_LIGHTING   26
#define PIN_PROJECTOR  27

#endif