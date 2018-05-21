#include <SoftwareSerial.h>
#include <Nextion.h>

#define pinW     3 // PWM
#define pinR     5 // PWM
#define pinG     6 // PWM
#define pinB     9 // PWM
#define pinBuzz  10 // PWM
#define pinSafeL 11 // PWM
#define pinTX    12
#define pinRX    13

const int a = 440;
const int b = 466;
const int c = 261;
const int d = 294;
const int e = 329;
const int f = 349;
const int g = 391;

SoftwareSerial nextion(12, 13); // TX to pin 12 and RX to pin 13
Nextion lcd(nextion, 9600);

int  pS = 200;
int  pW = 150;
int  pR = 150;
int  pG = 150;
int  pB = 150;
long  tW = 4000;
long  tR = 1500;
long  tG = 1500;
long  tB = 1500;
bool sS = true;
bool sW = true;
bool sR = false;
bool sG = false;
bool sB = false;
bool sDeveloper    = false;
bool sStopper      = false;
bool sFixer        = false;
int  tDeveloper    = 90000;
int  tStopper      = 20000;
int  tFixer        = 90000;
unsigned long  tmrDeveloper = 0;
unsigned long  tmrStopper   = 0;
unsigned long  tmrFixer     = 0;
bool enlargerProcessing = false;

void setup() {
  //Serial.begin(9600);
  lcd.init();
  pinMode(pinSafeL, OUTPUT);
  pinMode(pinW, OUTPUT);
  pinMode(pinR, OUTPUT);
  pinMode(pinG, OUTPUT);
  pinMode(pinB, OUTPUT);
  beep(d);
  beep(d);
  beep(b);
  beep(c);
  beep(e);
  beep(a);
  beep(c);
  updateSettingsPage();
  analogWrite(pinW, 0);
  analogWrite(pinR, 0);
  analogWrite(pinG, 0);
  analogWrite(pinB, 0);
  updateSafeLight();

  //Serial.println("Started");
}



