/*
   Water level monitoring system - ESP32 + Blynk IoT
   Version with hardened I2C LCD handling.

   LCD fixes included:
     - explicit Wire.begin(21, 22) instead of relying on defaults
     - I2C bus slowed to 50 kHz for long breadboard jumpers
     - address auto-detected (0x27 or 0x3F) at startup
     - both rows held in buffers and redrawn whole, never partially
     - LCD presence checked every cycle; re-initialised if it drops off
     - scheduled silent re-init recovers 4-bit nibble desync
*/

// ---- These MUST come before the Blynk include ----
#define BLYNK_TEMPLATE_ID   "TMPL67139iqur"
#define BLYNK_TEMPLATE_NAME "Water level monitoring system"
#define BLYNK_PRINT Serial

#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#define LED1 2
#define LED2 4
#define LED3 5
#define LED4 18
#define trig 12
#define echo 13
#define relay 14

#define SDA_PIN 21
#define SCL_PIN 22

// Set to 0 to switch the scheduled refresh off if the blink bothers you
#define LCD_REFRESH_SECONDS 60

// Enter your tank max value (CM) - distance from sensor to empty tank bottom
int MaxLevel = 13;

int Level1 = (MaxLevel * 75) / 100;
int Level2 = (MaxLevel * 65) / 100;
int Level3 = (MaxLevel * 55) / 100;
int Level4 = (MaxLevel * 35) / 100;

uint8_t lcdAddress = 0x27;                 // overwritten by the scan in setup()
LiquidCrystal_I2C lcd(0x27, 16, 2);

BlynkTimer timer;

char auth[] = "1YtSwLUwVP2zfCvsHaFR7XKkt7CtjPbp";

// Phone hotspot must be set to 2.4 GHz - ESP32 cannot see 5 GHz
char ssid[] = "POCO";
char pass[] = "12345678";

// Both rows kept here so we can always redraw the full screen
char lineTop[17]    = "                ";
char lineBottom[17] = "                ";

unsigned long lastRefresh = 0;
bool lcdReady = false;

// ---------------------------------------------------------------- LCD helpers

// Returns true if a device acknowledges at the given address
bool i2cPresent(uint8_t addr) {
  Wire.beginTransmission(addr);
  return (Wire.endTransmission() == 0);
}

// Finds the LCD backpack. Returns 0 if nothing answers.
uint8_t findLcd() {
  if (i2cPresent(0x27)) return 0x27;
  if (i2cPresent(0x3F)) return 0x3F;
  for (uint8_t a = 0x20; a <= 0x27; a++) if (i2cPresent(a)) return a;
  for (uint8_t a = 0x38; a <= 0x3F; a++) if (i2cPresent(a)) return a;
  return 0;
}

// Writes a padded 16-character string into a row buffer
void setLine(char *buf, const char *text) {
  int i = 0;
  while (text[i] != '\0' && i < 16) { buf[i] = text[i]; i++; }
  while (i < 16) { buf[i] = ' '; i++; }
  buf[16] = '\0';
}

// Pushes both buffers to the display in full
void drawLcd() {
  if (!lcdReady) return;
  lcd.setCursor(0, 0);
  lcd.print(lineTop);
  lcd.setCursor(0, 1);
  lcd.print(lineBottom);
}

// Brings the LCD back from a corrupted or disconnected state
void initLcd() {
  lcd.init();
  delay(60);                 // HD44780 needs settling time after init
  lcd.backlight();
  lcd.clear();
  delay(10);
  lcdReady = true;
  drawLcd();
  lastRefresh = millis();
}

// Called once per second alongside the sensor read
void serviceLcd() {
  if (!i2cPresent(lcdAddress)) {
    // Backpack stopped answering - loose jumper or a power dip
    lcdReady = false;
    Serial.println("LCD not responding on I2C");
    uint8_t found = findLcd();
    if (found) { lcdAddress = found; lcd = LiquidCrystal_I2C(found, 16, 2); initLcd(); }
    return;
  }
  if (!lcdReady) { initLcd(); return; }

  // Scheduled re-init: the only reliable cure for 4-bit nibble desync
  if (LCD_REFRESH_SECONDS > 0 &&
      millis() - lastRefresh > (unsigned long)LCD_REFRESH_SECONDS * 1000UL) {
    initLcd();
  }
}

// ---------------------------------------------------------------- setup

void setup() {
  Serial.begin(115200);
  delay(300);

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(relay, OUTPUT);
  digitalWrite(relay, HIGH);
  digitalWrite(trig, LOW);

  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(50000);            // 50 kHz - tolerant of long jumper wires

  uint8_t found = findLcd();
  if (found == 0) {
    Serial.println("No I2C device found. Check SDA=21, SCL=22, VCC on VIN (5V), GND.");
  } else {
    Serial.print("LCD found at 0x");
    Serial.println(found, HEX);
    lcdAddress = found;
    lcd = LiquidCrystal_I2C(found, 16, 2);
    initLcd();
  }

  setLine(lineTop,    "System");
  setLine(lineBottom, "    Loading..");
  drawLcd();

  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);

  setLine(lineTop,    "");
  setLine(lineBottom, "Motor is OFF");
  drawLcd();

  timer.setInterval(1000L, ultrasonic);
}

// ---------------------------------------------------------------- sensor

void ultrasonic() {
  serviceLcd();

  digitalWrite(trig, LOW);
  delayMicroseconds(4);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  long t = pulseIn(echo, HIGH, 30000UL);
  if (t == 0) {
    Serial.println("No echo - check sensor wiring");
    return;
  }

  int distance = t / 29 / 2;
  Serial.println(distance);

  int blynkDistance = MaxLevel - distance;
  if (blynkDistance < 0) blynkDistance = 0;
  Blynk.virtualWrite(V0, blynkDistance);

  char row[17];
  if (distance >= Level1) {
    strcpy(row, "WLevel:Very Low");
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
    digitalWrite(LED4, LOW);
  } else if (distance >= Level2) {
    strcpy(row, "WLevel:Low");
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, HIGH);
    digitalWrite(LED3, LOW);
    digitalWrite(LED4, LOW);
  } else if (distance >= Level3) {
    strcpy(row, "WLevel:Medium");
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, HIGH);
    digitalWrite(LED3, HIGH);
    digitalWrite(LED4, LOW);
  } else {
    strcpy(row, "WLevel:Full");
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, HIGH);
    digitalWrite(LED3, HIGH);
    digitalWrite(LED4, HIGH);
  }

  setLine(lineTop, row);
  drawLcd();
}

// ---------------------------------------------------------------- Blynk

BLYNK_WRITE(V1) {
  bool Relay = param.asInt();
  if (Relay == 1) {
    digitalWrite(relay, LOW);      // swap LOW/HIGH if your relay board is active-HIGH
    setLine(lineBottom, "Motor is ON");
  } else {
    digitalWrite(relay, HIGH);
    setLine(lineBottom, "Motor is OFF");
  }
  drawLcd();
}

BLYNK_CONNECTED() {
  Blynk.syncVirtual(V1);
}

void loop() {
  Blynk.run();
  timer.run();
}
