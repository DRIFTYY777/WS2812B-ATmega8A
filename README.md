# WS2812B LED Strip Controller with ATmega8A/ATmega8

This project demonstrates a simple yet powerful RGB LED strip controller using the **WS2812B** LED strip and **ATmega8A** microcontroller. The firmware is written in **Arduino** using the Arduino bootloader and runs at a **16 MHz clock speed**.

## 📌 Features

- **Multi-Board Support**: ATmega8A, ATmega88, ATmega168, ATmega328P, ATtiny85, ESP8266, ESP32
- **LED Strip**: WS2812B RGB LEDs (66 LEDs)
- **Clock Speed**: 16 MHz
- **Single Push Button Control**: Cycle through all lighting modes with one button
- **Button Activity LED**: A status LED blinks for 50 ms on each button press
- **Memory Persistence**: Last mode is saved to EEPROM and restored on power-up
- **Advanced Sleep Mode**: Dynamic transitions between pulse and random fade effects
- **Optimized Performance**: Efficient memory usage and improved code structure
- **Wokwi Simulation**: Test the project online without hardware

## 🌐 Wokwi Simulation
Experience the project without physical hardware! Try our interactive online simulation:

🔗 **[Live Wokwi Simulation](https://wokwi.com/projects/432899754435850241)**

### Simulation Features:
- **Interactive Button**: Click to cycle through all 12 lighting modes
- **Real-time LED Control**: See WS2812B LEDs respond instantly
- **Multiple Board Options**: Test with different microcontrollers
- **Code Editing**: Modify and test code changes in real-time
- **No Hardware Required**: Perfect for learning and prototyping

> 💡 The simulation includes all 12 lighting modes and demonstrates the complete functionality of the physical project.

## 🎯 Board Compatibility

| Board | Microcontroller | Flash | EEPROM | Status | Notes |
|-------|----------------|-------|--------|---------|-------|
| ATmega8A | ATmega8A | 8KB | 512B | ✅ Primary | Original target board |
| ATmega88 | ATmega88 | 8KB | 512B | ✅ Tested | Pin-compatible upgrade |
| ATmega168 | ATmega168 | 16KB | 512B | ✅ Tested | More flash memory |
| Arduino Uno/Nano | ATmega328P | 32KB | 1KB | ✅ Tested | Most popular Arduino boards |
| ATtiny85 | ATtiny85 | 8KB | 512B | ✅ Compatible | Compact design |
| ESP8266 | ESP8266 | 1MB | 4KB | ✅ WiFi Ready | Wireless capabilities |
| ESP32 | ESP32 | 4MB | 4KB | ✅ WiFi + BT | Advanced features |

---


## 🎨 Lighting Modes (12 Modes)

1. **Solid Red**
2. **Solid Green**
3. **Solid Blue**
4. **Cyan Pulse** - Smooth breathing effect
5. **Static Rainbow** - Fixed rainbow pattern across all LEDs
6. **Moving Rainbow** - ARGB-style flowing rainbow effect
7. **Rainbow Chase** - Fast-moving rainbow chase pattern
8. **Amber** - Warm amber lighting
9. **Purple** - Deep purple lighting
10. **White** - Pure white lighting
11. **Sleep Mode** - Alternates between:
    - **Pulse Mode**: Gentle orange breathing with random LED flickers
    - **Random Fade Mode**: 15-30 LEDs randomly fade in/out simultaneously
12. **OFF** - All LEDs turned off

Each press of the button switches to the next mode in the sequence.

---


## 🧠 How It Works

- **Push Button**: Connected to analog pin A3 with internal pull-up enabled and debouncing
- **LED Strip**: Controlled using `Adafruit_NeoPixel` library with optimized color functions
- **Status LED**: Connected to pin 9 and flashes briefly on each button press
- **Memory Management**: Current mode saved to EEPROM address 0 for persistence
- **Performance**: Optimized state management and efficient LED updates

---

## 🔧 Technical Specifications

| Parameter | Value |
|-----------|-------|
| Supported Boards | ATmega8A, ATmega88, ATmega168, ATmega328P, ATtiny85, ESP8266, ESP32 |
| LED Count | 66 LEDs (configurable) |
| Data Pin | Auto-configured per board |
| Button Pin | Auto-configured per board |
| Status LED Pin | Auto-configured per board |
| Default Brightness | 200/255 |
| Update Rate | Variable (10-100ms depending on mode) |
| Memory Usage | Optimized with struct-based state management |

---

## ⚡ Advanced Features

### Sleep Mode Details
- **Auto-switching**: Alternates between pulse and random fade every 20 seconds
- **Smooth transitions**: 50-step fade transitions between sub-modes
- **Pulse mode**: Sine wave breathing with random LED flickers every 500ms
- **Random fade**: Up to 30 LEDs fade in/out independently with 500ms intervals

### Code Optimizations
- **Modular design**: Each mode handled by dedicated functions
- **Memory efficient**: Uses appropriate data types (`uint8_t`, `int8_t`)
- **Compile-time constants**: `constexpr` for better performance
- **State management**: Consolidated global state in single struct
- **Type safety**: Enums for modes and proper type casting

---

## 📷 Images

Below are some images of the working setup and prototype:

| Image                                                                                                                  | Description |
|------------------------------------------------------------------------------------------------------------------------|-----------|
| ![IMG_9293](https://github.com/DRIFTYY777/WS2812B-ATmega8A/raw/main/images/IMG_9293.png)                               | DIY Board Setup |
| ![IMG_9295](https://github.com/DRIFTYY777/WS2812B-ATmega8A/raw/main/images/IMG_9295.png)                               | ATmega8A DIP Package |
| ![IMG_9298](https://github.com/DRIFTYY777/WS2812B-ATmega8A/raw/main/images/IMG_9298.gif)                               | Mode Demonstration |
| ![Schematic](https://github.com/DRIFTYY777/WS2812B-ATmega8A/raw/main/images/Schematic.png) | Schematic |

> 🔗 More images available in the [Images folder](https://github.com/DRIFTYY777/WS2812B-ATmega8A/tree/main/images)

---

## 📐 Schematic

You can find the circuit schematic below:

📄 [Schematic_WBS2812B-ATmega8A.pdf](https://github.com/DRIFTYY777/WS2812B-ATmega8A/blob/main/schematic/Schematic_WBS2812B-ATmgea8A.pdf)

> 🗂 Full schematic files are available in the [schematic folder](https://github.com/DRIFTYY777/WS2812B-ATmega8A/tree/main/schematic)

---

## 🚀 Getting Started

### 🛠 Requirements

- Arduino IDE or PlatformIO
- One of the supported boards:
  - **ATmega8A** with Arduino bootloader
  - **ATmega88/168** development boards
  - **Arduino Uno/Nano** (ATmega328P)
  - **ATtiny85** development board
  - **ESP8266** (NodeMCU, Wemos D1 Mini)
  - **ESP32** development board
- WS2812B LED strip (66 LEDs or configurable)
- Push button
- 1 status LED (optional - can use built-in LED)
- `Adafruit_NeoPixel` library

### 🔌 Connections

**Pin assignments are automatically configured based on the target board:**

| Board Type | Data Pin | Button Pin | Status LED |
|------------|----------|------------|------------|
| ATmega8A | A2 | A3 | Pin 9 |
| ATmega88/168/328P | Pin 2 | Pin 3 | Pin 13 (Built-in) |
| ATtiny85 | Pin 0 | Pin 1 | Pin 2 |
| ESP8266 | GPIO4 | GPIO0 | GPIO2 (Built-in) |
| ESP32 | GPIO4 | GPIO0 | GPIO2 (Built-in) |

### 📥 Installation

#### Physical Hardware:
1. Install the `Adafruit_NeoPixel` library in Arduino IDE
2. Select your target board from the supported list
3. Connect components according to your board's pin configuration
4. Upload the sketch to your microcontroller
5. Press the button to cycle through modes

#### Using PlatformIO:
1. Open the project in PlatformIO
2. Select your target environment (ATmega8, ATmega328P, esp8266, esp32, etc.)
3. Build and upload: `pio run -e [your_board] --target upload`

#### Wokwi Simulation:
1. Click the [Wokwi Simulation Link](https://wokwi.com/projects/432899754435850241)
2. Press the virtual button to test all modes
3. Modify code directly in the browser
4. Download the code for your physical project

---

## 🛠 Customization

The code is highly modular and easily customizable:

- **Change LED count**: Modify `NUM_LEDS` constant
- **Adjust brightness**: Change `BRIGHTNESS` constant (0-255)
- **Add new modes**: Add to `LightMode` enum and create handler function
- **Modify timing**: Adjust timing constants at the top of the file
- **Change colors**: Use the `createColor(r, g, b)` helper function
- **Board-specific optimizations**: Pin assignments automatically handled in `boards.h`

---

## 💡 Contributions

Feel free to fork this repository, raise issues or submit pull requests if you enhance the functionality or fix bugs. Areas for potential improvement:

- Additional lighting effects
- IR remote control support
- Sound-reactive modes
- WiFi connectivity for ESP8266/ESP32 variants
- Additional board support (STM32, Raspberry Pi Pico)

---

## 📜 License

This project is open source and available under the [MIT License](LICENSE).

---

## 🤝 Author

**DRIFTYY777**

> GitHub: [@DRIFTYY777](https://github.com/DRIFTYY777)

---

## 📊 Version History

- **v3.0** - Added multi-board support (ATmega88/168/328P, ATtiny85, ESP8266/ESP32) and Wokwi simulation
- **v2.5** - Added sleep mode with dual effects, optimized memory usage
- **v2.0** - Optimized code structure and performance, added new lighting modes
- **v1.0** - Initial release with basic color modes and rainbow effects