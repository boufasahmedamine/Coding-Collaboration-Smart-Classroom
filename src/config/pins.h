#ifndef PINS_H
#define PINS_H

// --- SPI BUS ---
// Standard VSPI pins for ESP32
#define PIN_SPI_SCK    18
#define PIN_SPI_MISO   19
#define PIN_SPI_MOSI   23

// --- CHIP SELECTS ---
// Must be unique to prevent bus collisions! 
#define PIN_PN532_CS   22   // Dedicated CS for RFID
// PIN_SD_CS (5) is reclaimed

// --- DIGITAL SENSORS ---
#define PIN_PIR        16   // Reclaimed from Radar RX
// PIN_RADAR_TX (17) is reclaimed

// --- ANALOG SENSORS ---
// Photoresistor
#define PIN_LDR        34

// --- RELAYS & ACTUATORS ---
#define PIN_DOOR_LOCK  25
#define DOOR_ACTIVE_HIGH true

#define PIN_LIGHTING   26
#define PIN_PROJECTOR  27

#endif