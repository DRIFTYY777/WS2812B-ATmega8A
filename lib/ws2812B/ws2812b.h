/*
 * ws2812b.h
 *
 *  Created on: June 24, 2026
 *  Author: DRIFTYY777
 */

#if !defined(WS2812B_H_)
#define WS2812B_H_

#include "stdint.h"
#include "stdbool.h"

#include <Arduino.h>

#if defined(__AVR__)
#include "avr/avr_ws2812b.h"
#elif defined(ESP32) || defined(ESP8266)
#include "esp/esp_ws2812b.h"
#else
#warning "WS2812B library is not implemented for this MCU."
#endif

class WS2812B
{
public:
    WS2812B(uint8_t pin, uint8_t leds);
    ~WS2812B();

    void begin();
    void setPixelColor(uint16_t n, uint32_t c);
    void fill(uint32_t c = 0, uint16_t first = 0, uint16_t count = 0);
    void clear();
    void show();
    void setBrightness(uint8_t brightness);
    static uint32_t Color(uint8_t r, uint8_t g, uint8_t b, uint8_t w = 0);
    static uint32_t ColorHSV(uint16_t hue, uint8_t sat = 255, uint8_t val = 255);

private:
    uint8_t _pin, _leds;
};

#endif // WS2812B_H_