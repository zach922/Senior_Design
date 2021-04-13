/*
  The top-level .ino file for the OBD-II Red Team Senior Design project.

  Developers: Zach Schmitz, Dillon Mahr, Daniel Folden, & Trevor Washinsky
*/

// Display Libraries
#include<SPI.h>
#include"Adafruit_GFX.h"
#include"Adafruit_RA8875.h"

// CAN Libraries
#include <FlexCAN_T4.h>

//CAN definitions
#define VEHICLE_SPEED 0x0D
#define ENGINE_RPM 0x0C
#define COOLANT_TEMP 0x05
#define OIL_TEMP 0x5C
#define FUEL_LEVEL 0x2F
#define STBY  2

typedef struct   {
  int vspd; //vehicle speed
  int espd; //engine speed
  int ctmp; //coolant temp
  int otmp; //oil temp
  int flvl; //fuel level
} CANDATA;

//Display definitions
#define RA8875_INT 3
#define RA8875_CS 10
#define RA8875_RESET 9

// Initialize Screen and CAN transiever 
Adafruit_RA8875 tft = Adafruit_RA8875(RA8875_CS, RA8875_RESET);
FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> Can0;

CANDATA cdat;

//Interpret CAN Data into usable diagnostic data
void processData(const uint8_t pid, const uint8_t buf[8]){

    if (pid == VEHICLE_SPEED){
        cdat.vspd = buf[3];
    }
    else if (pid == ENGINE_RPM){
        cdat.espd = ((256*buf[3]+buf[4])/4);
    }
    else if (pid == COOLANT_TEMP){
        cdat.ctmp = (buf[3]-40);
    }
    else if (pid == OIL_TEMP){
        cdat.otmp = (buf[3]-40);
    }
    else if(pid == FUEL_LEVEL){
        cdat.flvl = ((100*buf[3])/254);
    }
}

//Interrupt to process recieved CAN message
void canSniff(const CAN_message_t &msg) {
  
  //uint8_t pdat = 
  processData(msg.buf[2], msg.buf);

  printDiagnostic();
}

//Print entire CANDATA struct info
void printDiagnostic(){
  Serial.print(" cdat: "); 
  Serial.print(cdat.vspd); 
  Serial.print(", "); 
  Serial.print(cdat.espd);
  Serial.print(", "); 
  Serial.print(cdat.ctmp); 
  Serial.print(", "); 
  Serial.print(cdat.otmp);
  Serial.print(", "); 
  Serial.print(cdat.flvl); 
  Serial.println();
}

//request to update a specific diagnostic PID
void requestDiagnostic(uint8_t pid){
  CAN_message_t msg;
  msg.id = 0x7DF;
  msg.buf[0] = 0x02;
  msg.buf[1] = 0x01;
  msg.buf[2] = pid;
  msg.buf[3] = 0x00;
  msg.buf[4] = 0x00;
  msg.buf[5] = 0x00;
  msg.buf[6] = 0x00;
  msg.buf[7] = 0x00;


  static uint32_t timeout = millis();
  if (timeout - millis() > 200){
    Can0.write(msg);
    timeout = millis();
  }
}

//Request for all diagnostics to be updated
void request(){
  requestDiagnostic(VEHICLE_SPEED);
  requestDiagnostic(ENGINE_RPM);
  requestDiagnostic(COOLANT_TEMP);
  requestDiagnostic(OIL_TEMP);
  requestDiagnostic(FUEL_LEVEL);
}

// convert int to string
char* toString(const int num){
    char str[8];
    sprintf(str, "%d", num);

    return str;
}

void setup() {

  // CAN Config
  Serial.begin(115200); delay(400);
  pinMode(STBY, OUTPUT); digitalWrite(STBY, LOW); /* optional tranceiver enable pin */
  Can0.begin();
  Can0.setBaudRate(500000);
  Can0.setMaxMB(16);
  Can0.enableFIFO();
  Can0.enableFIFOInterrupt();
  Can0.onReceive(canSniff);
  Can0.mailboxStatus();

  // Display Config
  //Serial.begin(9600);
  //Serial.println("CAN Start");
  
  // Initialize the display using RA8875_480x272
//  if (!tft.begin(RA8875_480x272)) {
//    Serial.println("RA8875 Not Found!");
//    while (1);
//  }
//
//  Serial.println("Found RA8875");
//
//  // initialize Screen
//  tft.displayOn(true);
//  tft.GPIOX(true);      // Enable TFT - display enable tied to GPIOX
//  tft.PWM1config(true, RA8875_PWM_CLK_DIV1024); // PWM output for backlight
//  tft.PWM1out(255);
//  tft.fillScreen(RA8875_BLACK);


  char* str = toString(60);

  // print text on display
//  tft.textMode();
//  tft.textSetCursor(200,200);
//  tft.textTransparent(RA8875_WHITE);
//
//  tft.textEnlarge(1);
//  tft.textWrite(str);

  pinMode(RA8875_INT, INPUT);
  digitalWrite(RA8875_INT, HIGH);

}

void loop() {
  Can0.events();

  request();
}
