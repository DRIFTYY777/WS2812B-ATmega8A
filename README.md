# WS2812B LED Strip Controller with ATmega8A/ATmega8

This project demonstrates a simple yet powerful RGB LED strip controller using the **WS2812B** LED strip and **ATmega8A** microcontroller. The firmware is written in **Arduino** using the Arduino bootloader and runs at a **16 MHz clock speed**.

## 📌 Features

- **Microcontroller**: ATmega8A (Arduino Bootloader)
- **LED Strip**: WS2812B RGB LEDs
- **Clock Speed**: 16 MHz
- **Single Push Button Control**: Cycle through all lighting modes with one button
- **Button Activity LED**: A status LED blinks for 50 ms on each button press

## 🎨 Lighting Modes (10 Modes)

1. **Solid Red**
2. **Solid Green**
3. **Solid Blue**
4. **Cyan Pulse**
5. **Static Rainbow**
6. **ARGB-style Moving Rainbow**
7. **Rainbow Chase**
8. **Amber**
9. **Purple**
10. **White**
11. **OFF**

Each press of the button switches to the next mode in the sequence.

---

## 🧠 How It Works

- **Push Button**: Connected to a digital input with internal pull-up enabled.
- **LED Strip**: Controlled using `Adafruit_NeoPixel` library.
- **Status LED**: Connected to a GPIO pin and flashes briefly on each button press.

---

## 📷 Images

Below are some images of the working setup and prototype:

| Image | Description |
|-------|-------------|
| ![IMG_9293](https://github.com/DRIFTYY777/WS2812B-ATmega8A/raw/main/images/IMG_9293.png) | Project Setup |
| ![IMG_9295](https://github.com/DRIFTYY777/WS2812B-ATmega8A/raw/main/images/IMG_9295.png) | Working Prototype |
| ![IMG_9298](https://github.com/DRIFTYY777/WS2812B-ATmega8A/raw/main/images/IMG_9298.gif) | Mode Demonstration |

> 🔗 More images available in the [Images folder](https://github.com/DRIFTYY777/WS2812B-ATmega8A/tree/main/images)

---

## 📐 Schematic

You can find the circuit schematic below:

📄 [Schematic_WBS2812B-ATmega8A.pdf](https://github.com/DRIFTYY777/WS2812B-ATmega8A/raw/main/schematic/Schematic_WBS2812B-ATmega8A.pdf)

> 🗂 Full schematic files are available in the [schematic folder](https://github.com/DRIFTYY777/WS2812B-ATmega8A/tree/main/schematic)

---

## 🚀 Getting Started

### 🛠 Requirements

- Arduino IDE
- ATmega8A with Arduino bootloader
- WS2812B LED strip
- Push button
- 1 status LED

### 🔌 Connections Overview

| Component        | Pin              |
|------------------|------------------|
| WS2812B DIN      | Digital Pin (as defined in sketch) |
| Push Button      | Digital Pin (pulled up) |
| Status LED       | Any available Digital Pin |

---

## 💡 Contributions

Feel free to fork this repository, raise issues or submit pull requests if you enhance the functionality or fix bugs.

---

## 📜 License

This project is open source and available under the [MIT License](LICENSE).

---

## 🤝 Author

**DRIFTYY777**

> GitHub: [@DRIFTYY777](https://github.com/DRIFTYY777)
