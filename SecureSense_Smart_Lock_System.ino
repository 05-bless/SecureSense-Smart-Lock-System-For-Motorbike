#include <SPI.h>
  
#include <MFRC522.h>
  
#include <Wire.h>
  
#include <LiquidCrystal_I2C.h>
  
#include <MPU6050.h>
  

  
// ---------- RFID ----------
  
#define SS_PIN   5
  
#define RST_PIN  21
  
MFRC522 rfid(SS_PIN, RST_PIN);
  

  
// ---------- OUTPUT ----------
  
#define RELAY_PIN   22
  
#define BUZZER_PIN  4
  

  
// ---------- LCD ----------
  
LiquidCrystal_I2C lcd(0x27,16,2);
  

  
// ---------- MPU ----------
  
MPU6050 mpu;
  

  
// ---------- UID ----------
  
byte allowedUID[4] = {0x79, 0xF4, 0x95, 0x04};
  

  
// ---------- SETTINGS ----------
  
int threshold = 17000;
  
unsigned long lastTamper = 0;
  

  
void setup() {
  

  
  Serial.begin(115200);
  

  
  pinMode(RELAY_PIN, OUTPUT);
  
  pinMode(BUZZER_PIN, OUTPUT);
  

  
  digitalWrite(RELAY_PIN, HIGH);   // locked
  
  digitalWrite(BUZZER_PIN, LOW);
  

  
  // RFID
  
  SPI.begin();
  
  rfid.PCD_Init();
  

  
  // LCD
  
  Wire.begin(25,26);
  
  lcd.init();
  
  lcd.backlight();
  

  
  lcd.setCursor(0,0);
  
  lcd.print("SecureSense");
  
  lcd.setCursor(0,1);
  
  lcd.print("Starting...");
  
  delay(2000);
  

  
  // MPU
  
  Wire.begin(13,14);
  
  mpu.initialize();
  

  
  showReady();
  

  
  Serial.println("SYSTEM READY");
  
}
  

  
void loop() {
  

  
  // ---------- RFID ----------
  
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
  

  
    if (authorized()) {
  
      accessGranted();
  
    } else {
  
      accessDenied();
  
    }
  

  
    rfid.PICC_HaltA();
  
  }
  

  
  // ---------- TAMPER ----------
  
  int16_t ax, ay, az;
  
  mpu.getAcceleration(&ax, &ay, &az);
  

  
  if ((abs(ax) > threshold || abs(ay) > threshold) &&
  
      millis() - lastTamper > 3000) {
  

  
    tamperAlert();
  
    lastTamper = millis();
  
  }
  

  
  delay(100);
  
}
  

  
// =====================================
  
// FUNCTIONS
  
// =====================================
  

  
bool authorized() {
  
  for(int i=0;i<4;i++) {
  
    if(rfid.uid.uidByte[i] != allowedUID[i]) return false;
  
  }
  
  return true;
  
}
  

  
void accessGranted() {
  

  
  Serial.println("ACCESS GRANTED");
  

  
  lcd.clear();
  
  lcd.setCursor(0,0);
  
  lcd.print("ACCESS GRANTED");
  
  lcd.setCursor(0,1);
  
  lcd.print("BIKE ENABLED");
  

  
  goodBeep();
  

  
  digitalWrite(RELAY_PIN, LOW);   // enable bike
  
  delay(5000);
  
  digitalWrite(RELAY_PIN, HIGH);  // disable again
  

  
  lcd.clear();
  
  lcd.setCursor(0,0);
  
  lcd.print("SYSTEM LOCKED");
  

  
  delay(1500);
  
  showReady();
  
}
  

  
void accessDenied() {
  

  
  Serial.println("ACCESS DENIED");
  

  
  lcd.clear();
  
  lcd.setCursor(0,0);
  
  lcd.print("ACCESS DENIED");
  
  lcd.setCursor(0,1);
  
  lcd.print("UNAUTHORIZED");
  

  
  badBeep();
  

  
  delay(2000);
  
  showReady();
  
}
  

  
void tamperAlert() {
  

  
  Serial.println("TAMPER ALERT");
  

  
  lcd.clear();
  
  lcd.setCursor(0,0);
  
  lcd.print("WARNING!");
  
  lcd.setCursor(0,1);
  
  lcd.print("BIKE MOVED");
  

  
  for(int i=0;i<5;i++) {
  
    digitalWrite(BUZZER_PIN,HIGH);
  
    delay(120);
  
    digitalWrite(BUZZER_PIN,LOW);
  
    delay(120);
  
  }
  

  
  delay(1500);
  
  showReady();
  
}
  

  
void showReady() {
  
  lcd.clear();
  
  lcd.setCursor(0,0);
  
  lcd.print("SCAN RFID TAG");
  
  lcd.setCursor(0,1);
  
  lcd.print("LOCK ACTIVE");
  
}
  

  
void goodBeep() {
  
  digitalWrite(BUZZER_PIN,HIGH);
  
  delay(150);
  
  digitalWrite(BUZZER_PIN,LOW);
  
}
  

  
void badBeep() {
  
  for(int i=0;i<3;i++) {
  
    digitalWrite(BUZZER_PIN,HIGH);
  
    delay(200);
  
    digitalWrite(BUZZER_PIN,LOW);
  
    delay(150);
  
  }
  
}