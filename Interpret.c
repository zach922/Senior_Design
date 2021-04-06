#include"consts.h"
#include"Interpret.h"

uint8_t requestData(uint8_t pid, uint8_t* data){
    CANMSG msgTx, msgRx;

    msgTx.buf = config buffer;
    msgTx.pid = pid;

    CANWrite(msgTx);

    while(){
        if(CANRead(msgRx)){
            *data = processData(pid, msgRx.buf);
            return 1;
        }
    }

    return 0;
}

uint8_t processData(uint8_t pid, uint8_t* buf){
    uint8_t canData;

    if (pid == VEHICLE_SPEED){
        canData = buf[0];
    }
    else if (pid == ENGINE_RPM){
        canData = (256*buf[0]+buf[1])/4;
    }
    else if (pid == COOLANT_TMP){
        canData = buf[0]-40;
    }
    else if (pid == OIL_TEMP){
        canData = buf[0]-40;
    }
    else if(pid == FUEL_LEVEL){
        canData = (100*buf[0])/255;
    }
    else {
        canData = 0;
    }

    return canData
}
