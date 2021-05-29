#include <Arduino.h>
#include <DFPlayerMini_Fast.h>
#include <SoftwareSerial.h>
#include <TM1637Display.h>

#define CLK 2
#define DIO 3
#define outputA 7
#define outputB 6
#define button A5
#define lights A1

bool prevA = false;
bool prefs = false;
bool discnext = true;
unsigned long shootDelay = 3000;
unsigned long prevShot = 0;
uint8_t count = 137;

// AbCdEFGHILJSoPunytr
const uint8_t worlds[19] = {
    SEG_DP | SEG_F | SEG_A | SEG_E | SEG_D,                 // C
    SEG_DP | SEG_A | SEG_F | SEG_B | SEG_E | SEG_C | SEG_G, // A
    SEG_DP | SEG_F | SEG_E | SEG_D | SEG_C | SEG_G,         // b
    SEG_DP | SEG_A | SEG_F | SEG_G | SEG_E,                 // F
    SEG_DP | SEG_F | SEG_E | SEG_B | SEG_E | SEG_C | SEG_G, // H
    SEG_DP | SEG_E | SEG_F,                                 // I
    SEG_DP | SEG_F | SEG_E | SEG_D | SEG_G,                 // t
    SEG_DP | SEG_A | SEG_F | SEG_G | SEG_C | SEG_D,         // S
    SEG_DP | SEG_D | SEG_A | SEG_C | SEG_E | SEG_F,         // G
    SEG_DP | SEG_E | SEG_G,                                 // r
    SEG_DP | SEG_F | SEG_C | SEG_G | SEG_B | SEG_D,         // y
    SEG_DP | SEG_B | SEG_F | SEG_E | SEG_C | SEG_D,         // U
    SEG_DP | SEG_D | SEG_B | SEG_C | SEG_E,                 // J
    SEG_DP | SEG_D | SEG_E | SEG_F,                         // L
    SEG_DP | SEG_E | SEG_F | SEG_A | SEG_B | SEG_G,         // P
    SEG_DP | SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,         // d
    SEG_DP | SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F, // O
    SEG_DP | SEG_C | SEG_E | SEG_G,                         // n
    SEG_DP | SEG_A | SEG_D | SEG_E | SEG_F | SEG_G          // E
};

TM1637Display display(CLK, DIO);
SoftwareSerial mySerial(10, 11); // RX, TX
DFPlayerMini_Fast myMP3;

void setup() {
  pinMode(outputA, INPUT);
  pinMode(outputB, INPUT);
  pinMode(button, INPUT);
  pinMode(lights, OUTPUT);

  mySerial.begin(9600);
  myMP3.begin(mySerial);
  myMP3.volume(20);
  delay(20);
  myMP3.wakeUp();
  delay(20);

  display.setBrightness(0x0f);
  display.showNumberDec(count, true);
  uint8_t c[1] = {worlds[0]};
  display.setSegments(c, 1, 0);
}

void loop() {
  bool shoot = !digitalRead(button);
  unsigned long t = millis();
  if (shoot && t - shootDelay >= prevShot) {
    myMP3.playNext();
    digitalWrite(lights, HIGH);
    delay(2000);
    digitalWrite(lights, LOW);
    prevShot = t;
  }

  bool currA = digitalRead(outputA);
  delay(1);
  if (currA != prevA) {
    prevA = currA;
    if (discnext) {
      discnext = false;
      return;
    }
    discnext = true;
    if (digitalRead(outputB) != currA) {
      count += random(9, 31);
    } else {
      count -= random(9, 31);
    }
    uint8_t world[1] = {0};
    if (count > 131 && count < 146) {
      count = 137;
      world[0] = worlds[0];
    } else {
      world[0] = worlds[random(18)];
    }
    display.showNumberDec(count, true);
    display.setSegments(world, 1, 0);
  }
}
