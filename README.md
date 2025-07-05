# WS2812B LED Strip Controller with ATmega8A/ATmega8

This project demonstrates a simple yet powerful RGB LED strip controller using the **WS2812B** LED strip and **ATmega8A** microcontroller. The firmware is written in **Arduino** using the Arduino bootloader and runs at a **16 MHz clock speed**.

## 📌 Features

- **Microcontroller**: ATmega8A (Arduino Bootloader)
- **LED Strip**: WS2812B RGB LEDs (66 LEDs)
- **Clock Speed**: 16 MHz
- **Single Push Button Control**: Cycle through all lighting modes with one button
- **Button Activity LED**: A status LED blinks for 50 ms on each button press
- **Memory Persistence**: Last mode is saved to EEPROM and restored on power-up
- **Advanced Sleep Mode**: Dynamic transitions between pulse and random fade effects
- **Optimized Performance**: Efficient memory usage and improved code structure

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
| LED Count | 66 LEDs |
| Data Pin | A2 |
| Button Pin | A3 |
| Status LED Pin | 9 |
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

- Arduino IDE
- ATmega8A with Arduino bootloader
- WS2812B LED strip (66 LEDs)
- Push button
- 1 status LED
- `Adafruit_NeoPixel` library

### 🔌 Connections

| Component        | ATmega8A Pin | Arduino Pin |
|------------------|--------------|-------------|
| WS2812B DIN      | PC2          | A2          |
| Push Button      | PC3          | A3          |
| Status LED       | PB1          | 9           |
| VCC              | VCC          | 5V          |
| GND              | GND          | GND         |

### 📥 Installation

1. Install the `Adafruit_NeoPixel` library in Arduino IDE
2. Connect components according to the schematic
3. Upload the optimized sketch to your ATmega8A
4. Press the button to cycle through modes

---

## 🛠 Customization

The code is highly modular and easily customizable:

- **Change LED count**: Modify `NUM_LEDS` constant
- **Adjust brightness**: Change `BRIGHTNESS` constant (0-255)
- **Add new modes**: Add to `LightMode` enum and create handler function
- **Modify timing**: Adjust timing constants at the top of the file
- **Change colors**: Use the `createColor(r, g, b)` helper function

---

## 💡 Contributions

Feel free to fork this repository, raise issues or submit pull requests if you enhance the functionality or fix bugs. Areas for potential improvement:

- Additional lighting effects
- IR remote control support
- Sound-reactive modes
- WiFi connectivity for ESP8266/ESP32 variants

---

## 📜 License

This project is open source and available under the [MIT License](LICENSE).

---

## 🤝 Author

**DRIFTYY777**

> GitHub: [@DRIFTYY777](https://github.com/DRIFTYY777)

---

## 📊 Version History

- **v2.0** - Optimized code structure, added sleep mode with dual effects
- **v1.0** - Initial release with basic color modes and rainbow effects