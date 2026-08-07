# ESP32 Water Level Monitoring System

A smart water tank monitoring and automatic pump controller system using ESP32, HC-SR04 ultrasonic sensor, and I2C LCD display with Blynk IoT integration.

---

## 📋 Project Overview

This project provides a complete solution for:
- **Real-time water level monitoring** using ultrasonic sensor
- **Automatic pump control** based on water level thresholds
- **Visual status indicators** with 4 LED lights
- **Remote monitoring** via Blynk IoT mobile app
- **LCD display** for local status information
- **Hardened I2C handling** with auto-recovery features

---

## 🔧 Hardware Components

| Component | Quantity | Description |
|-----------|----------|-------------|
| ESP32 Microcontroller | 1 | Main control board |
| HC-SR04 Ultrasonic Sensor | 1 | Water level detection |
| 16x2 I2C LCD Display | 1 | Status display |
| LED Bulbs | 4 | Red, Yellow, Yellow, Red (level indicators) |
| 180Ω Resistors | 4 | LED current limiting |
| 5V Relay Module | 1 | Pump control switch |
| AC/DC Water Pump | 1 | Water pumping motor |
| 5V External Power Supply | 1 | System power |
| Connecting Wires | As needed | Jumper wires |
| Breadboard | 1 | For circuit assembly |

---

## 📌 Pin Connections

### ESP32 GPIO Pins
```
GPIO 2   → LED1 (Red)
GPIO 4   → LED2 (Yellow)
GPIO 5   → LED3 (Yellow)
GPIO 18  → LED4 (Red)
GPIO 12  → HC-SR04 Trigger Pin (TRIG)
GPIO 13  → HC-SR04 Echo Pin (ECHO)
GPIO 14  → Relay Module Signal Pin
GPIO 21  → I2C SDA (LCD Data)
GPIO 22  → I2C SCL (LCD Clock)
```

### I2C LCD Module (16x2)
```
VCC  → 5V Power
GND  → Ground
SDA  → GPIO 21 (Data Line)
SCL  → GPIO 22 (Clock Line)
```

### HC-SR04 Ultrasonic Sensor
```
VCC   → 5V Power
GND   → Ground
TRIG  → GPIO 12 (Trigger)
ECHO  → GPIO 13 (Echo)
```

### LED Indicators with Resistors
```
LED1 (Red)    → GPIO 2  + 180Ω Resistor → GND
LED2 (Yellow) → GPIO 4  + 180Ω Resistor → GND
LED3 (Yellow) → GPIO 5  + 180Ω Resistor → GND
LED4 (Red)    → GPIO 18 + 180Ω Resistor → GND
```

### 5V Relay Module
```
Signal Pin  → GPIO 14
VCC        → 5V Power
GND        → Ground
COM        → AC/DC Supply voltage (Pump Power Input)
NO         → Water Pump positive terminal
GND        → Water Pump negative terminal
```

---

## 🚀 Features & Water Level Stages

### LED Indicator Levels
```
Very Low (0-35%)   → LED1 ON (Red)                    → Pump turns ON
Low (35-55%)       → LED1 + LED2 ON                   → Pump continues
Medium (55-65%)    → LED1 + LED2 + LED3 ON            → Monitoring
Full (65-100%)     → All 4 LEDs ON (Red+Yellow+...)   → Pump turns OFF
```

### LCD Display Features
- Real-time water level distance display in CM
- Pump status (ON/OFF)
- Fill percentage indicator (0-100%)
- Auto-detection of I2C address (0x27 or 0x3F)
- Scheduled refresh every 60 seconds to prevent display errors
- Auto-recovery if LCD disconnects during operation

### Blynk IoT Integration
- Remote monitoring via smartphone app (iOS/Android)
- Virtual pin V0: Water level gauge (0-100%)
- Virtual pin V1: Motor control button (ON/OFF)
- Real-time cloud notifications
- Cloud data logging and history

---

## 💻 Software Requirements

### Arduino IDE Libraries
```
LiquidCrystal_I2C.h  - LCD control
Wire.h               - I2C communication
WiFi.h               - WiFi connectivity
BlynkSimpleEsp32.h   - Blynk IoT platform
```

### Step-by-Step Installation Guide

#### Step 1: Download Arduino IDE
- Download from https://www.arduino.cc/en/software
- Install on your computer

#### Step 2: Add ESP32 Board Support
1. Open Arduino IDE
2. Go to **File → Preferences**
3. In "Additional Board Manager URLs" paste:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
4. Click **OK**
5. Go to **Tools → Board → Boards Manager**
6. Search for "ESP32"
7. Click **Install** by Espressif Systems

#### Step 3: Install Required Libraries
1. Go to **Sketch → Include Library → Manage Libraries**
2. Search and install these libraries:
   - `LiquidCrystal I2C` by Frank de Brabander
   - `Blynk` by Volodymyr Shymanskyy
3. Click Install for each library

#### Step 4: Upload the Code
1. Open `water_level_blynk.ino`
2. Select **Tools → Board → ESP32 Dev Module**
3. Select your COM port
4. Click **Upload** button

---

## 📝 Code Configuration

Before uploading, edit these parameters in `water_level_blynk.ino`:

```cpp
// WiFi Credentials - Change to your network
char ssid[] = "YOUR_WIFI_SSID";      // Your WiFi network name
char pass[] = "YOUR_WIFI_PASSWORD";  // Your WiFi password

// Blynk Authentication Token
char auth[] = "YOUR_BLYNK_AUTH_TOKEN";  // From Blynk app

// Tank Configuration
int MaxLevel = 13;  // Distance from sensor to EMPTY tank bottom (in CM)
```

