#if defined(ESP32) || defined(ESP8266) // Only compile this file for ESP32 or ESP8266 platforms

#include "esp_ws2812b.h"

#include <Arduino.h>
#include <stdlib.h>
#include <string.h>

#if defined(ESP8266)
#include <eagle_soc.h>
#elif defined(ESP32)
#include "soc/gpio_struct.h"
#endif

static uint8_t _pin = 0;
static uint8_t numLeds = 0;
static uint8_t *ledBuffer = NULL;
static uint8_t brightness = 255;

// Default color order is GRB for WS2812B LEDs
static color_order currentColorOrder = GRB;

void init_ws2812b(uint8_t pin, uint8_t leds)
{
    // allocate buffer (3 bytes per LED: G,R,B order for WS2812)
    ledBuffer = (uint8_t *)malloc((size_t)leds * 3);
    if (!ledBuffer)
        return;
    memset(ledBuffer, 0, (size_t)leds * 3);

    _pin = pin;
    numLeds = leds;

    pinMode(pin, OUTPUT);
}

void deinit_ws2812b(void)
{
    if (ledBuffer)
    {
        free(ledBuffer);
        ledBuffer = NULL;
    }
}

void set_color_order(color_order order)
{
    currentColorOrder = order;
}

void set_brightness(uint8_t brightness)
{
    ::brightness = brightness;
}

void set_colors(uint8_t red, uint8_t green, uint8_t blue)
{
    for (uint8_t i = 0; i < numLeds; ++i)
    {
        uint8_t *p = &ledBuffer[i * 3];
        switch (currentColorOrder)
        {
        case RGB:
            p[0] = ((uint16_t)red * brightness) / 255;
            p[1] = ((uint16_t)green * brightness) / 255;
            p[2] = ((uint16_t)blue * brightness) / 255;
            break;
        case GRB:
            p[0] = ((uint16_t)green * brightness) / 255;
            p[1] = ((uint16_t)red * brightness) / 255;
            p[2] = ((uint16_t)blue * brightness) / 255;
            break;
        case BRG:
            p[0] = ((uint16_t)blue * brightness) / 255;
            p[1] = ((uint16_t)red * brightness) / 255;
            p[2] = ((uint16_t)green * brightness) / 255;
            break;
        case RBG:
            p[0] = ((uint16_t)red * brightness) / 255;
            p[1] = ((uint16_t)blue * brightness) / 255;
            p[2] = ((uint16_t)green * brightness) / 255;
            break;
        case GBR:
            p[0] = ((uint16_t)green * brightness) / 255;
            p[1] = ((uint16_t)blue * brightness) / 255;
            p[2] = ((uint16_t)red * brightness) / 255;
            break;
        case BGR:
            p[0] = ((uint16_t)blue * brightness) / 255;
            p[1] = ((uint16_t)green * brightness) / 255;
            p[2] = ((uint16_t)red * brightness) / 255;
            break;
        }
    }
}

void set_led_color(uint8_t led, uint8_t red, uint8_t green, uint8_t blue)
{
    if (!ledBuffer || led >= numLeds)
        return;
    uint8_t *p = &ledBuffer[led * 3];

    switch (currentColorOrder)
    {
    case RGB:
        p[0] = ((uint16_t)red * brightness) / 255;
        p[1] = ((uint16_t)green * brightness) / 255;
        p[2] = ((uint16_t)blue * brightness) / 255;
        break;
    case GRB:
        p[0] = ((uint16_t)green * brightness) / 255;
        p[1] = ((uint16_t)red * brightness) / 255;
        p[2] = ((uint16_t)blue * brightness) / 255;
        break;
    case BRG:
        p[0] = ((uint16_t)blue * brightness) / 255;
        p[1] = ((uint16_t)red * brightness) / 255;
        p[2] = ((uint16_t)green * brightness) / 255;
        break;
    case RBG:
        p[0] = ((uint16_t)red * brightness) / 255;
        p[1] = ((uint16_t)blue * brightness) / 255;
        p[2] = ((uint16_t)green * brightness) / 255;
        break;
    case GBR:
        p[0] = ((uint16_t)green * brightness) / 255;
        p[1] = ((uint16_t)blue * brightness) / 255;
        p[2] = ((uint16_t)red * brightness) / 255;
        break;
    case BGR:
        p[0] = ((uint16_t)blue * brightness) / 255;
        p[1] = ((uint16_t)green * brightness) / 255;
        p[2] = ((uint16_t)red * brightness) / 255;
        break;
    }
}

void clear_leds(void)
{
    if (!ledBuffer || numLeds == 0)
        return;
    memset(ledBuffer, 0, numLeds * 3);
}

void show_colors(void)
{
    if (!ledBuffer || numLeds == 0)
        return;

    /*
     * WS2812B uses a 1.25 us bit cell.  Direct GPIO writes keep the pulse
     * edges short, while the cycle-count waits make the timings independent
     * of whether the ESP is running at 80, 160, or 240 MHz.
     */
#if defined(ESP32)
    const uint32_t cpuCyclesPerUs = (uint32_t)ESP.getCpuFreqMHz();
    const uint8_t pin = _pin;
    const uint32_t pinMask = (pin < 32) ? (1UL << pin) : 0;

    auto setHigh = [pin, pinMask]()
    {
        if (pin < 32)
            GPIO.out_w1ts = pinMask;
        else
            GPIO.out1_w1ts.val = 1UL << (pin - 32);
    };
    auto setLow = [pin, pinMask]()
    {
        if (pin < 32)
            GPIO.out_w1tc = pinMask;
        else
            GPIO.out1_w1tc.val = 1UL << (pin - 32);
    };
#elif defined(ESP8266)
    const uint32_t cpuCyclesPerUs = (uint32_t)(F_CPU / 1000000UL);
    const uint32_t pinMask = 1UL << _pin;
#else
#error "This file requires ESP8266 or ESP32"
#endif

    const uint32_t bitCycles = cpuCyclesPerUs * 5U / 4U;
    const uint32_t zeroHighCycles = cpuCyclesPerUs * 35U / 100U;
    const uint32_t oneHighCycles = cpuCyclesPerUs * 70U / 100U;

    noInterrupts();

    for (size_t i = 0; i < (size_t)numLeds * 3U; ++i)
    {
        uint8_t value = ledBuffer[i];
        for (uint8_t mask = 0x80; mask != 0; mask >>= 1)
        {
            const uint32_t start = ESP.getCycleCount();
            const uint32_t highCycles = (value & mask) ? oneHighCycles : zeroHighCycles;

#if defined(ESP32)
            setHigh();
#else
            GPOS = pinMask;
#endif
            while ((uint32_t)(ESP.getCycleCount() - start) < highCycles)
                ;

#if defined(ESP32)
            setLow();
#else
            GPOC = pinMask;
#endif
            while ((uint32_t)(ESP.getCycleCount() - start) < bitCycles)
                ;
        }
    }

#if defined(ESP32)
    setLow();
#else
    GPOC = pinMask;
#endif
    interrupts();

    // WS2812B latches the frame after a low level of at least 50 us.
    delayMicroseconds(80);
}

#endif // end of esp family check
