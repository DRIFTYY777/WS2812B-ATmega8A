/**
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
 * 7: Amber Pulse
 * 8: Purple
 * 9: White
 * 10: Random Fade
 * 11: Off
 *
 *
 * Note this code also works with ATmega328p (Arduino uno) and other boards which support Arduino api.
 * 1: Make sure to install the Adafruit NeoPixel library from the Library Manager.
 * 2: Connect the NeoPixel data pin to the specified DATA_PIN.
 * 3: Connect a button to the specified BTN_PIN.
 * 4: Copy the code into your Arduino IDE.
 * 5: Upload the sketch to your Arduino board.
 *
 *
 *
 */

/*
 * Simulation Link:
 * https://wokwi.com/projects/432899754435850241
 *
 * Project Link:
 * https://github.com/DRIFTYY777/WS2812B-ATmega8A
 *
 */

#include <Adafruit_NeoPixel.h>
#include <boards.h>
#include <EEPROM.h>

#if !defined(__AVR_ATmega88__) // ATmega88 isn't supported by IRremote library
#include <IRremote.hpp>
#endif

/* Leds Counts */
#define NUM_LEDS 66 // Number of LEDs in the strip
#define MODE_ADDR 0 // EEPROM address to store mode

// prevent leds burning out because we did not use any resistors and diode
#define BRIGHTNESS 200 // Brightness level (0-255)

// Buffer length for IR receiver Note: This is temporary now. Must change it later based on requirements.
// #define RAW_BUFFER_LENGTH 700 // we require 2 buffer of this size for this example

// Timing constants
constexpr unsigned long DEBOUNCE_DELAY = 50;
constexpr unsigned long FADE_UPDATE_INTERVAL = 30;
constexpr unsigned long NEW_LED_INTERVAL = 500;

// Mode constants
constexpr int NUM_MODES = 12;
constexpr int MAX_FADE_LEDS = 40;
constexpr int ACTIVE_FADE_LIMIT = 30;

/**
 * @brief NeoPixel LED strip instance
 */
Adafruit_NeoPixel strip(NUM_LEDS, DATA_PIN, NEO_GRB + NEO_KHZ800);

/*
 *Global LED modes
 *If adding new modes, make sure to update NUM_MODES accordingly
 */
enum LightMode
{
  RED = 0,
  GREEN,
  BLUE,
  CYAN_PULSE,
  STATIC_RAINBOW,
  MOVING_RAINBOW,
  RAINBOW_CHASE,
  AMBER,
  PURPLE,
  WHITE,
  RANDOM_FADE,
  OFF
};

// Fade LED structure
struct FadeLED
{
  uint8_t index;
  uint8_t brightness;
  int8_t direction;
  bool active;

  void reset()
  {
    brightness = 0;
    direction = 1;
    active = false;
  }
};

// Global state
struct SystemState
{
  int mode = 0;
  bool lastButtonState = HIGH;
  bool currentButtonState = HIGH;
  unsigned long lastDebounceTime = 0;
  uint16_t rainbowOffset = 0;

  unsigned long lastFadeUpdate = 0;

  // Random fade state
  FadeLED fadeLeds[MAX_FADE_LEDS];
  int activeFadeLeds = 0;
  unsigned long lastNewLedTime = 0;
} state;

// Utility functions
inline uint8_t sineFade(float progress)
{
  return static_cast<uint8_t>(127.5f * (1.0f + sin(progress * PI * 2.0f - PI / 2.0f)));
}

inline uint32_t createColor(uint8_t r, uint8_t g, uint8_t b)
{
  return Adafruit_NeoPixel::Color(r, g, b);
}

void clearStrip()
{
  strip.clear();
}

void updateStrip()
{
  strip.show();
}

/// @brief Blinks the inbuilt LED to indicate a mode change.
void blinkInbuiltLED()
{
  digitalWrite(INBUILD_LED, HIGH);
  delay(50);
  digitalWrite(INBUILD_LED, LOW);
}

///@brief Handles the transitionFade between two colors.
void transitionFade(uint32_t colorFrom, uint32_t colorTo, uint8_t steps, uint16_t delayMs)
{
  const float stepSize = 1.0f / steps;

  for (uint8_t i = 0; i <= steps; i++)
  {
    const float t = i * stepSize;
    const float invT = 1.0f - t;

    const uint8_t r = invT * ((colorFrom >> 16) & 0xFF) + t * ((colorTo >> 16) & 0xFF);
    const uint8_t g = invT * ((colorFrom >> 8) & 0xFF) + t * ((colorTo >> 8) & 0xFF);
    const uint8_t b = invT * (colorFrom & 0xFF) + t * (colorTo & 0xFF);

    strip.fill(createColor(r, g, b));
    updateStrip();
    delay(delayMs);
  }
}