### Example Configuration:
```cpp
char ssid[] = "POCO";
char pass[] = "12345678";
char auth[] = "1YtSwLUwVP2zfCvsHaFR7XKkt7CtjPbp";
int MaxLevel = 13;  // Tank bottom is 13 cm from sensor
```

---

## 🔌 Power Supply Considerations

### Voltage & Current Requirements
```
ESP32 Board        → 5V DC, 200mA
HC-SR04 Sensor     → 5V DC, 50mA
I2C LCD Display    → 5V DC, 100mA
4 LED Bulbs        → 5V DC, 80mA (total with resistors)
Relay Module       → 5V DC, 100mA
Total (without pump) → ~500mA at 5V
```

### Power Supply Setup
- Use **5V external power supply** (at least 1A capacity)
- Connect ESP32, LCD, Sensor, LEDs, and Relay to this 5V supply
- Use **separate AC/DC supply** for the water pump to avoid voltage drops
- **Important**: Connect GND from both supplies together for common reference

⚠️ **Warning**: Using single weak power supply can cause:
- Sensor reading errors
- LCD display corruption
- Relay malfunction
- ESP32 resets

---

## 🛠️ Troubleshooting Guide

| Issue | Cause | Solution |
|-------|-------|----------|
| LCD not displaying | Loose I2C wires or wrong address | Check connections. Address should be 0x27 or 0x3F |
| Sensor returns 0 | Trigger/Echo pins not connected | Verify GPIO 12 (TRIG) and GPIO 13 (ECHO) connections |
| LEDs not lighting | GPIO pins not set HIGH or wrong resistor | Check resistor value (180Ω). Verify GPIO 2,4,5,18 |
| Relay clicking but pump off | Relay active-HIGH issue | Swap LOW/HIGH in relay control code |
| WiFi connection fails | Wrong SSID/password or 5GHz network | Ensure 2.4GHz WiFi. ESP32 doesn't support 5GHz |
| Blynk not syncing | Invalid auth token or no internet | Verify token from Blynk app. Check internet connection |
| Display shows "ERROR" | LCD disconnected or I2C bus error | Check SDA/SCL wires. Power cycle the system |

---

## 📱 Blynk Mobile App Setup

### Steps to Create Blynk Project

1. **Download Blynk App**
   - Google Play: Search "Blynk IoT"
   - Apple App Store: Search "Blynk IoT"

2. **Create New Project**
   - Tap "Create New Project"
   - Select "Device Type" → **ESP32**
   - Select "Connection Type" → **WiFi**
   - Tap "Create"

3. **Copy Auth Token**
   - Copy the token shown on screen
   - Paste into `water_level_blynk.ino` in the `auth[]` variable

4. **Add Virtual Pins**
   - Add **Gauge** widget for V0 (water level)
   - Add **Button** widget for V1 (motor control)

5. **Run & Test**
   - Upload code to ESP32
   - Open Blynk app
   - Press "Play" button to start monitoring

---

## 🔧 Maintenance & Care

- **Weekly**: Check for water leaks around sensor area
- **Monthly**: Clean HC-SR04 sensor with soft cloth
- **Quarterly**: 
  - Verify all wire connections
  - Check relay contacts for corrosion
  - Inspect pump for debris
- **Yearly**: Update firmware if new features released

---

## 📄 Project File Structure

```
ESP32-Water-Level-Monitoring-System/
├── water_level_blynk.ino      # Main Arduino code
├── CIRCUIT_DIAGRAM.md          # Detailed pin connections
├── README.md                   # This documentation
└── circuit_diagram.png         # Circuit diagram image
```

---

## 🎯 How This Project Works

### System Flow

1. **Sensor Reading** (Every 1 second)
   - HC-SR04 measures distance to water surface
   - Calculates water level percentage
   - Updates LCD display

2. **LED Indication**
   - LEDs light up based on water level percentage
   - Provides visual feedback without looking at display

3. **Automatic Pump Control**
   - When water level is VERY LOW → Relay turns ON → Pump starts
   - When water level reaches FULL → Relay turns OFF → Pump stops
   - Prevents tank overflow and dry running

4. **IoT Monitoring**
   - Data sent to Blynk cloud every second
   - View water level from anywhere via mobile app
   - Get notifications when levels change

5. **Error Recovery**
   - LCD checks I2C connection every cycle
   - Auto-restarts display if connection lost
   - Scheduled refresh prevents display corruption

---

## 🌟 Key Innovations

✅ **Hardened I2C LCD handling** - Recovers from display errors automatically  
✅ **Dual address detection** - Works with both 0x27 and 0x3F LCD addresses  
✅ **Efficient sensor reading** - 1-second update interval with timeout protection  
✅ **Cloud integration** - Remote monitoring from anywhere  
✅ **Visual + Digital feedback** - 4 LEDs + LCD + Mobile app  
✅ **Automatic operation** - No manual pump control needed  

---

## ⚖️ License

This project is open source and available for educational and commercial use.

## 👨‍💻 Author

**Prantik Chanda Dhrubo**  
Email: prantik1719@student.nstu.edu.bd  
Repository: https://github.com/prantik1719-ai/ESP32-Water-Level-Monitoring-System

## 📞 Support & Feedback

For issues or questions:
1. Check the **Troubleshooting** section above
2. Verify all connections against **CIRCUIT_DIAGRAM.md**
3. Review official documentation:
   - Blynk: https://blynk.io/
   - Arduino: https://forum.arduino.cc/
   - ESP32: https://docs.espressif.com/

---

**Last Updated**: August 7, 2026  
**Project Version**: 1.0  
**Status**: ✅ Stable & Production Ready
