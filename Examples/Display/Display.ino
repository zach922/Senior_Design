/*
  The top-level .ino file for the OBD-II Red Team Senior Design project.
  Developers: Zach Schmitz, Dillon Mahr, Daniel Folden , & Trevor Washinsky
*/

// Display Libraries
#include<SPI.h>
#include<FlexCAN_T4.h>
#include"Adafruit_GFX.h"
#include"Adafruit_RA8875.h"

// Our Libraries

//#include"Interpret.h"

// Display definitions
#define WIDTH 480
#define HEIGHT 272
#define RA8875_INT 3
#define RA8875_CS 10
#define RA8875_RESET 9

typedef struct   {
  int vspd; //vehicle speed
  int espd; //engine speed
  int ctmp; //coolant temp
  int otmp; //oil temp
  int flvl; //fuel level
  int volt; //battery Voltage 
} CANDATA;


CANDATA cdat;

// convert int to string
char* toString(const int num){
    char str[8];
    sprintf(str, "%d", num);

    return str;
}



Adafruit_RA8875 tft = Adafruit_RA8875(RA8875_CS, RA8875_RESET);

void displayDiagnostic(){
  tft.fillScreen(RA8875_BLACK);
  tft.textMode();
  tft.textTransparent(RA8875_WHITE);
  //tft.textSetCurser(left/right, up/down);
  
  tft.textSetCursor(140,106);
  tft.textEnlarge(8);
  char str[8];
  sprintf(str, "%d", cdat.vspd);
  tft.textWrite(str);
  tft.textSetCursor(260,106);
  tft.textWrite("MPH");

  
  tft.textSetCursor(150, 170);
  tft.textEnlarge(2);
  sprintf(str, "%d", cdat.espd);
  tft.textWrite(str);
  tft.textSetCursor(260,170);
  tft.textWrite("RPM");
  

 tft.textSetCursor(1,30);
  tft.textEnlarge(1);
 sprintf(str, "%d", cdat.otmp);
  tft.textWrite(str);
  tft.textSetCursor(1,1);
  tft.textWrite("OIL TEMP");


  tft.textSetCursor(190,30);
  tft.textEnlarge(1);
  sprintf(str, "%d", cdat.ctmp);
  tft.textWrite(str);
  tft.textSetCursor(160,1);
  tft.textWrite("Coolant TEMP");


  tft.textSetCursor(400,230);
  tft.textEnlarge(1);
  sprintf(str, "%d", cdat.flvl);
  tft.textWrite(str);
  tft.textSetCursor(400,200);
  tft.textWrite("Fuel");



  tft.textSetCursor(1,230);
  tft.textEnlarge(1);
  sprintf(str, "%d", cdat.volt);
  tft.textWrite(str);
  tft.textSetCursor(1,200);
  tft.textWrite("Voltage");



  tft.textSetCursor(1,230);
  tft.textEnlarge(1);
  sprintf(str, "%d", cdat.volt);
  tft.textWrite(str);
  tft.textSetCursor(1,200);
  tft.textWrite("Voltage");

  if (cdat.ctmp>200 || cdat.otmp>200 )
  {
    tft.textSetCursor(1,100);
    tft.textEnlarge(.95);
    //tft.textWrite(str);
    tft.textWrite("CHECK ENGINE!");
    
    }
   return 0;
}


void setup() {
  Serial.begin(9600);
  Serial.println("CAN Start");
 
char str[15] = "";

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
//  int velocity = 60;
//
//  char str[8];
//  sprintf(str, "%d", velocity);
//  
  // print text on display
  /*
  tft.textMode();
  tft.textSetCursor(200,200);
  tft.textTransparent(RA8875_WHITE);

//tft.getCursorX();
//tft.getCursorY();

//tft.setTextColor(TFT_WHITE,TFT_BLACK); 
//tft.setTextSize(1);

  tft.textEnlarge(1);
  tft.textWrite(str);

  //tft.textSetCursor(170,150);
  //tft.textEnlarge(8);
  //tft.textWrite("MPH");

  
 
tft.textSetCursor(200,200);
tft.textEnlarge(2);
  tft.textWrite("RPMs");

   
tft.textSetCursor(1,1);
tft.textEnlarge(1);
tft.textWrite("OIL PRESSURE:");
tft.textSetCursor(1,25);
tft.textEnlarge(2);
tft.textWrite("200");

 
  
  pinMode(RA8875_INT, INPUT);
  digitalWrite(RA8875_INT, HIGH);
*/
}

void loop() {

  //request();

  displayDiagnostic();

  delay(250);
  cdat.vspd = 70;
  cdat.espd = 1000;
  cdat.otmp = 1;
  cdat.ctmp=100;
  cdat.flvl=75;
  cdat.volt=12;
}
