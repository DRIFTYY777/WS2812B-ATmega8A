/*
 * ws2812b.h
 *
 *  Created on: June 24, 2026
 *  Author: DRIFTYY777
 */

#ifndef AVR_WS2812B_H
#define AVR_WS2812B_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif
    /// @brief Enum for color order of WS2812B LEDs
    typedef enum
    {
        RGB,
        GRB,
        BRG,
        RBG,
        GBR,
        BGR
    } color_order;

    /// @brief Initializes the WS2812B library with the specified pin and number of LEDs.
    /// @param pin
    /// @param leds
    void init_ws2812b(uint8_t pin, uint8_t leds);

    /// @brief Deinitializes the WS2812B library, freeing any allocated resources.
    /// @param  NONE
    void deinit_ws2812b(void);

    /// @brief Sets the color order for the WS2812B LEDs.
    /// @param RGB, GRB, BRG, RBG, GBR, BGR
    void set_color_order(color_order order);

    /// @brief  Sets the brightness for the WS2812B LEDs.
    /// @param brightness 0-255 (0 = off, 255 = full brightness)
    void set_brightness(uint8_t brightness);

    /// @brief Sets the color for all WS2812B LEDs.
    /// @param red 0-255
    /// @param green 0-255
    /// @param blue 0-255
    void set_colors(uint8_t red, uint8_t green, uint8_t blue);

    /// @brief Sets the color for a specific WS2812B LED.
    /// @param led Numeric index of the LED to set (0-based)
    /// @param red 0-255
    /// @param green 0-255
    /// @param blue 0-255
    void set_led_color(uint8_t led, uint8_t red, uint8_t green, uint8_t blue);

    /// @brief  Clears the colors of all WS2812B LEDs (turns them off).
    /// @param
    void clear_leds(void);

    /// @brief Updates the display with the current LED colors.
    void show_colors(void);

#ifdef __cplusplus
}
#endif

#endif // AVR_WS2812B_H