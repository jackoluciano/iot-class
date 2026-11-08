#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "frames.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const uint8_t* frames[] = {
  frame1,
  frame2,
  frame3,
  frame4,
  frame5,
  frame6
};

const int totalFrames = 6;
int currentFrame = 0;

void setup() {

  Serial.begin(115200);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 gagal");
    while(true);
  }

  display.clearDisplay();
}

void loop() {

  display.clearDisplay();

  display.drawBitmap(
    0,
    0,
    frames[currentFrame],
    128,
    64,
    WHITE
  );

  display.display();

  currentFrame++;

  if(currentFrame >= totalFrames) {
    currentFrame = 0;
  }

  delay(120);
}