#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Keypad_I2C.h>

#define BLYNK_TEMPLATE_ID "TMPL63fd4U5c4"
#define BLYNK_TEMPLATE_NAME "kel 1"
#define BLYNK_AUTH_TOKEN "KXq7_5QsxbfkKzZLpX-kjzt3hvnWAQFd"

#define LDR_PIN 33
#define SERVO_PIN 15
#define LCD_I2C_ADDR 0x27
#define KEYPAD_I2C_ADDR 0x20
#define LCD_ROWS 16
#define LCD_COLS 2
#define KEYPAD_ROWS 4
#define KEYPAD_COLS 4

String enterPassword = "";
LiquidCrystal_I2C lcd(LCD_I2C_ADDR, LCD_ROWS, LCD_COLS);
char keys[KEYPAD_ROWS][KEYPAD_COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[KEYPAD_ROWS] = {0, 1, 2, 3};  
byte colPins[KEYPAD_COLS] = {4, 5, 6, 7}; 

Keypad_I2C keyPad(makeKeymap(keys), rowPins, colPins, KEYPAD_ROWS, KEYPAD_COLS, KEYPAD_I2C_ADDR, PCF8574);
const String correctPassword = "123A";

char ssid[] = "Galaxy A02442e";
char pass[] = "aaaaaaaa";

int ledPin[] = {13, 12, 14};
int numLed = 3;
int ldrValue;
Servo servo;

long lastKeypadTime = 0;  
BLYNK_WRITE(V0) {
  int pin = param.asInt();
  digitalWrite(ledPin[0], pin);
}

BLYNK_WRITE(V1) {
  int pin = param.asInt();
  digitalWrite(ledPin[1], pin);
}

BLYNK_WRITE(V2) {
  int pin = param.asInt();
  digitalWrite(ledPin[2], pin);
}

BLYNK_WRITE(V3) {
  if (param.asInt() == 1) {
    servo.write(180);
  } else {
    servo.write(0);
  }
}

void setup() {
  Serial.begin(115200);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  
  for (int i = 0; i < numLed; i++) {
    pinMode(ledPin[i], OUTPUT);
  }
  servo.attach(SERVO_PIN);

  lcd.init();
  lcd.setCursor(0, 0);
  lcd.print("Type Passkey");
  lcd.setCursor(0, 1);
  keyPad.begin();
}

void loop() {
  Blynk.run();
  turnOnLivingRoomIfDark();
  checkKeypadInput();
}

void turnOnLivingRoomIfDark() {
  ldrValue = analogRead(LDR_PIN);
  Serial.println(ldrValue);

  if (ldrValue > 2000) {
    digitalWrite(ledPin[2], HIGH);
    Blynk.virtualWrite(V2, 1);
  } else {
    digitalWrite(ledPin[2], LOW);
    Blynk.virtualWrite(V2, 0);
  }
}

void checkKeypadInput() {
  char ch = keyPad.getKey();
  if (ch) {
    Serial.println(ch);
    enterPassword += ch;
    
    lcd.backlight();
    lcd.setCursor(enterPassword.length() - 1, 1);  
    lcd.print("*");
    lastKeypadTime = millis();
    
    if (ch == '#') {
      enterPassword = "";
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Type Passkey:");
      lcd.setCursor(0, 1);
    }
    
    if (enterPassword == correctPassword) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("  CORRECT");
      servo.write(180);
      Blynk.virtualWrite(V3, 1);
      delay(2000);  
      enterPassword = "";  
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Type Passkey:");
      lcd.setCursor(0, 1);
    }
  }
  
  if (millis() - lastKeypadTime > 5000 && !enterPassword.isEmpty()) {
    enterPassword = "";
    lcd.noBacklight();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Type Passkey:");
    lcd.setCursor(0, 1);
    servo.write(0);
    Blynk.virtualWrite(V3, 0);
  }
}
