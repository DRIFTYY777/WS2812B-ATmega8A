/// @file avr_ws2812b.c
/// @brief Implementation of WS2812B LED control for AVR microcontrollers.
/// @note Tested on ATmega8, Atmega328p only Currently with clock speed of 8MHz, 12MHz, and 16MHz.
///

#include "avr_ws2812b.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <string.h>

#ifndef F_CPU
#error "F_CPU must be defined"
#endif

// Map Arduino UNO-style pin numbers to AVR ports/bit positions
#if defined(__AVR_ATtiny85__)

#define PIN_TO_PIN_REG(pin) ((pin) <= 5 ? &PINB : NULL)
#define PIN_TO_BIT(pin) ((pin) <= 5 ? (pin) : 0)

#define PIN_TO_PORT_REG(pin) ((pin) <= 5 ? &PORTB : NULL)
#define PIN_TO_DDR_REG(pin) ((pin) <= 5 ? &DDRB : NULL)

#else
#define PIN_TO_PIN_REG(pin) ((pin) <= 7 ? &PIND : ((pin) <= 13 ? &PINB : &PINC))
#define PIN_TO_BIT(pin) ((pin) <= 7 ? (pin) : ((pin) <= 13 ? ((pin) - 8) : ((pin) - 14)))

#define PIN_TO_PORT_REG(pin) ((pin) <= 7 ? &PORTD : ((pin) <= 13 ? &PORTB : &PORTC))
#define PIN_TO_DDR_REG(pin) ((pin) <= 7 ? &DDRD : ((pin) <= 13 ? &DDRB : &DDRC))

#endif

static volatile uint8_t *portReg = 0;
static volatile uint8_t *ddrReg = 0;
static uint8_t pinMask = 0;
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

    volatile uint8_t *p = PIN_TO_PORT_REG(pin);
    volatile uint8_t *d = PIN_TO_DDR_REG(pin);

    portReg = p;
    ddrReg = d;
    pinMask = (1 << PIN_TO_BIT(pin));
    numLeds = leds;

    // configure pin as output and drive low
    *ddrReg |= pinMask;
    *portReg &= ~pinMask;
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