void loop() {
  unsigned long currentMillis = millis();
  if (sDeveloper) {
    if (currentMillis - tmrDeveloper >= (tDeveloper)) {
      lcd.setComponentValue("bDEVELOP", 0);
      beep(e); beep(d); beep(e); beep(d); beep(e); beep(d); beep(c); beep(b); beep(a);
      sDeveloper = false;
    }
  }

  if (sStopper) {
    if (currentMillis - tmrStopper >= (tStopper)) {
      lcd.setComponentValue("bSTOP", 0);
      beep(e); beep(d); beep(e); beep(d); beep(e); beep(d); beep(c); beep(b); beep(a);
      sStopper = false;
    }
  }

  if (sFixer) {
    if (currentMillis - tmrFixer >= (tFixer)) {
      lcd.setComponentValue("bFIX", 0);
      beep(e); beep(d); beep(e); beep(d); beep(e); beep(d); beep(c); beep(b); beep(a);
      sFixer = false;
    }
  }

  if (enlargerProcessing) {
    beep(c); beep(c); beep(e); beep(d); beep(c); beep(f); beep(g);
    lcd.sendCommand("dim=5");
    sS = false;
    updateSafeLight();
    if (sW) {
      analogWrite(pinW, pW);
      delay(tW);
      analogWrite(pinW, 0);
    }
    if (sR) {
      analogWrite(pinR, pR);
      delay(tR);
      analogWrite(pinR, 0);
    }
    if (sG) {
      analogWrite(pinG, pG);
      delay(tG);
      analogWrite(pinG, 0);
    }
    if (sB) {
      analogWrite(pinB, pB);
      delay(tB);
      analogWrite(pinB, 0);
    }
    enlargerProcessing = false;
    sS = true;
    updateSafeLight();
    lcd.sendCommand("dim=50");
    beep(e); beep(d); beep(e); beep(d); beep(e); beep(d); beep(c); beep(b); beep(a);
  }

  String message = lcd.listen(); // Message format example => 65 0 c 0 ffff ffff ffff
  if (message != "") {
    // Trim message for easy reading
    message.replace(" 0 ffff ffff ffff", "");
    message.replace("65 ", "");

    if (message == "0 c") { // Settings page clicked update it!
      updateSettingsPage();
    }
    if (message == "0 1") { // Safelight LED toggle
      sS = !sS;
      updateSafeLight();
    }
    if (message == "0 2") { // Increase safelight power
      pS = (pS < 250) ? pS + 10 : pS;
      updateSafeLight();
    }
    if (message == "0 3") { // Decrease safelight power
      pS = (pS > 10) ? pS - 10 : pS;
      updateSafeLight();
    }
    if (message == "1 1") { // Toggle White Led
      sW = !sW;
    }
    if (message == "1 6") { // Increase White led power
      pW = (pW < 250) ? pW + 10 : pW;
      lcd.setComponentText("page1.pW", String(pW));
    }
    if (message == "1 e") { // Decrease white led power
      pW = (pW > 10) ? pW - 10 : pW;
      lcd.setComponentText("page1.pW", String(pW));
    }
    if (message == "1 5") { // Increase white led exposure time
      tW = (tW < 30000) ? tW + 250 : tW;
      lcd.setComponentText("page1.tW", String(tW));
    }
    if (message == "1 d") { // Decrease white led exposure time
      tW = (tW > 250) ? tW - 250 : tW;
      lcd.setComponentText("page1.tW", String(tW));
    }
    if (message == "1 2") { // Toggle Red led
      sR = !sR;
    }
    if (message == "1 7") { // Increase red led power
      pR = (pR < 250) ? pR + 10 : pR;
      lcd.setComponentText("page1.pR", String(pR));
    }
    if (message == "1 10") { // Decrease red led power
      pR = (pR > 10) ? pR - 10 : pR;
      lcd.setComponentText("page1.pR", String(pR));
    }
    if (message == "1 8") { // Increase red led exposure time
      tR = (tR < 30000) ? tR + 250 : tR;
      lcd.setComponentText("page1.tR", String(tR));
    }
    if (message == "1 f") { // Decrease red led exposure time
      tR = (tR > 250) ? tR - 250 : tR;
      lcd.setComponentText("page1.tR", String(tR));
    }
    if (message == "1 3") { // Toggle Green led
      sG = !sG;
    }
    if (message == "1 9") { // Increase green led power
      pG = (pG < 250) ? pG + 10 : pG;
      lcd.setComponentText("page1.pG", String(pG));
    }
    if (message == "1 12") {  // Decrease green led power
      pG = (pG > 10) ? pG - 10 : pG;
      lcd.setComponentText("page1.pG", String(pG));
    }
    if (message == "1 a") {  // Increase green led exposure time
      tG = (tG < 30000) ? tG + 250 : tG;
      lcd.setComponentText("page1.tG", String(tG));
    }
    if (message == "1 11") { // Decrease green led exposure time
      tG = (tG > 250) ? tG - 250 : tG;
      lcd.setComponentText("page1.tG", String(tG));
    }
    if (message == "1 4") { // Toggle Blue led
      sB = !sB;
    }
    if (message == "1 b") { // Increase Blue led power
      pB = (pB < 250) ? pB + 10 : pB;
      lcd.setComponentText("page1.pB", String(pB));
    }
    if (message == "1 14") {  // Decrease Blue led power
      pB = (pB > 10) ? pB - 10 : pB;
      lcd.setComponentText("page1.pB", String(pB));
    }
    if (message == "1 c") {  // Increase Blue led exposure time
      tB = (tB < 30000) ? tB + 250 : tB;
      lcd.setComponentText("page1.tB", String(tB));
    }
    if (message == "1 13") { // Decrease Blue led exposure time
      tB = (tB > 250) ? tB - 250 : tB;
      lcd.setComponentText("page1.tB", String(tB));
    }
    if (message == "0 f") { // Start DEVELOPMENT BATH timer
      beep(f); beep(g); beep(e); beep(f); beep(g); beep(g); beep(c); beep(b); beep(d);
      sDeveloper = true;
      tmrDeveloper = millis();
    }
    if (message == "0 13") { // Increase DEVELOPMENT BATH timer
      tDeveloper = (tDeveloper < 180000) ? tDeveloper + 1000 : tDeveloper;
      lcd.setComponentText("tdeveloper", String(tDeveloper / 1000));
    }
    if (message == "0 12") {  // Decrease DEVELOPMENT BATH timer
      tDeveloper = (tDeveloper > 10000) ? tDeveloper - 1000 : tDeveloper;
      lcd.setComponentText("tdeveloper", String(tDeveloper / 1000));
    }
    if (message == "0 10") { // Start STOP BATH timer
      beep(f); beep(g); beep(e); beep(f); beep(g); beep(g); beep(c); beep(b); beep(d);
      sStopper = true;
      tmrStopper = millis();
    }
    if (message == "0 14") { //Increase STOP BATH timer
      tStopper = (tStopper < 60000) ? tStopper + 1000 :  tStopper;
      lcd.setComponentText("tstopper", String(tStopper / 1000));
    }
    if (message == "0 15") { // Decrease STOP BATH timer
      tStopper = (tStopper > 10000) ? tStopper - 1000 : tStopper;
      lcd.setComponentText("tstopper", String(tStopper / 1000));
    }
    if (message == "0 11") { // Start FIXING BATH timer
      beep(f); beep(g); beep(e); beep(f); beep(g); beep(g); beep(c); beep(b); beep(d);
      sFixer = true;
      tmrFixer = millis();
    }
    if (message == "0 16") { // Increase FIXING BATH timer
      tFixer = (tFixer < 180000) ? tFixer + 1000 : tFixer;
      lcd.setComponentText("tfixer", String(tFixer / 1000));
    }
    if (message == "0 17") { // Decrease FIXING BATH timer
      tFixer = (tFixer > 10000) ? tFixer - 1000 : tFixer;
      lcd.setComponentText("tfixer", String(tFixer / 1000));
    }
    if (message == "0 b") { // Turn on study mode
      analogWrite(pinW, 255);
      analogWrite(pinSafeL, 10);
    }
    if (message == "0 8") { // Start exposure
      enlargerProcessing = true;
    }
    if (message == "0 9") { // Stop exposure immediately
      analogWrite(pinW, 0);
      analogWrite(pinR, 0);
      analogWrite(pinG, 0);
      analogWrite(pinB, 0);
      enlargerProcessing = false;
      updateSafeLight();
    }
  }
}

