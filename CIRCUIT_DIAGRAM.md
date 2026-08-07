```
# Circuit Diagram - ESP32 Water Level Monitoring System

## Components Used:
- **ESP32 Microcontroller Board**
- **HC-SR04 Ultrasonic Sensor** (Water level detection)
- **16x2 I2C LCD Display** (Status display)
- **4x LED Bulbs** (Red, Yellow, Yellow, Red - Water level indicators)
- **4x 180Ω Resistors** (LED current limiting)
- **Relay Module** (5V coil, for pump control)
- **AC/DC Water Pump** (Controlled via relay)
- **5VDC External Power Supply**

## Pin Connections:

### ESP32 GPIO Pins:
- **GPIO 12** → HC-SR04 Trigger Pin
- **GPIO 13** → HC-SR04 Echo Pin
- **GPIO 2** → LED1 (Red)
- **GPIO 4** → LED2 (Yellow)
- **GPIO 5** → LED3 (Yellow)
- **GPIO 18** → LED4 (Red)
- **GPIO 14** → Relay Module Signal Pin
- **GPIO 21** → I2C SDA (LCD)
- **GPIO 22** → I2C SCL (LCD)

### I2C LCD Module:
- **VCC** → 5V
- **GND** → GND
- **SDA** → GPIO 21
- **SCL** → GPIO 22

### HC-SR04 Sensor:
- **VCC** → 5V
- **GND** → GND
- **TRIG** → GPIO 12
- **ECHO** → GPIO 13

### LED Indicators:
- **LED1 (GPIO 2)** → Red LED with 180Ω resistor
- **LED2 (GPIO 4)** → Yellow LED with 180Ω resistor
- **LED3 (GPIO 5)** → Yellow LED with 180Ω resistor
- **LED4 (GPIO 18)** → Red LED with 180Ω resistor

### Relay Module:
- **Signal Pin** → GPIO 14
- **VCC** → 5V
- **GND** → GND
- **Common (COM)** → AC/DC Supply voltage
- **Normally Open (NO)** → Water Pump positive terminal
- **GND** → Water Pump negative terminal

## Power Supply:
- **5VDC External Power** for ESP32, LCD, LEDs, Relay Module, and Sensor
- **AC/DC Supply** for Water Pump (controlled via relay)

## Water Level Stages (LEDs):
1. **Very Low** → LED1 ON (Red)
2. **Low** → LED1 + LED2 ON
3. **Medium** → LED1 + LED2 + LED3 ON
4. **Full** → All 4 LEDs ON
```
