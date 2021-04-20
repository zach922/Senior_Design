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
#define VOLTAGE 0x42
#define STBY  2

typedef struct   {
  int vspd; //vehicle speed
  int espd; //engine speed
  int ctmp; //coolant temp
  int otmp; //oil temp
  int flvl; //fuel level
  int volt; //battery voltage
} CANDATA;

//Display definitions
#define WIDTH 480
#define HEIGHT 272
#define RA8875_INT 3
#define RA8875_CS 10
#define RA8875_RESET 9

// Initialize Screen and CAN transiever 
Adafruit_RA8875 tft = Adafruit_RA8875(RA8875_CS, RA8875_RESET);
FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> Can0;

// Initialize CAN data struct
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
    else if(pid == VOLTAGE){
      cdat.volt = (256*buf[3]+buf[4])/1000;
    }
}

void displayVspd(){
  tft.fillRect(140, 106, 120, 70, RA8875_BLACK);

  tft.textTransparent(RA8875_WHITE);
  tft.textSetCursor(140,106);
  tft.textEnlarge(8);
  char str[8];
  sprintf(str, "%d", cdat.vspd);
  tft.textWrite(str);
  tft.textSetCursor(260,106);
  tft.textWrite("KPH");
}

void displayEspd(){
  tft.fillRect(150, 170, 100, 55, RA8875_BLACK);

  tft.textTransparent(RA8875_WHITE);
  
  tft.textSetCursor(150, 170);
  tft.textEnlarge(2);
  char str[8];
  sprintf(str, "%d", cdat.espd);
  tft.textWrite(str);
  tft.textSetCursor(260,170);
  tft.textWrite("RPM");
}

void displayOtmp(){
  tft.fillRect(1, 30, 60, 35, RA8875_BLACK);

  tft.textTransparent(RA8875_WHITE);
  
  tft.textSetCursor(1,30);
  tft.textEnlarge(1);
  char str[8];
  sprintf(str, "%d", cdat.otmp);
  tft.textWrite(str);
  tft.textSetCursor(1,1);
  tft.textWrite("OIL TEMP");
}

void displayCtmp(){
  tft.fillRect(190, 30, 60, 35, RA8875_BLACK);

  tft.textTransparent(RA8875_WHITE);
  
  tft.textSetCursor(190,30);
  tft.textEnlarge(1);
  char str[8];
  sprintf(str, "%d", cdat.ctmp);
  tft.textWrite(str);
  tft.textSetCursor(160,1);
  tft.textWrite("Coolant TEMP");
}

void displayFlvl(){
  tft.fillRect(410, 120, 60, 140, RA8875_WHITE);

  int lvl = ((100-cdat.flvl)*138)/100;
  if (lvl > 1 ){
    tft.fillRect(411, 121, 58, lvl, RA8875_BLACK);
  }

  
  tft.textTransparent(RA8875_WHITE);
  tft.textSetCursor(410,86);
  tft.textEnlarge(1);
  tft.textWrite("FUEL");
  
}

void displayVolt(){
  tft.fillRect(1, 230, 60, 35, RA8875_BLACK);

  tft.textTransparent(RA8875_WHITE);
  
  tft.textSetCursor(1,230);
  tft.textEnlarge(1);
  char str[8];
  sprintf(str, "%d", 12);
  tft.textWrite(str);
  tft.textSetCursor(1,200);
  tft.textWrite("Voltage");
}

void displayChkEng(){
    tft.fillRect(1, 100, 105, 15, RA8875_BLACK);
    
  if (cdat.ctmp>150 || cdat.otmp>150 )
  {


    tft.textTransparent(RA8875_WHITE);
    tft.textSetCursor(1,100);
    tft.textEnlarge(.95);
    tft.textWrite("CHECK ENGINE!");
  }
}

void displayDiagnostic(){
  tft.textMode();
  tft.textTransparent(RA8875_WHITE);
  
  displayVspd();
  displayEspd();
  displayOtmp();
  displayCtmp();
  displayFlvl();
  displayVolt();
  displayChkEng();

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
  CAN_message_t msg, rx_msg;
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

  if(Can0.read(rx_msg) && rx_msg.id == 0x7E8){
    processData(rx_msg.buf[2], rx_msg.buf);
  }
}

//Request for all diagnostics to be updated
void requestAll(){
  requestDiagnostic(VEHICLE_SPEED);
  requestDiagnostic(ENGINE_RPM);
  requestDiagnostic(COOLANT_TEMP);
  requestDiagnostic(OIL_TEMP);
  requestDiagnostic(FUEL_LEVEL);
}


void setup() {

  // CAN Config
  Serial.begin(11500); delay(400);
  pinMode(STBY, OUTPUT); digitalWrite(STBY, LOW); /* optional tranceiver enable pin */
  Can0.begin();
  Can0.setBaudRate(500000);
  Can0.setMaxMB(16);
  Can0.enableFIFO();
  Can0.mailboxStatus();
  
  // Initialize the display using RA8875_480x272
  if (!tft.begin(RA8875_480x272)) {
    Serial.println("RA8875 Not Found!");
    while (1);
  }

  // initialize Screen
  tft.displayOn(true);
  tft.GPIOX(true);      // Enable TFT - display enable tied to GPIOX
  tft.PWM1config(true, RA8875_PWM_CLK_DIV1024); // PWM output for backlight
  tft.PWM1out(255);
  tft.fillScreen(RA8875_BLACK);

}

int updateDisp = 0;

void loop() {
  Can0.events();

  requestAll();

  if (updateDisp == 1000){
    updateDisp=0;
    displayDiagnostic();
    printDiagnostic();
  }
  updateDisp++;

}