void set_brightness(uint8_t b)
{
    brightness = b;
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
    // // WS2812 expects GRB order
    // p[0] = ((uint16_t)green * brightness) / 255;
    // p[1] = ((uint16_t)red * brightness) / 255;
    // p[2] = ((uint16_t)blue * brightness) / 255;

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

// check it is ATtiny85 or ATmega8/48/88/168/328P
#if defined(__AVR_ATtiny85__)
void show_colors(void)
{
    if (!ledBuffer || numLeds == 0)
        return;

    // Enforce that only PORTB can be targeted on this hardware configuration
    if (portReg != &PORTB)
    {
        // Safety fallback: prevent the code from running out into undefined memory
        return;
    }

    // Local variables for the assembly inputs
    uint8_t *data = ledBuffer;
    uint16_t count = (uint16_t)numLeds * 3;
    uint8_t mask = pinMask;
    uint8_t mask_inv = ~pinMask;

    // Disable interrupts
    uint8_t sreg = SREG;
    cli();

#if F_CPU == 8000000UL
    __asm__ __volatile__(
        "push r18\n\t"
        "push r19\n\t"
        "push r20\n\t"
        "push r21\n\t"
        "push r22\n\t"
        "push r23\n\t"
        "mov r30, %A0\n\t"
        "mov r31, %B0\n\t"
        "mov r22, %A1\n\t"
        "mov r23, %B1\n\t"
        "send8_loopB:\n\t"
        "ld r18, Z+\n\t"
        "ldi r20, 8\n\t"
        "bit8_loopB:\n\t"
        "in r19, %[port_io]\n\t"
        "or r19, %[mask]\n\t"
        "out %[port_io], r19\n\t" // High (T=0)
        "rol r18\n\t"             // Shift bit into Carry
        "brcs one8B\n\t"          // 1 cycle if 0-bit, 2 cycles if 1-bit
        "and r19, %[mask_inv]\n\t"
        "out %[port_io], r19\n\t" // Clear pin immediately for 0-bit
        "rjmp cont8B\n\t"
        "one8B:\n\t"
        "nop\n\t"
        "nop\n\t" // Hold pulse high
        "and r19, %[mask_inv]\n\t"
        "out %[port_io], r19\n\t" // Clear pin for 1-bit
        "cont8B:\n\t"
        "dec r20\n\t"
        "brne bit8_loopB\n\t"
        "subi r22, 1\n\t"
        "sbci r23, 0\n\t"
        "brne send8_loopB\n\t"
        "pop r23\n\t"
        "pop r22\n\t"
        "pop r21\n\t"
        "pop r20\n\t"
        "pop r19\n\t"
        "pop r18\n\t"
        : : "r"(data), "r"(count), [port_io] "I"(_SFR_IO_ADDR(PORTB)), [mask] "r"(mask), [mask_inv] "r"(mask_inv)
        : "r18", "r19", "r20", "r21", "r22", "r23", "r30", "r31", "memory");

#elif F_CPU == 12000000UL
    __asm__ __volatile__(
        "push r18\n\t"
        "push r19\n\t"
        "push r20\n\t"
        "push r21\n\t"
        "push r22\n\t"
        "push r23\n\t"
        "mov r30, %A0\n\t"
        "mov r31, %B0\n\t"
        "mov r22, %A1\n\t"
        "mov r23, %B1\n\t"
        "send12_loopB:\n\t"
        "ld r18, Z+\n\t"
        "ldi r20, 8\n\t"
        "bit12_loopB:\n\t"
        "in r19, %[port_io]\n\t"
        "or r19, %[mask]\n\t"
        "out %[port_io], r19\n\t" // High at T=0
        "rol r18\n\t"
        "brcs one12B\n\t"
        "in r19, %[port_io]\n\t"
        "and r19, %[mask_inv]\n\t"
        "out %[port_io], r19\n\t" // Low for '0' (T=4)
        "rjmp cont12B\n\t"
        "one12B:\n\t"
        "nop\n\t"
        "nop\n\t"
        "nop\n\t"
        "in r19, %[port_io]\n\t"
        "and r19, %[mask_inv]\n\t"
        "out %[port_io], r19\n\t" // Low for '1' (T=8)
        "cont12B:\n\t"
        "dec r20\n\t"
        "brne bit12_loopB\n\t"
        "subi r22, 1\n\t"
        "sbci r23, 0\n\t"
        "brne send12_loopB\n\t"
        "pop r23\n\t"
        "pop r22\n\t"
        "pop r21\n\t"
        "pop r20\n\t"
        "pop r19\n\t"
        "pop r18\n\t"
        : : "r"(data), "r"(count), [port_io] "I"(_SFR_IO_ADDR(PORTB)), [mask] "r"(mask), [mask_inv] "r"(mask_inv)
        : "r18", "r19", "r20", "r21", "r22", "r23", "r30", "r31", "memory");

#elif F_CPU == 16000000UL
    __asm__ __volatile__(
        "push r18\n\t"
        "push r19\n\t"
        "push r20\n\t"
        "push r21\n\t"
        "push r22\n\t"
        "push r23\n\t"
        "mov r30, %A0\n\t"
        "mov r31, %B0\n\t"
        "mov r22, %A1\n\t"
        "mov r23, %B1\n\t"
        "send_bytes_16B:\n\t"
        "ld r18, Z+\n\t"
        "ldi r20, 8\n\t"
        "bitloop_16bB:\n\t"
        "in r19, %[port_io]\n\t"
        "or r19, %[mask]\n\t"
        "out %[port_io], r19\n\t"
        "rol r18\n\t"
        "brcs one16bB\n\t"
        "nop\n\t"
        "nop\n\t"
        "in r19, %[port_io]\n\t"
        "and r19, %[mask_inv]\n\t"
        "out %[port_io], r19\n\t"
        "nop\n\t"
        "nop\n\t"
        "rjmp cont16bB\n\t"
        "one16bB:\n\t"
        "nop\n\t"
        "nop\n\t"
        "nop\n\t"
        "in r19, %[port_io]\n\t"
        "and r19, %[mask_inv]\n\t"
        "out %[port_io], r19\n\t"
        "nop\n\t"
        "nop\n\t"
        "cont16bB:\n\t"
        "dec r20\n\t"
        "brne bitloop_16bB\n\t"
        "subi r22, 1\n\t"
        "sbci r23, 0\n\t"
        "brne send_bytes_16B\n\t"
        "pop r23\n\t"
        "pop r22\n\t"
        "pop r21\n\t"
        "pop r20\n\t"
        "pop r19\n\t"
        "pop r18\n\t"
        : : "r"(data), "r"(count), [port_io] "I"(_SFR_IO_ADDR(PORTB)), [mask] "r"(mask), [mask_inv] "r"(mask_inv)
        : "r18", "r19", "r20", "r21", "r22", "r23", "r30", "r31", "memory");

#else
#error "Unsupported F_CPU clock speed for WS2812B library!"
#endif

    SREG = sreg;

    // Standard reset delay verification (>50us)
    for (volatile uint16_t i = 0; i < (F_CPU / 1000000UL) * 60; ++i)
    {
        __asm__ __volatile__("nop");
    }
}

#else

void show_colors(void)
{
    if (!ledBuffer || numLeds == 0)
        return;

    // Local variables for the assembly inputs
    uint8_t *data = ledBuffer;
    uint16_t count = (uint16_t)numLeds * 3;
    uint8_t mask = pinMask;
    uint8_t mask_inv = ~pinMask;

    // Disable interrupts
    uint8_t sreg = SREG;
    cli();

#if F_CPU == 8000000UL

    if (portReg == &PORTD)
    {
        __asm__ __volatile__(
            "push r18\n\t"
            "push r19\n\t"
            "push r20\n\t"
            "push r21\n\t"
            "push r22\n\t"
            "push r23\n\t"
            "mov r30, %A0\n\t"
            "mov r31, %B0\n\t"
            "mov r22, %A1\n\t"
            "mov r23, %B1\n\t"
            "send8_loopD:\n\t"
            "ld r18, Z+\n\t"
            "ldi r20, 8\n\t"
            "bit8_loopD:\n\t"
            "in r19, %[port_io]\n\t"
            "or r19, %[mask]\n\t"
            "out %[port_io], r19\n\t" // High (T=0)
            "rol r18\n\t"             // Shift bit into Carry (1 cycle)
            "brcs one8D\n\t"          // 1 cycle if 0-bit, 2 cycles if 1-bit
            "and r19, %[mask_inv]\n\t"
            "out %[port_io], r19\n\t" // Clear pin IMMEDIATELY for 0-bit (T=5 cycles High -> ~375ns)
            "rjmp cont8D\n\t"
            "one8D:\n\t"
            "nop\n\t"
            "nop\n\t" // Stretch the High phase for a 1-bit
            "and r19, %[mask_inv]\n\t"
            "out %[port_io], r19\n\t" // Clear pin for 1-bit (T=7 cycles High -> ~750ns)
            "cont8D:\n\t"
            "dec r20\n\t"
            "brne bit8_loopD\n\t"
            "subi r22, 1\n\t"
            "sbci r23, 0\n\t"
            "brne send8_loopD\n\t"
            "pop r23\n\t"
            "pop r22\n\t"
            "pop r21\n\t"
            "pop r20\n\t"
            "pop r19\n\t"
            "pop r18\n\t"
            : : "r"(data), "r"(count), [port_io] "I"(_SFR_IO_ADDR(PORTD)), [mask] "r"(mask), [mask_inv] "r"(mask_inv)
            : "r18", "r19", "r20", "r21", "r22", "r23", "r30", "r31", "memory");
    }
    else if (portReg == &PORTB)
    {
        __asm__ __volatile__(
            "push r18\n\t"
            "push r19\n\t"
            "push r20\n\t"
            "push r21\n\t"
            "push r22\n\t"
            "push r23\n\t"
            "mov r30, %A0\n\t"
            "mov r31, %B0\n\t"
            "mov r22, %A1\n\t"
            "mov r23, %B1\n\t"
            "send8_loopB:\n\t"
            "ld r18, Z+\n\t"
            "ldi r20, 8\n\t"
            "bit8_loopB:\n\t"
            "in r19, %[port_io]\n\t"
            "or r19, %[mask]\n\t"
            "out %[port_io], r19\n\t"
            "rol r18\n\t"
            "brcs one8B\n\t"
            "and r19, %[mask_inv]\n\t"
            "out %[port_io], r19\n\t"
            "rjmp cont8B\n\t"
            "one8B:\n\t"
            "nop\n\t"
            "nop\n\t"
            "and r19, %[mask_inv]\n\t"
            "out %[port_io], r19\n\t"
            "cont8B:\n\t"
            "dec r20\n\t"
            "brne bit8_loopB\n\t"
            "subi r22, 1\n\t"
            "sbci r23, 0\n\t"
            "brne send8_loopB\n\t"
            "pop r23\n\t"
            "pop r22\n\t"
            "pop r21\n\t"
            "pop r20\n\t"
            "pop r19\n\t"
            "pop r18\n\t"
            : : "r"(data), "r"(count), [port_io] "I"(_SFR_IO_ADDR(PORTB)), [mask] "r"(mask), [mask_inv] "r"(mask_inv)
            : "r18", "r19", "r20", "r21", "r22", "r23", "r30", "r31", "memory");
    }
    else
    {
        __asm__ __volatile__(
            "push r18\n\t"
            "push r19\n\t"
            "push r20\n\t"
            "push r21\n\t"
            "push r22\n\t"
            "push r23\n\t"
            "mov r30, %A0\n\t"
            "mov r31, %B0\n\t"
            "mov r22, %A1\n\t"
            "mov r23, %B1\n\t"
            "send8_loopC:\n\t"
            "ld r18, Z+\n\t"
            "ldi r20, 8\n\t"
            "bit8_loopC:\n\t"
            "in r19, %[port_io]\n\t"
            "or r19, %[mask]\n\t"
            "out %[port_io], r19\n\t"
            "rol r18\n\t"
            "brcs one8C\n\t"
            "and r19, %[mask_inv]\n\t"
            "out %[port_io], r19\n\t"
            "rjmp cont8C\n\t"
            "one8C:\n\t"
            "nop\n\t"
            "nop\n\t"
            "and r19, %[mask_inv]\n\t"
            "out %[port_io], r19\n\t"
            "cont8C:\n\t"
            "dec r20\n\t"
            "brne bit8_loopC\n\t"
            "subi r22, 1\n\t"
            "sbci r23, 0\n\t"
            "brne send8_loopC\n\t"
            "pop r23\n\t"
            "pop r22\n\t"
            "pop r21\n\t"
            "pop r20\n\t"
            "pop r19\n\t"
            "pop r18\n\t"
            : : "r"(data), "r"(count), [port_io] "I"(_SFR_IO_ADDR(PORTC)), [mask] "r"(mask), [mask_inv] "r"(mask_inv)
            : "r18", "r19", "r20", "r21", "r22", "r23", "r30", "r31", "memory");
    }

#elif F_CPU == 12000000UL
    if (portReg == &PORTD)
    {
        __asm__ __volatile__(
            "push r18\n\t"
            "push r19\n\t"
            "push r20\n\t"
            "push r21\n\t"
            "push r22\n\t"
            "push r23\n\t"
            "mov r30, %A0\n\t"
            "mov r31, %B0\n\t"
            "mov r22, %A1\n\t"
            "mov r23, %B1\n\t"
            "send12_loopD:\n\t"
            "ld r18, Z+\n\t"
            "ldi r20, 8\n\t"
            "bit12_loopD:\n\t"
            "in r19, %[port_io]\n\t"
            "or r19, %[mask]\n\t"
            "out %[port_io], r19\n\t" // High at T=0
            "rol r18\n\t"
            "brcs one12D\n\t" // 1 cycle if false, 2 if true
            "in r19, %[port_io]\n\t"
            "and r19, %[mask_inv]\n\t"
            "out %[port_io], r19\n\t" // Low for '0' (T=4)
            "rjmp cont12D\n\t"        // Go to loop tracking
            "one12D:\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "in r19, %[port_io]\n\t"
            "and r19, %[mask_inv]\n\t"
            "out %[port_io], r19\n\t" // Low for '1' (T=8)
            "cont12D:\n\t"
            "dec r20\n\t"
            "brne bit12_loopD\n\t"
            "subi r22, 1\n\t"
            "sbci r23, 0\n\t"
            "brne send12_loopD\n\t"
            "pop r23\n\t"
            "pop r22\n\t"
            "pop r21\n\t"
            "pop r20\n\t"
            "pop r19\n\t"
            "pop r18\n\t"
            : : "r"(data), "r"(count), [port_io] "I"(_SFR_IO_ADDR(PORTD)), [mask] "r"(mask), [mask_inv] "r"(mask_inv)
            : "r18", "r19", "r20", "r21", "r22", "r23", "r30", "r31", "memory");
    }
    else if (portReg == &PORTB)
    {
        __asm__ __volatile__(
            "push r18\n\t"
            "push r19\n\t"
            "push r20\n\t"
            "push r21\n\t"
            "push r22\n\t"
            "push r23\n\t"
            "mov r30, %A0\n\t"
            "mov r31, %B0\n\t"
            "mov r22, %A1\n\t"
            "mov r23, %B1\n\t"
            "send12_loopB:\n\t"
            "ld r18, Z+\n\t"
            "ldi r20, 8\n\t"
            "bit12_loopB:\n\t"
            "in r19, %[port_io]\n\t"
            "or r19, %[mask]\n\t"
            "out %[port_io], r19\n\t"
            "rol r18\n\t"
            "brcs one12B\n\t"
            "in r19, %[port_io]\n\t"
            "and r19, %[mask_inv]\n\t"
            "out %[port_io], r19\n\t"
            "rjmp cont12B\n\t"
            "one12B:\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "in r19, %[port_io]\n\t"
            "and r19, %[mask_inv]\n\t"
            "out %[port_io], r19\n\t"
            "cont12B:\n\t"
            "dec r20\n\t"
            "brne bit12_loopB\n\t"
            "subi r22, 1\n\t"
            "sbci r23, 0\n\t"
            "brne send12_loopB\n\t"
            "pop r23\n\t"
            "pop r22\n\t"
            "pop r21\n\t"
            "pop r20\n\t"
            "pop r19\n\t"
            "pop r18\n\t"
            : : "r"(data), "r"(count), [port_io] "I"(_SFR_IO_ADDR(PORTB)), [mask] "r"(mask), [mask_inv] "r"(mask_inv)
            : "r18", "r19", "r20", "r21", "r22", "r23", "r30", "r31", "memory");
    }
    else
    {
        __asm__ __volatile__(
            "push r18\n\t"
            "push r19\n\t"
            "push r20\n\t"
            "push r21\n\t"
            "push r22\n\t"
            "push r23\n\t"
            "mov r30, %A0\n\t"
            "mov r31, %B0\n\t"
            "mov r22, %A1\n\t"
            "mov r23, %B1\n\t"
            "send12_loopC:\n\t"
            "ld r18, Z+\n\t"
            "ldi r20, 8\n\t"
            "bit12_loopC:\n\t"
            "in r19, %[port_io]\n\t"
            "or r19, %[mask]\n\t"
            "out %[port_io], r19\n\t"
            "rol r18\n\t"
            "brcs one12C\n\t"
            "in r19, %[port_io]\n\t"
            "and r19, %[mask_inv]\n\t"
            "out %[port_io], r19\n\t"
            "rjmp cont12C\n\t"
            "one12C:\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "in r19, %[port_io]\n\t"
            "and r19, %[mask_inv]\n\t"
            "out %[port_io], r19\n\t"
            "cont12C:\n\t"
            "dec r20\n\t"
            "brne bit12_loopC\n\t"
            "subi r22, 1\n\t"
            "sbci r23, 0\n\t"
            "brne send12_loopC\n\t"
            "pop r23\n\t"
            "pop r22\n\t"
            "pop r21\n\t"
            "pop r20\n\t"
            "pop r19\n\t"
            "pop r18\n\t"
            : : "r"(data), "r"(count), [port_io] "I"(_SFR_IO_ADDR(PORTC)), [mask] "r"(mask), [mask_inv] "r"(mask_inv)
            : "r18", "r19", "r20", "r21", "r22", "r23", "r30", "r31", "memory");
    }

#elif F_CPU == 16000000UL
    if (portReg == &PORTD)
    {
        __asm__ __volatile__(
            "push r18\n\t"
            "push r19\n\t"
            "push r20\n\t"
            "push r21\n\t"
            "push r22\n\t"
            "push r23\n\t"
            "mov r30, %A0\n\t"
            "mov r31, %B0\n\t"
            "mov r22, %A1\n\t"
            "mov r23, %B1\n\t"
            "send_bytes_16D:\n\t"
            "ld r18, Z+\n\t"
            "ldi r20, 8\n\t"
            "bitloop_16bD:\n\t"
            "in r19, %[port_io]\n\t"
            "or r19, %[mask]\n\t"
            "out %[port_io], r19\n\t"
            "rol r18\n\t"
            "brcs one16bD\n\t"
            "nop\n\t"
            "nop\n\t"
            "in r19, %[port_io]\n\t"
            "and r19, %[mask_inv]\n\t"
            "out %[port_io], r19\n\t"
            "nop\n\t"
            "nop\n\t"
            "rjmp cont16bD\n\t"
            "one16bD:\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "in r19, %[port_io]\n\t"
            "and r19, %[mask_inv]\n\t"
            "out %[port_io], r19\n\t"
            "nop\n\t"
            "nop\n\t"
            "cont16bD:\n\t"
            "dec r20\n\t"
            "brne bitloop_16bD\n\t"
            "subi r22, 1\n\t"
            "sbci r23, 0\n\t"
            "brne send_bytes_16D\n\t"
            "pop r23\n\t"
            "pop r22\n\t"
            "pop r21\n\t"
            "pop r20\n\t"
            "pop r19\n\t"
            "pop r18\n\t"
            : : "r"(data), "r"(count), [port_io] "I"(_SFR_IO_ADDR(PORTD)), [mask] "r"(mask), [mask_inv] "r"(mask_inv)
            : "r18", "r19", "r20", "r21", "r22", "r23", "r30", "r31", "memory");
    }
    else if (portReg == &PORTB)
    {
        __asm__ __volatile__(
            "push r18\n\t"
            "push r19\n\t"
            "push r20\n\t"
            "push r21\n\t"
            "push r22\n\t"
            "push r23\n\t"
            "mov r30, %A0\n\t"
            "mov r31, %B0\n\t"
            "mov r22, %A1\n\t"
            "mov r23, %B1\n\t"
            "send_bytes_16B:\n\t"
            "ld r18, Z+\n\t"
            "ldi r20, 8\n\t"
            "bitloop_16bB:\n\t"
            "in r19, %[port_io]\n\t"
            "or r19, %[mask]\n\t"
            "out %[port_io], r19\n\t"
            "rol r18\n\t"
            "brcs one16bB\n\t"
            "nop\n\t"
            "nop\n\t"
            "in r19, %[port_io]\n\t"
            "and r19, %[mask_inv]\n\t"
            "out %[port_io], r19\n\t"
            "nop\n\t"
            "nop\n\t"
            "rjmp cont16bB\n\t"
            "one16bB:\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "in r19, %[port_io]\n\t"
            "and r19, %[mask_inv]\n\t"
            "out %[port_io], r19\n\t"
            "nop\n\t"
            "nop\n\t"
            "cont16bB:\n\t"
            "dec r20\n\t"
            "brne bitloop_16bB\n\t"
            "subi r22, 1\n\t"
            "sbci r23, 0\n\t"
            "brne send_bytes_16B\n\t"
            "pop r23\n\t"
            "pop r22\n\t"
            "pop r21\n\t"
            "pop r20\n\t"
            "pop r19\n\t"
            "pop r18\n\t"
            : : "r"(data), "r"(count), [port_io] "I"(_SFR_IO_ADDR(PORTB)), [mask] "r"(mask), [mask_inv] "r"(mask_inv)
            : "r18", "r19", "r20", "r21", "r22", "r23", "r30", "r31", "memory");
    }
    else
    {
        __asm__ __volatile__(
            "push r18\n\t"
            "push r19\n\t"
            "push r20\n\t"
            "push r21\n\t"
            "push r22\n\t"
            "push r23\n\t"
            "mov r30, %A0\n\t"
            "mov r31, %B0\n\t"
            "mov r22, %A1\n\t"
            "mov r23, %B1\n\t"
            "send_bytes_16C:\n\t"
            "ld r18, Z+\n\t"
            "ldi r20, 8\n\t"
            "bitloop_16bC:\n\t"
            "in r19, %[port_io]\n\t"
            "or r19, %[mask]\n\t"
            "out %[port_io], r19\n\t"
            "rol r18\n\t"
            "brcs one16bC\n\t"
            "nop\n\t"
            "nop\n\t"
            "in r19, %[port_io]\n\t"
            "and r19, %[mask_inv]\n\t"
            "out %[port_io], r19\n\t"
            "nop\n\t"
            "nop\n\t"
            "rjmp cont16bC\n\t"
            "one16bC:\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "in r19, %[port_io]\n\t"
            "and r19, %[mask_inv]\n\t"
            "out %[port_io], r19\n\t"
            "nop\n\t"
            "nop\n\t"
            "cont16bC:\n\t"
            "dec r20\n\t"
            "brne bitloop_16bC\n\t"
            "subi r22, 1\n\t"
            "sbci r23, 0\n\t"
            "brne send_bytes_16C\n\t"
            "pop r23\n\t"
            "pop r22\n\t"
            "pop r21\n\t"
            "pop r20\n\t"
            "pop r19\n\t"
            "pop r18\n\t"
            : : "r"(data), "r"(count), [port_io] "I"(_SFR_IO_ADDR(PORTC)), [mask] "r"(mask), [mask_inv] "r"(mask_inv)
            : "r18", "r19", "r20", "r21", "r22", "r23", "r30", "r31", "memory");
    }
#endif

    SREG = sreg;

    // This loop cleanly scales for 8, 12, or 16 MHz to keep reset delay > 50us
    for (volatile uint16_t i = 0; i < (F_CPU / 1000000UL) * 60; ++i)
    {
        __asm__ __volatile__("nop");
    }
}

#endif // End of ATtiny85 check