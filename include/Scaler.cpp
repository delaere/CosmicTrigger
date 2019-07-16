#include "Scaler.h"
#include "CommonDef.h"
#include <iostream>
using namespace std;

Scaler::Scaler(VmeController* controller,int address):VmeBoard(controller,address,cvA24_U_DATA,cvD16,true){
  LOG_DEBUG("Address" + int_to_hex(baseAddress()));
  getInfo();
}
 
int Scaler::getCount(int channel){
  int DATA=0;
  int completeAdd=baseAddress()+0x80+4*(channel-1);
  controller()->mode(cvA24_U_DATA,cvD32)->readData(completeAdd,&DATA);
  LOG_DEBUG("Count=" + to_string(DATA) + "(" + int_to_hex(DATA) + ") at add:" + int_to_hex(completeAdd));
  return(DATA);
}

int Scaler::getInfo(){ 
  int DATA=0;
  LOG_INFO("Getting Scaler information...");
  readData(baseAddress()+0xFE,&DATA);
  readData(baseAddress()+0xFE,&DATA);
  return(DATA);
}

void Scaler::reset(){
  int DATA=0;
  LOG_INFO("Reseting Scaler...");
  writeData(baseAddress(),&DATA);
}

int Scaler::readPresets(int channel){
  int DATA=0;
  readData(baseAddress()+0x40+4*(channel-1),&DATA);
  LOG_INFO("Preset: "+to_string(DATA));
  return(DATA);
}

void Scaler::setPresets(int channel,int value){
  int DATA=value;
  LOG_INFO("Setting presets to "+to_string(value)+"...");
  writeData(baseAddress()+0x40+4*(channel-1),&DATA);
}