void updateSettingsPage() {
  lcd.setComponentText("page1.pW", String(pW));
  lcd.setComponentText("page1.pR", String(pR));
  lcd.setComponentText("page1.pG", String(pG));
  lcd.setComponentText("page1.pB", String(pB));
  lcd.setComponentText("page1.tW", String(tW));
  lcd.setComponentText("page1.tR", String(tR));
  lcd.setComponentText("page1.tG", String(tG));
  lcd.setComponentText("page1.tB", String(tB));
  int toggleW = (sW) ? 1 : 0;
  int toggleR = (sR) ? 1 : 0;
  int toggleG = (sG) ? 1 : 0;
  int toggleB = (sB) ? 1 : 0;
  lcd.setComponentValue("page1.pW", toggleW);
  lcd.setComponentValue("page1.pR", toggleR);
  lcd.setComponentValue("page1.pG", toggleG);
  lcd.setComponentValue("page1.pB", toggleB);
}

void updateSafeLight() {
  if (sS) {
    analogWrite(pinSafeL, pS);
  } else {
    analogWrite(pinSafeL, 0);
  }
  lcd.setComponentText("tsafelight", String(pS));
}

void beep(int note) {
  tone(pinBuzz, note, 100);
  delay(100);
  noTone(pinBuzz);
  delay(30);
}

