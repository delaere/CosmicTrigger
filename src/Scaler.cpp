#include "Scaler.h"
#include "CommonDef.h"
#include <iostream>
using namespace std;

Scaler::Scaler(VmeController* controller,int address):VmeBoard(controller,address,cvA24_U_DATA,cvD16,true){
  LOG_DEBUG("Address" + int_to_hex(baseAddress()));
  getInfo();
}
 
int Scaler::getCount(int channel){
  int data=0;
  int completeAdd=baseAddress()+0x80+4*(channel-1);
  controller()->mode(cvA24_U_DATA,cvD32)->readData(completeAdd,&data);
  LOG_DEBUG("Count=" + to_string(data) + "(" + int_to_hex(data) + ") at add:" + int_to_hex(completeAdd));
  return(data);
}

int Scaler::getInfo(){ 
  int data=0;
  LOG_INFO("Getting Scaler information...");
  readData(baseAddress()+0xFE,&data);
  readData(baseAddress()+0xFE,&data);
  return(data);
}

void Scaler::reset(){
  int data=0;
  LOG_INFO("Reseting Scaler...");
  writeData(baseAddress(),&data);
}

int Scaler::readPresets(int channel){
  int data=0;
  readData(baseAddress()+0x40+4*(channel-1),&data);
  LOG_INFO("Preset: "+to_string(data));
  return(data);
}

void Scaler::setPresets(int channel,int value){
  int data=value;
  LOG_INFO("Setting presets to "+to_string(value)+"...");
  writeData(baseAddress()+0x40+4*(channel-1),&data);
}
