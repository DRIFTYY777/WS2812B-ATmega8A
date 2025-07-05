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
 * 7: Amber
 * 8: Purple
 * 9: White
 * 10: Sleep Mode
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
 */

#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>

/*
  Works on my diy board with ATmega8a
*/

// #define DATA_PIN 2 // ARGB pin
// #define BTN_PIN 3  // Button pin on board

// Works on a schematic board with ATmega8a
#define DATA_PIN A2 // ARGB pin
#define BTN_PIN A3  // Button pin on board

/* Data Pin and No of leds */
#define INBUILD_LED 9 // Inbuilt LED pin
#define NUM_LEDS 66   // Number of LEDs in the strip
#define MODE_ADDR 0   // EEPROM address to store mode

// prevent leds burning out because we did not use any resistors and diode
#define BRIGHTNESS 200 // Brightness level (0-255)


/**
 * @brief NeoPixel strip object
 * This object controls the NeoPixel strip.
 */
Adafruit_NeoPixel strip(NUM_LEDS, DATA_PIN, NEO_GRB + NEO_KHZ800);



// global variables
int mode = 0;
constexpr int numModes = 12; // Number of modes
bool lastButtonState = HIGH;
bool currentButtonState = HIGH;
unsigned long lastDebounceTime = 0;
constexpr unsigned long debounceDelay = 50;
uint16_t rainbowOffset = 0;



// Timing variables for sleep mode

// Modes
enum Mode { PULSE, RANDOM_FADE };
Mode currentMode = PULSE;


unsigned long lastSwitch = 0;
unsigned long switchInterval = 20000; // 60 seconds for mode switch
unsigned long pulseDuration = 12000;  // 12s = slow fade (6s up + 6s down)
unsigned long lastFadeUpdate = 0;
unsigned long lastFlicker = 0;
const unsigned long fadeStepDelay = 30;

// Pulse
uint8_t flickerLed = 0;

// Random Fade Mode
uint8_t ledBrightness[NUM_LEDS];
bool ledActive[NUM_LEDS];
int currentFadeIndex = 0;
int randomOrder[NUM_LEDS];
const uint8_t fadeStep = 5;


// Flicker effect variables
uint8_t flickerBrightness = 0;
int flickerDirection = 1; // 1 = fade in, -1 = fade out
unsigned long lastFlickerUpdate = 0;
constexpr unsigned long flickerStepDelay = 20;  // speed of fade step
constexpr unsigned long flickerInterval = 500; // delay before new flicker
bool isFlickering = false;





// Sine fade function
uint8_t sineFade(float progress) {
  return (uint8_t)(127.5 * (1.0 + sin(progress * PI * 2 - PI / 2)));
}

void transitionFade(uint32_t colorFrom, uint32_t colorTo, uint8_t steps, uint16_t delayMs) {
  for (uint8_t i = 0; i <= steps; i++) {
    float t = i / (float)steps;
    uint8_t r = (1 - t) * ((colorFrom >> 16) & 0xFF) + t * ((colorTo >> 16) & 0xFF);
    uint8_t g = (1 - t) * ((colorFrom >> 8) & 0xFF) + t * ((colorTo >> 8) & 0xFF);
    uint8_t b = (1 - t) * ((colorFrom >> 0) & 0xFF) + t * ((colorTo >> 0) & 0xFF);
    for (int j = 0; j < NUM_LEDS; j++) {
      strip.setPixelColor(j, strip.Color(r, g, b));
    }
    strip.show();
    delay(delayMs);
  }
}


