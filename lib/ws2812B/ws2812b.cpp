#include "ws2812b.h"

/*
 * ws2812b.cpp
 * Created on: June 24, 2026
 * Some of the code is based on the Adafruit NeoPixel library.
 */

WS2812B::WS2812B(uint8_t pin, uint8_t leds)
{
    _pin = pin;
    _leds = leds;
}

WS2812B::~WS2812B()
{
    deinit_ws2812b();
}

void WS2812B::begin()
{
    init_ws2812b(_pin, _leds);
}

/*!
  @brief   Set a pixel's color using a 32-bit 'packed' RGB or RGBW value.
  @param   n  Pixel index, starting from 0.
  @param   c  32-bit color value. Most significant byte is white (for RGBW
              pixels) or ignored (for RGB pixels), next is red, then green,
              and least significant byte is blue.
*/
void WS2812B::setPixelColor(uint16_t n, uint32_t c)
{
    if (n >= _leds)
        return;

    uint8_t red = (c >> 16) & 0xFF;
    uint8_t green = (c >> 8) & 0xFF;
    uint8_t blue = c & 0xFF;

    set_led_color(n, red, green, blue);
}

/*!
  @brief   Fill all or part of the NeoPixel strip with a color.
  @param   c      32-bit color value. Most significant byte is white (for
                  RGBW pixels) or ignored (for RGB pixels), next is red,
                  then green, and least significant byte is blue. If all
                  arguments are unspecified, this will be 0 (off).
  @param   first  Index of first pixel to fill, starting from 0. Must be
                  in-bounds, no clipping is performed. 0 if unspecified.
  @param   count  Number of pixels to fill, as a positive value. Passing
                  0 or leaving unspecified will fill to end of strip.
*/
void WS2812B::fill(uint32_t c, uint16_t first, uint16_t count)
{
    if (first >= _leds)
        return;

    if (count == 0 || first + count > _leds)
        count = _leds - first;

    for (uint16_t i = first; i < first + count; i++)
    {
        setPixelColor(i, c);
    }
}

void WS2812B::clear()
{
    clear_leds();
}

void WS2812B::show()
{
    show_colors();
}

void WS2812B::setBrightness(uint8_t brightness)
{
    set_brightness(brightness);
}

uint32_t WS2812B::Color(uint8_t r, uint8_t g, uint8_t b, uint8_t w)
{
    return ((uint32_t)w << 24) | ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}

uint32_t WS2812B::ColorHSV(uint16_t hue, uint8_t sat, uint8_t val)
{
    uint8_t r, g, b;

    hue = (hue * 1530L + 32768) / 65536;
    if (hue < 510)
    { // Red to Green-1
        b = 0;
        if (hue < 255)
        { //   Red to Yellow-1
            r = 255;
            g = hue; //     g = 0 to 254
        }
        else
        {                  //   Yellow to Green-1
            r = 510 - hue; //     r = 255 to 1
            g = 255;
        }
    }
    else if (hue < 1020)
    { // Green to Blue-1
        r = 0;
        if (hue < 765)
        { //   Green to Cyan-1
            g = 255;
            b = hue - 510; //     b = 0 to 254
        }
        else
        {                   //   Cyan to Blue-1
            g = 1020 - hue; //     g = 255 to 1
            b = 255;
        }
    }
    else if (hue < 1530)
    { // Blue to Red-1
        g = 0;
        if (hue < 1275)
        {                   //   Blue to Magenta-1
            r = hue - 1020; //     r = 0 to 254
            b = 255;
        }
        else
        { //   Magenta to Red-1
            r = 255;
            b = 1530 - hue; //     b = 255 to 1
        }
    }
    else
    { // Last 0.5 Red (quicker than % operator)
        r = 255;
        g = b = 0;
    }

    // Apply saturation and value to R,G,B, pack into 32-bit result:
    uint32_t v1 = 1 + val;  // 1 to 256; allows >>8 instead of /255
    uint16_t s1 = 1 + sat;  // 1 to 256; same reason
    uint8_t s2 = 255 - sat; // 255 to 0
    return ((((((r * s1) >> 8) + s2) * v1) & 0xff00) << 8) |
           (((((g * s1) >> 8) + s2) * v1) & 0xff00) |
           (((((b * s1) >> 8) + s2) * v1) >> 8);
}