/// @brief Handles the random fade effect.
void randomFadeAmber(const unsigned long now)
{
  // Add new LEDs
  if (state.activeFadeLeds < ACTIVE_FADE_LIMIT &&
      now - state.lastNewLedTime > NEW_LED_INTERVAL)
  {

    for (int i = 0; i < MAX_FADE_LEDS; i++)
    {
      if (!state.fadeLeds[i].active)
      {
        state.fadeLeds[i].index = random(NUM_LEDS);
        state.fadeLeds[i].brightness = 0;
        state.fadeLeds[i].direction = 1;
        state.fadeLeds[i].active = true;
        state.activeFadeLeds++;
        state.lastNewLedTime = now;
        break;
      }
    }
  }

  // Update LEDs
  if (now - state.lastFadeUpdate > FADE_UPDATE_INTERVAL)
  {
    clearStrip();

    for (int i = 0; i < MAX_FADE_LEDS; i++)
    {
      if (state.fadeLeds[i].active)
      {
        state.fadeLeds[i].brightness += state.fadeLeds[i].direction * 5;

        if (state.fadeLeds[i].brightness >= 255)
        {
          state.fadeLeds[i].brightness = 255;
          state.fadeLeds[i].direction = -1;
        }
        else if (state.fadeLeds[i].brightness <= 0)
        {
          state.fadeLeds[i].brightness = 0;
          state.fadeLeds[i].active = false;
          state.activeFadeLeds--;
        }

        if (state.fadeLeds[i].active)
        {
          const uint8_t brightness = state.fadeLeds[i].brightness;
          strip.setPixelColor(state.fadeLeds[i].index,
                              createColor(brightness, brightness / 5, 0));
        }
      }
    }
    state.lastFadeUpdate = now;
    updateStrip(); // Add this line to actually display the changes
  }
}

///@brief Mode handlers
void handleStaticColor(uint32_t color)
{
  strip.fill(color);
  strip.setBrightness(BRIGHTNESS);
  updateStrip();
}

// Handle cyan pulse effect
void handleCyanPulse()
{
  static uint8_t brightness = 0;
  static int8_t fadeDirection = 1;
  static unsigned long lastPulseUpdate = 0;

  if (millis() - lastPulseUpdate >= 5)
  {
    lastPulseUpdate = millis();
    brightness += fadeDirection;

    if (brightness == BRIGHTNESS || brightness == 0)
    {
      fadeDirection *= -1;
    }

    strip.fill(createColor(0, 255, 255));
    strip.setBrightness(brightness);
    updateStrip();
  }
}

void handleAmberPulse()
{
  static uint8_t brightness = 0;
  static int8_t fadeDirection = 1;
  static unsigned long lastPulseUpdate = 0;

  if (millis() - lastPulseUpdate >= 5)
  {
    lastPulseUpdate = millis();
    brightness += fadeDirection;

    if (brightness == BRIGHTNESS || brightness == 0)
    {
      fadeDirection *= -1;
    }

    // Amber color: RGB(255, 165, 0)
    strip.fill(createColor(255, 165, 0));
    strip.setBrightness(brightness);
    updateStrip();
  }
}

///@brief Static rainbow effect
void handleStaticRainbow()
{
  for (int i = 0; i < NUM_LEDS; i++)
  {
    strip.setPixelColor(i, Adafruit_NeoPixel::ColorHSV(i * 65536L / NUM_LEDS));
  }
  strip.setBrightness(BRIGHTNESS);
  updateStrip();
}

///@brief Moving rainbow effect
void handleMovingRainbow()
{
  for (int i = 0; i < NUM_LEDS; i++)
  {
    const uint16_t color = (i * 65536L / NUM_LEDS + state.rainbowOffset) % 65536;
    strip.setPixelColor(i, Adafruit_NeoPixel::ColorHSV(color));
  }
  state.rainbowOffset += 256;
  strip.setBrightness(BRIGHTNESS);
  updateStrip();
}

///@brief Rainbow chase effect
void handleRainbowChase()
{
  for (int i = 0; i < NUM_LEDS; i++)
  {
    const uint16_t hue = ((static_cast<uint32_t>(i) * 1000UL) + state.rainbowOffset) % 65536;
    strip.setPixelColor(i, Adafruit_NeoPixel::ColorHSV(hue));
  }
  state.rainbowOffset += 1000;
  strip.setBrightness(BRIGHTNESS);
  updateStrip();
}

///@brief Handle off mode
void handleOffMode()
{
  clearStrip();
  strip.setBrightness(0);
  updateStrip();
}

///@brief Button handling
void handleButton()
{
  const bool reading = digitalRead(BTN_PIN);
  if (reading != state.lastButtonState)
  {
    state.lastDebounceTime = millis();
  }
  if ((millis() - state.lastDebounceTime) > DEBOUNCE_DELAY)
  {
    if (state.currentButtonState == HIGH && reading == LOW)
    {
      state.mode = (state.mode + 1) % NUM_MODES;
      EEPROM.write(MODE_ADDR, state.mode);
      blinkInbuiltLED();
    }
    state.currentButtonState = reading;
  }
  state.lastButtonState = reading;
}

