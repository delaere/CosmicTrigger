#include "Scaler.h"
#include "CommonDef.h"
#include <iostream>
using namespace std;

scaler::scaler(vmeController* controller,int address):vmeBoard(controller,address,cvA24_U_DATA,cvD16,true){
  if(verbosity(DEBUG)) std::cout << "Address" << std::hex << baseAddress() << std::dec <<endl;
  getInfo();
}
 
int scaler::getCount(int channel){
  int DATA=0;
  int completeAdd=baseAddress()+0x80+4*(channel-1);
  controller()->mode(cvA24_U_DATA,cvD32)->readData(completeAdd,&DATA);
  if(verbosity(DEBUG)) std::cout << "Count=" << DATA << "(" << std::hex << DATA << ") at add:" << completeAdd << std::dec << endl;
  return(DATA);
}

int scaler::getInfo(){ 
  int DATA=0;
  if(verbosity(NORMAL))cout<<"Getting scaler information...";
  readData(baseAddress()+0xFE,&DATA);
  readData(baseAddress()+0xFE,&DATA);
  return(DATA);
}

int scaler::reset(){
  int DATA=0;
  if(verbosity(NORMAL))cout<<"Reseting scaler...";
  writeData(baseAddress(),&DATA);
  if(verbosity(NORMAL)){
  std::cout << " ok!" << std::endl;
  }
  return(1);
}

int scaler::readPresets(int channel){
  int DATA=0;
  readData(baseAddress()+0x40+4*(channel-1),&DATA);
  if(verbosity(NORMAL))cout<<"Preset: "<<DATA<<endl;
  return(DATA);
}

int scaler::setPresets(int channel,int value){
  int DATA=value;
  if(verbosity(NORMAL))cout<<"Setting presets to "<<value<<"...";
  writeData(baseAddress()+0x40+4*(channel-1),&DATA);
  if(verbosity(NORMAL)){
    cout<<" ok!"<<endl;
  }
  return(1);
}
