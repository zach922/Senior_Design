/*
  The top-level .ino file for the OBD-II Red Team Senior Design project.

  Developers: Zach Schmitz, Dillon Mahr, Daniel Folden , & Trevor Washinsky
*/

// Display Libraries
#include<SPI.h>
#include"Adafruit_GFX.h"
#include"Adafruit_RA8875.h"

// Our Libraries
#include"Interpret.h"
#include"Display.h"

// Display definitions
#define RA8875_INT 3
#define RA8875_CS 10
#define RA8875_RESET 9

Adafruit_RA8875 tft = Adafruit_RA8875(RA8875_CS, RA8875_RESET);

void setup() {
  Serial.begin(9600);
  Serial.println("CAN Start");

  char string[15] = "Hello, World! ";

  // Initialize the display using RA8875_480x80
  if (!tft.begin(RA8875_480x272)) {
    Serial.println("RA8875 Not Found!");
    while (1);
  }

  Serial.println("Found RA8875");

  // initialize Screen
  tft.displayOn(true);
  tft.GPIOX(true);      // Enable TFT - display enable tied to GPIOX
  tft.PWM1config(true, RA8875_PWM_CLK_DIV1024); // PWM output for backlight
  tft.PWM1out(255);
  tft.fillScreen(RA8875_BLACK);

  // convert int to string
  int velocity = 60;

  char str[8];
  sprintf(str, "%d", velocity);
  
  // print text on display
  tft.textMode();
  tft.textSetCursor(200,200);
  tft.textTransparent(RA8875_WHITE);

  tft.textEnlarge(1);
  tft.textWrite(str);

  pinMode(RA8875_INT, INPUT);
  digitalWrite(RA8875_INT, HIGH);

}

void loop() {


}