// --------------------- PULSE MODE ----------------------
void handlePulse(unsigned long now) {
  float progress = (now % pulseDuration) / (float)pulseDuration;
  uint8_t pulseBrightness = sineFade(progress); // smooth sine fade

  // Smooth flicker handling
  if (isFlickering) {
    if (now - lastFlickerUpdate > flickerStepDelay) {
      lastFlickerUpdate = now;
      flickerBrightness += flickerDirection * 15;

      if (flickerBrightness >= 255) {
        flickerBrightness = 255;
        flickerDirection = -1;
      } else if (flickerBrightness <= 0) {
        flickerBrightness = 0;
        isFlickering = false;
      }
    }
  } else {
    // Start new flicker after delay
    if (now - lastFlickerUpdate > flickerInterval) {
      flickerLed = random(NUM_LEDS);
      flickerBrightness = 0;
      flickerDirection = 1;
      isFlickering = true;
      lastFlickerUpdate = now;
    }
  }

  // Render the whole strip
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t r = pulseBrightness;
    uint8_t g = pulseBrightness / 5;

    // If this is the flicker LED, blend flicker on top
    if (i == flickerLed && isFlickering) {
      r = max(r, flickerBrightness);
      g = max(g, flickerBrightness / 15);
    }

    strip.setPixelColor(i, Adafruit_NeoPixel::Color(r, g, 0));
  }
}


// ------------------- RANDOM FADE MODE --------------------
void handleRandomFade(unsigned long now) {
  if (now - lastFadeUpdate > fadeStepDelay) {
    lastFadeUpdate = now;

    // Only proceed if we haven't finished all LEDs
    if (currentFadeIndex < NUM_LEDS) {
      int ledIndex = randomOrder[currentFadeIndex];

      // Fade in the current LED
      if (!ledActive[ledIndex]) {
        ledBrightness[ledIndex] += fadeStep;
        if (ledBrightness[ledIndex] >= 255) {
          ledBrightness[ledIndex] = 255;
          ledActive[ledIndex] = true;
          currentFadeIndex++;  // Move to next LED only when current one is done
        }
      }
    }

    // Update the entire strip display
    for (int i = 0; i < NUM_LEDS; i++) {
      if (ledBrightness[i] > 0) {
        uint8_t b = ledBrightness[i];
        strip.setPixelColor(i, Adafruit_NeoPixel::Color(b, b / 5, 0));
      } else {
        strip.setPixelColor(i, 0); // Ensure it's off until it's supposed to glow
      }
    }
  }
}


void shuffleRandomOrder() {
  for (int i = 0; i < NUM_LEDS; i++) {
    randomOrder[i] = i;
  }

  for (int i = NUM_LEDS - 1; i > 0; i--) {
    int j = random(i + 1);
    const int temp = randomOrder[i];
    randomOrder[i] = randomOrder[j];
    randomOrder[j] = temp;
  }
}


void sleepMode() {
  unsigned long now = millis();

  // Mode switch with transition
  if (now - lastSwitch > switchInterval) {
    uint32_t fromColor = (currentMode == PULSE)
      ? Adafruit_NeoPixel::Color(255, 50, 0)
      : Adafruit_NeoPixel::Color(ledBrightness[randomOrder[currentFadeIndex]], ledBrightness[randomOrder[currentFadeIndex]] / 5, 0);

    currentMode = (currentMode == PULSE) ? RANDOM_FADE : PULSE;
    lastSwitch = now;

    // Reset for random fade
    if (currentMode == RANDOM_FADE) {
      currentFadeIndex = 0;
      for (int i = 0; i < NUM_LEDS; i++) {
        ledBrightness[i] = 0;
        ledActive[i] = false;
      }
      shuffleRandomOrder();
    }

    uint32_t toColor = (currentMode == PULSE)
      ? Adafruit_NeoPixel::Color(255, 50, 0)
      : Adafruit_NeoPixel::Color(ledBrightness[randomOrder[0]], ledBrightness[randomOrder[0]] / 5, 0);

    transitionFade(fromColor, toColor, 50, 15); // 750ms transition
  }

  shuffleRandomOrder(); // randomize order each time

  // Run active mode
  if (currentMode == PULSE) {
    handlePulse(now);
  } else {
    handleRandomFade(now);
  }

  strip.show();
}

// Function to blink the inbuilt LED for feedback
void blinkInbuiltLED()
{
  digitalWrite(INBUILD_LED, HIGH);
  delay(50);
  digitalWrite(INBUILD_LED, LOW);
}

