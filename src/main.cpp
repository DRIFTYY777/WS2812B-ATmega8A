
/*
 * @file main.cpp
 * @brief Arduino sketch for controlling a NeoPixel LED strip with various modes.
 * @author DRIFTYY_777
 * @date 2023-10-01
 *
 * This sketch allows you to control a NeoPixel LED strip with different lighting modes.
 * The modes can be changed using a button, and the current mode is saved in EEPROM.
 *
 * WBS2812B LED strip is used in this example.
 *
 * Modes:
 * 0: Red
 * 1: Green
 * 2: Blue
 * 3: Cyan Pulse
 * 4: Static Rainbow
 * 5: ARGB-style Moving Rainbow
 * 6: Rainbow Chase
 *
 */

#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>

/*
Works on my diy board with ATmega8
*/
#define DATA_PIN 2 // ARGB pin
#define BTN_PIN 3  // Button pin on board

// Works on schematic board with ATmega8
// #define DATA_PIN A2 // ARGB pin
// #define BTN_PIN A3  // Button pin on board

/* Data Pin and No of leds */
#define INBUILD_LED 9 // Inbuilt LED pin
#define NUM_LEDS 66   // Number of LEDs in the strip
#define MODE_ADDR 0   // EEPROM address to store mode

// prevent leds burning out because we did not use any resistors and diode
#define BRIGHTNESS 200 // Brightness level (0-255)

// global variables
int mode = 0;
const int numModes = 11; // Number of modes
bool lastButtonState = HIGH;
bool currentButtonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;
uint16_t rainbowOffset = 0;

Adafruit_NeoPixel strip(NUM_LEDS, DATA_PIN, NEO_GRB + NEO_KHZ800);

void blinkInbuiltLED()
{
  digitalWrite(INBUILD_LED, HIGH);
  delay(50);
  digitalWrite(INBUILD_LED, LOW);
}

void setup()
{
  pinMode(BTN_PIN, INPUT_PULLUP);
  pinMode(INBUILD_LED, OUTPUT);
  digitalWrite(INBUILD_LED, LOW); // Turn off inbuilt LED

  // EEPROM.begin(1); // Not needed for AVR EEPROM
  EEPROM.begin();
  blinkInbuiltLED();
  mode = EEPROM.read(MODE_ADDR);
  if (mode >= numModes)
    mode = 0; // Sanity check
  strip.begin();
  strip.setBrightness(BRIGHTNESS);
  strip.show();
}

void loop()
{
  // -------- Button Handling --------
  bool reading = digitalRead(BTN_PIN);
  if (reading != lastButtonState)
  {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay)
  {
    if (currentButtonState == HIGH && reading == LOW)
    {
      mode = (mode + 1) % numModes;
      EEPROM.write(MODE_ADDR, mode); // Save mode
      // EEPROM.commit();            // Not needed for AVR EEPROM
      blinkInbuiltLED();
    }
    currentButtonState = reading;
  }
  lastButtonState = reading;

  // -------- Mode Handling --------
  switch (mode)
  {
  case 0: // Red
    strip.fill(strip.Color(255, 0, 0));
    strip.setBrightness(BRIGHTNESS);
    strip.show();
    break;

  case 1: // Green
    strip.fill(strip.Color(0, 255, 0));
    strip.setBrightness(BRIGHTNESS);
    strip.show();
    break;

  case 2: // Blue
    strip.fill(strip.Color(0, 0, 255));
    strip.setBrightness(BRIGHTNESS);
    strip.show();
    break;

  case 3: // Cyan Pulse
  {
    static uint8_t brightness = 0;
    static int fadeDirection = 1;
    static unsigned long lastPulseUpdate = 0;
    const unsigned long pulseInterval = 5;

    if (millis() - lastPulseUpdate >= pulseInterval)
    {
      lastPulseUpdate = millis();
      brightness += fadeDirection;
      if (brightness == BRIGHTNESS || brightness == 0)
        fadeDirection *= -1;

      strip.fill(strip.Color(0, 255, 255));
      strip.setBrightness(brightness);
      strip.show();
    }
    break;
  }

  case 4: // Static Rainbow
    for (int i = 0; i < NUM_LEDS; i++)
    {
      strip.setPixelColor(i, strip.ColorHSV(i * 65536L / NUM_LEDS));
    }
    strip.setBrightness(BRIGHTNESS);
    strip.show();
    delay(50);
    break;

  case 5: // ARGB-style Moving Rainbow
    for (int i = 0; i < NUM_LEDS; i++)
    {
      uint16_t color = (i * 65536L / NUM_LEDS + rainbowOffset) % 65536;
      strip.setPixelColor(i, strip.ColorHSV(color));
    }
    rainbowOffset += 256;
    strip.setBrightness(BRIGHTNESS);
    strip.show();
    delay(50);
    break;

  case 6: // Rainbow Chase
    for (int i = 0; i < NUM_LEDS; i++)
    {
      uint16_t hue = ((i * 1000) + rainbowOffset) % 65536;
      strip.setPixelColor(i, strip.ColorHSV(hue));
    }
    rainbowOffset += 1000;
    strip.setBrightness(BRIGHTNESS);
    strip.show();
    delay(100);
    break;

  case 7: // amber
    for (int i = 0; i < NUM_LEDS; i++)
    {
      strip.setPixelColor(i, strip.Color(255, 165, 0));
    }
    strip.setBrightness(BRIGHTNESS);
    strip.show();
    break;

  case 8: // purple
    for (int i = 0; i < NUM_LEDS; i++)
    {
      strip.setPixelColor(i, strip.Color(128, 0, 128));
    }
    strip.setBrightness(BRIGHTNESS);
    strip.show();
    break;

  case 9: // white
    for (int i = 0; i < NUM_LEDS; i++)
    {
      strip.setPixelColor(i, strip.Color(255, 255, 255));
    }
    strip.setBrightness(BRIGHTNESS);
    strip.show();
    break;

  case 10: // off
    for (int i = 0; i < NUM_LEDS; i++)
    {
      strip.setPixelColor(i, strip.Color(0, 0, 0));
    }
    strip.setBrightness(0);
    strip.clear();
    strip.show();
    break;
  }
  delay(10);
}
