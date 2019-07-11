#include "Scaler.h"
#include "CommonDef.h"
#include <iostream>
using namespace std;

scaler::scaler(vmeController* controller,int address):vmeBoard(controller,cvA24_U_DATA,cvD16){
  this->add=address;
  if(vLevel(DEBUG)) std::cout << "Address" << std::hex << add << std::dec <<endl;
  getInfo();
}
 
int scaler::getCount(int channel){
  int DATA=0;
  int completeAdd=add+0x80+4*(channel-1);
  try { 
    readData(completeAdd,&DATA,cvA24_U_DATA,cvD32);
  } catch(CAENVMEexception &e) {
    std::cout << e.what() << " while testing scaler count." << std::endl;
    return -1;
  }
  if(vLevel(DEBUG)) std::cout << "Count=" << DATA << "(" << std::hex << DATA << ") at add:" << completeAdd << std::dec << endl;
  return(DATA);
}

int scaler::getInfo(){ 
  int DATA=0;
  if(vLevel(NORMAL))cout<<"Getting scaler information...";
  readData(add+0xFE,&DATA);
  try { 
    readData(add+0xFE,&DATA);
  } catch(CAENVMEexception &e) {
    std::cout << e.what() << " while testing scaler communication." << std::endl;
    return -1;
  }
  return(DATA);
}

int scaler::reset(){
  int DATA=0;
  if(vLevel(NORMAL))cout<<"Reseting scaler...";
  try {
    writeData(add,&DATA);
  } catch(CAENVMEexception &e) {
    std::cout << e.what() << " while resetting the scaler." << std::endl;
    return(-1);
  }
  if(vLevel(NORMAL)){
  std::cout << " ok!" << std::endl;
  }
  return(1);
}

int scaler::readPresets(int channel){
  int DATA=0;
  try {
    readData(add+0x40+4*(channel-1),&DATA);
  } catch(CAENVMEexception &e) {
    std::cout << e.what() << " while reading scaler presets." << std::endl;
    return(-1);
  }
  if(vLevel(NORMAL))cout<<"Preset: "<<DATA<<endl;
  return(DATA);
}

int scaler::setPresets(int channel,int value){
  int DATA=value;
  if(vLevel(NORMAL))cout<<"Setting presets to "<<value<<"...";
  try {
    writeData(add+0x40+4*(channel-1),&DATA);
  } catch(CAENVMEexception &e) {
    std::cout << e.what() << " while setting scaler presets." << std::endl;
    return -1;
  }
  if(vLevel(NORMAL)){
    cout<<" ok!"<<endl;
  }
  return(1);
}