void setup()
{
  randomSeed(analogRead(0)); // Seed randomness

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
  //  Button Handling
  const bool reading = digitalRead(BTN_PIN);
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

  // Mode Handling
  switch (mode)
  {
  case 0: // Red
    strip.fill(Adafruit_NeoPixel::Color(255, 0, 0));
    strip.setBrightness(BRIGHTNESS);
    strip.show();
    break;

  case 1: // Green
    strip.fill(Adafruit_NeoPixel::Color(0, 255, 0));
    strip.setBrightness(BRIGHTNESS);
    strip.show();
    break;

  case 2: // Blue
    strip.fill(Adafruit_NeoPixel::Color(0, 0, 255));
    strip.setBrightness(BRIGHTNESS);
    strip.show();
    break;

  case 3: // Cyan Pulse
  {
    static uint8_t brightness = 0;
    static int fadeDirection = 1;
    static unsigned long lastPulseUpdate = 0;

    if (constexpr unsigned long pulseInterval = 5; millis() - lastPulseUpdate >= pulseInterval)
    {
      lastPulseUpdate = millis();
      brightness += fadeDirection;
      if (brightness == BRIGHTNESS || brightness == 0)
        fadeDirection *= -1;

      strip.fill(Adafruit_NeoPixel::Color(0, 255, 255));
      strip.setBrightness(brightness);
      strip.show();
    }
    break;
  }

  case 4: // Static Rainbow
    for (int i = 0; i < NUM_LEDS; i++)
    {
      strip.setPixelColor(i, Adafruit_NeoPixel::ColorHSV(i * 65536L / NUM_LEDS));
    }
    strip.setBrightness(BRIGHTNESS);
    strip.show();
    delay(50);
    break;

  case 5: // ARGB-style Moving Rainbow
    for (int i = 0; i < NUM_LEDS; i++)
    {
      uint16_t color = (i * 65536L / NUM_LEDS + rainbowOffset) % 65536;
      strip.setPixelColor(i, Adafruit_NeoPixel::ColorHSV(color));
    }
    rainbowOffset += 256;
    strip.setBrightness(BRIGHTNESS);
    strip.show();
    delay(50);
    break;

    case 6: // Rainbow Chase
      for (int i = 0; i < NUM_LEDS; i++)
      {
        // Use unsigned long to prevent overflow
        const uint16_t hue = ((static_cast<uint32_t>(i) * 1000UL) + rainbowOffset) % 65536;
        strip.setPixelColor(i, Adafruit_NeoPixel::ColorHSV(hue));
      }
      rainbowOffset += 1000;
      strip.setBrightness(BRIGHTNESS);
      strip.show();
      delay(100);
      break;

  case 7: // amber
    for (int i = 0; i < NUM_LEDS; i++)
    {
      strip.setPixelColor(i, Adafruit_NeoPixel::Color(255, 165, 0));
    }
    strip.setBrightness(BRIGHTNESS);
    strip.show();
    break;

  case 8: // purple
    for (int i = 0; i < NUM_LEDS; i++)
    {
      strip.setPixelColor(i, Adafruit_NeoPixel::Color(128, 0, 128));
    }
    strip.setBrightness(BRIGHTNESS);
    strip.show();
    break;

  case 9: // white
    for (int i = 0; i < NUM_LEDS; i++)
    {
      strip.setPixelColor(i, Adafruit_NeoPixel::Color(255, 255, 255));
    }
    strip.setBrightness(BRIGHTNESS);
    strip.show();
    break;

  case 10: // sleep mode
    sleepMode();
    break;

  case 11: // off
    for (int i = 0; i < NUM_LEDS; i++)
    {
      strip.setPixelColor(i, Adafruit_NeoPixel::Color(0, 0, 0));
    }
    strip.setBrightness(0);
    strip.clear();
    strip.show();
    break;
  default:
      break;
  }
  delay(10);
}