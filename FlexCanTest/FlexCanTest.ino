#include <FlexCAN_T4.h>
FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> Can0;

#define VEHICLE_SPEED 0x0D
#define ENGINE_RPM 0x0C
#define COOLANT_TEMP 0x05
#define OIL_TEMP 0x5C
#define FUEL_LEVEL 0x2F

int STBY = 2;

typedef struct   {
  int vspd; //vehicle speed
  int espd; //engine speed
  int ctmp; //coolant temp
  int otmp; //oil temp
  int flvl; //fuel level
} CANDATA;

CANDATA cdat;

void setup(void) {
  //CAN Config
  Serial.begin(115200); delay(400);
  pinMode(STBY, OUTPUT); digitalWrite(STBY, LOW); /* optional tranceiver enable pin */
  Can0.begin();
  Can0.setBaudRate(500000);
  Can0.setMaxMB(16);
  Can0.enableFIFO();
  Can0.enableFIFOInterrupt();
  Can0.onReceive(canSniff);
  Can0.mailboxStatus();
}

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

void canSniff(const CAN_message_t &msg) {
  
  //uint8_t pdat = 
  processData(msg.buf[2], msg.buf);

  printDiagnostic();
}

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

void request(){
  requestDiagnostic(VEHICLE_SPEED);
  requestDiagnostic(ENGINE_RPM);
  requestDiagnostic(COOLANT_TEMP);
  requestDiagnostic(OIL_TEMP);
  requestDiagnostic(FUEL_LEVEL);
}

void loop() {
  Can0.events();

  request();
  //display(cdat);

}
