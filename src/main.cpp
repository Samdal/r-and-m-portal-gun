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
#define toplight 9

#define shootsound 1
#define tick 2
#define shutdown 3
#define startup 4
#define fadedelay 13
#define fademin 160

bool prevA = false;
bool prefs = false;
bool shutoff = true;
bool discnext = true;
uint8_t fade = fademin;
bool fadeup = true;
const unsigned long shootDelay = 4000;
unsigned long prevShot = 0;
unsigned long prevFade = 0;
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
  pinMode(toplight, OUTPUT);

  // reset display
  display.setBrightness(0x0f);
  uint8_t c[4] = {0};
  display.setSegments(c);

  mySerial.begin(9600);
  myMP3.begin(mySerial);
  myMP3.volume(20);
  delay(20);
}

void loop() {
  bool shoot = !digitalRead(button);
  unsigned long t = millis();
  if (shutoff) {
    if (shoot && t - shootDelay >= prevShot) {
      prevShot = t;
      while (!digitalRead(button)) {
        if (millis() - 1000 >= prevShot) {
          myMP3.play(startup);
          shutoff = false;
          delay(400);
          uint8_t c[1] = {worlds[0]};
          display.showNumberDec(count, true);
          display.setSegments(c, 1, 0);
          digitalWrite(lights, HIGH);
          delay(50);
          digitalWrite(lights, LOW);
          delay(100);
          digitalWrite(lights, HIGH);
          analogWrite(toplight, fade);
          delay(50);
          digitalWrite(lights, LOW);
          delay(500);
          break;
        }
      }
    }
    return;
  }

  dofade(t);

  if (shoot && t - shootDelay >= prevShot) {
    prevShot = t;
    while (!digitalRead(button)) {
      if (millis() - 1000 >= prevShot) {
        myMP3.play(shutdown);
        shutoff = true;
        delay(400);
        uint8_t c[4] = {0};
        display.setSegments(c);
        digitalWrite(lights, HIGH);
        delay(50);
        digitalWrite(lights, LOW);
        digitalWrite(toplight, LOW);
        delay(800);
        return;
      }
    }
    fade = 255;
    analogWrite(toplight, fade);
    digitalWrite(lights, HIGH);
    myMP3.play(shootsound);
    delay(2000);
    digitalWrite(lights, LOW);
  }

  bool currA = digitalRead(outputA);
  delay(1);
  if (currA != prevA) {
    if (discnext) {
      discnext = false;
      prevA = currA;
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
    prevA = currA;
    myMP3.play(tick);
    delay(40);
    myMP3.play(tick);
    display.showNumberDec(count, true);
    display.setSegments(world, 1, 0);
  }
}

void dofade(unsigned long t) {
  if (t - fadedelay >= prevFade) {
    if (fadeup) {
      fade++;
      if (fade == 255)
        fadeup = false;
      analogWrite(toplight, fade);
    } else {
      fade--;
      if (fade == fademin)
        fadeup = true;
      analogWrite(toplight, fade);
    }
    prevFade = t;
  }
  if (random(500) == 21)
    digitalWrite(toplight, LOW);
}
