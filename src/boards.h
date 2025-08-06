#if !defined(BOARDS_H)
#define BOARDS_H

// For debug purposes
// #define DATA_PIN 2
// #define BTN_PIN 3

// MCU Detection and Configuration
#if defined(__AVR_ATmega8__) // ATmega8 and ATmega8A is the same
#define MCU_NAME "ATmega8"
#define MCU_FLASH_SIZE 8192
#define MCU_EEPROM_SIZE 512
#define DATA_PIN A2       // ARGB pin  (PA2)
#define BTN_PIN A3        // Button pin on board (PA3)
#define INBUILD_LED 9     // Inbuilt LED pin (PB1)
#define IR_RECEIVE_PIN 10 // IR receive pin (PB2)

#elif defined(__AVR_ATmega48__) // ATmega8A
#define MCU_NAME "ATmega48"
#define MCU_FLASH_SIZE 4096
#define MCU_EEPROM_SIZE 256
#define DATA_PIN 2     // ARGB pin  (PB2)
#define BTN_PIN 3      // Button pin on board (PB3)
#define INBUILD_LED 13 // Inbuilt LED pin (PB5)

#elif defined(__AVR_ATmega88__)
#define MCU_NAME "ATmega88"
#define MCU_FLASH_SIZE 8192
#define MCU_EEPROM_SIZE 512
#define DATA_PIN 2        // ARGB pin (PB2)
#define BTN_PIN 3         // Button pin on board (PB3)
#define INBUILD_LED 13    // Inbuilt LED pin (PB5)

#elif defined(__AVR_ATmega168__) // ATmega168
#define MCU_NAME "ATmega168"
#define MCU_FLASH_SIZE 16384
#define MCU_EEPROM_SIZE 512
#define DATA_PIN 2        // ARGB pin (PB2)
#define BTN_PIN 3         // Button pin on board (PB3)
#define INBUILD_LED 13    // Inbuilt LED pin (PB5)
#define IR_RECEIVE_PIN 10 // IR receive pin (PB2)

#elif defined(__AVR_ATmega328P__) // ATmega328P (Arduino Uno, Nano)
#define MCU_NAME "ATmega328P"
#define MCU_FLASH_SIZE 32768
#define MCU_EEPROM_SIZE 1024
#define DATA_PIN 2              // ARGB pin (PB2)
#define BTN_PIN 3               // Button pin on board (PB3)
#define INBUILD_LED LED_BUILTIN // Inbuilt LED pin (PB5)
#define IR_RECEIVE_PIN 10       // IR receive pin (PB2)

// attiny85
#elif defined(__AVR_ATtiny85__) // ATtiny85
#define MCU_NAME "ATtiny85"
#define MCU_FLASH_SIZE 8192
#define MCU_EEPROM_SIZE 512
#define DATA_PIN 0       // ARGB pin  (PB0)
#define BTN_PIN 1        // Button pin on board (PB1)
#define INBUILD_LED 2    // Inbuilt LED pin (PB2)
#define IR_RECEIVE_PIN 5 // IR receive pin (PB5)

// ESP8266
#elif defined(ESP8266)   // ESP8266
#define MCU_NAME "ESP8266"
#define MCU_FLASH_SIZE 1048576  // 1MB
#define MCU_EEPROM_SIZE 4096    // 4KB
#define DATA_PIN 4              // ARGB pin (GPIO4)
#define BTN_PIN 0               // Button pin on board (GPIO0)
#define INBUILD_LED LED_BUILTIN // Inbuilt LED pin (GPIO2)
#define IR_RECEIVE_PIN 5        // IR receive pin (GPIO5) // Note: using Hardware I2C pin

// ESP32
#elif defined(ESP32)            // ESP32
#define MCU_NAME "ESP32"
#define MCU_FLASH_SIZE 4194304 // 4MB
#define MCU_EEPROM_SIZE 4096   // 4KB
#define DATA_PIN 4             // ARGB pin (GPIO4)
#define BTN_PIN 0              // Button pin on board (GPIO0)
#define INBUILD_LED 2          // Inbuilt LED pin (GPIO2)
#define IR_RECEIVE_PIN 5       // IR receive pin (GPIO5)  Note: using Hardware SPI pin

#else
#error "Unsupported MCU. Please define the correct MCU."
#endif

#endif // BOARDS_H
