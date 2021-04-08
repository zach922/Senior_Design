#include <FlexCAN_T4.h>
FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> Can0;

#define VEHICLE_SPEED 0x0D
#define ENGINE_RPM 0x0C
#define COOLANT_TEMP 0x05
#define OIL_TEMP 0x5C
#define FUEL_LEVEL 0x2F

int STBY = 2;
struct CANData  {
  uint8_t vspd = 0; //vehicle speed
  uint8_t espd = 0; //engine speed
  uint8_t ctmp = 0; //coolant temp
  uint8_t otmp = 0; //oil temp
  uint8_t flvl = 0; //fuel level
  };

void setup(void) {
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

uint8_t processData(const uint8_t pid, const uint8_t buf[8]){
    uint8_t canData;

    if (pid == VEHICLE_SPEED){
        canData = buf[3];
    }
    else if (pid == ENGINE_RPM){
        canData = (256*buf[3]+buf[4])/4;
    }
    else if (pid == COOLANT_TEMP){
        canData = buf[3]-40;
    }
    else if (pid == OIL_TEMP){
        canData = buf[3]-40;
    }
    else if(pid == FUEL_LEVEL){
        canData = (100*buf[3])/255;
    }


        return canData;

}

void canSniff(const CAN_message_t &msg) {
  
  uint8_t pdat = processData(msg.buf[2], msg.buf);

  Serial.print(" Data: "); Serial.print(pdat); Serial.println();


}



void loop() {
  Can0.events();
  CAN_message_t tx_msg, rx_msg;
  tx_msg.id = 0x7DF;
  tx_msg.buf[0] = 0x02;
  tx_msg.buf[1] = 0x01;
  tx_msg.buf[2] = 0x0D;
  tx_msg.buf[3] = 0x00;
  tx_msg.buf[4] = 0x00;
  tx_msg.buf[5] = 0x00;
  tx_msg.buf[6] = 0x00;
  tx_msg.buf[7] = 0x00;


  static uint32_t timeout = millis();
  if (timeout - millis() > 200){
    Can0.write(tx_msg);

    //Can0.read(rx_msg);

    //processData(rx_msg.buf[2], rx_msg.buf);
    
    timeout = millis();
  }

  //display!

}
