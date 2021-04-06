/*
    The functions required for interpreting and requesting CAN data from the CAN Controller
*/ 
#ifndef INTERPRET_H
#define INTERPRET_H

uint8_t requestData(uint8_t pid, uint8_t* data);
uint8_t processData(uint8_t pid, uint8_t* buf);

#endif 