/** @brief Initializes the IR receiver.
 *  @brief It's initializes based on the capabilities of MCU and the IRremote library.
 */
void initIRReceiver()
{
#if !defined(__AVR_ATmega88__) // ATmega88 isn't supported by IRremote library
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK, INBUILD_LED);
  // ENABLE_LED_FEEDBACK, INBUILD_LED for testing purposes so we can see the IR receiver is working.
#endif
}

/** @brief
 * Handles IR receive functionality.
 */
void handleIRReceive() // Currently this function is not used, but it can be used to handle IR receive functionality.
{
  if (IrReceiver.decode())
  {

    IrReceiver.printIRResultShort(&Serial);
    if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_WAS_OVERFLOW)
    {
      Serial.println("Try to increase the RAW_BUFFER_LENGTH value or if not defined define it in your sketch.");
    }
    else
    {
      if (IrReceiver.decodedIRData.protocol == UNKNOWN)
      {
        Serial.println(F("Received noise or an unknown (or not yet enabled) protocol"));
      }
      Serial.println();
      IrReceiver.printIRSendUsage(&Serial);
      Serial.println();
      Serial.println(F("Raw result in internal ticks (50 us) - with leading gap"));
      IrReceiver.printIRResultRawFormatted(&Serial, false); // Output the results in RAW format
      Serial.println(F("Raw result in microseconds - with leading gap"));
      IrReceiver.printIRResultRawFormatted(&Serial, true); // Output the results in RAW format
      Serial.println();                                    // blank line between entries
      Serial.print(F("Result as internal 8bit ticks (50 us) array - compensated with MARK_EXCESS_MICROS="));
      Serial.println(MARK_EXCESS_MICROS);
      IrReceiver.compensateAndPrintIRResultAsCArray(&Serial, false); // Output the results as uint8_t source code array of ticks
      Serial.print(F("Result as microseconds array - compensated with MARK_EXCESS_MICROS="));
      Serial.println(MARK_EXCESS_MICROS);
      IrReceiver.compensateAndPrintIRResultAsCArray(&Serial, true); // Output the results as uint16_t source code array of micros
      IrReceiver.printIRResultAsCVariables(&Serial);                // Output address and data as source code variables
      Serial.println();                                             // blank line between entries

      IrReceiver.compensateAndPrintIRResultAsPronto(&Serial);
    }
    IrReceiver.resume(); // Prepare for the next IR frame
  }
}

void setup()
{
  randomSeed(analogRead(0)); // Seed randomness

  pinMode(BTN_PIN, INPUT_PULLUP);
  pinMode(INBUILD_LED, OUTPUT);
  digitalWrite(INBUILD_LED, LOW); // Turn off inbuilt LED

// check if its ESP family
#if defined(ESP8266) || defined(ESP32)
  EEPROM.begin(MCU_EEPROM_SIZE);
#else
  EEPROM.begin(); //
#endif

  blinkInbuiltLED(); // Indicate setup start

  initIRReceiver(); // Initialize IR receiver

  state.mode = EEPROM.read(MODE_ADDR);
  if (state.mode >= NUM_MODES)
  {
    state.mode = 0; // Sanity check
  }

  strip.begin();
  strip.setBrightness(BRIGHTNESS);
  strip.show();
}

void loop()
{
  //  Button Handling
  handleButton();
  // Handle the current mode
  switch (static_cast<LightMode>(state.mode))
  {
  case RED:
    handleStaticColor(createColor(255, 0, 0));
    break; // Red color
  case GREEN:
    handleStaticColor(createColor(0, 255, 0));
    break; // Green color
  case BLUE:
    handleStaticColor(createColor(0, 0, 255));
    break; // Blue color
  case CYAN_PULSE:
    handleCyanPulse();
    break; // Cyan pulse effect
  case STATIC_RAINBOW:
    handleStaticRainbow();
    delay(50);
    break; // Static rainbow effect
  case MOVING_RAINBOW:
    handleMovingRainbow();
    delay(50);
    break; // Moving rainbow effect
  case RAINBOW_CHASE:
    handleRainbowChase();
    delay(100);
    break; // Rainbow chase effect
  case AMBER:
    handleAmberPulse();
    break; // Amber color
  case PURPLE:
    handleStaticColor(createColor(128, 0, 128));
    break; // Purple color
  case WHITE:
    handleStaticColor(createColor(255, 200, 255));
    break; // White color
  case RANDOM_FADE:
    randomFadeAmber(millis());
    break; // Sleep mode with sub-modes
  case OFF:
    handleOffMode();
    break; // Off mode
  }
  delay(10);
}