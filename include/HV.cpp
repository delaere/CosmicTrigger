#include "HV.h"
#include <iostream>
#include <unistd.h>
using namespace std;

hv::hv(vmeController *controller, int bridgeAdd, int hvAdd):vmeBoard(controller,cvA24_S_DATA,cvD16){
  this->add=bridgeAdd;
  this->hvAdd=hvAdd;
  setAM(cvA24_S_DATA);
  setDW(cvD16);
}

int hv::reset(void){
  int DATA=0x0000;
  if(vLevel(NORMAL))cout<<"Reseting HV...";
  try {
    writeData(add+0x06,&DATA);
    if (vLevel(NORMAL))cout<<" ok!"<<endl;
  } catch (CAENVMEexception &e) {
    std::cout << "HV: " << e.what() << " while resetting." << std::endl;
    return -1;
  }
  return 1;
}

int hv::getStatus(){
    int DATA=0;
    if(vLevel(DEBUG))cout<<"Getting HV status...";
    try {
      readData(add+0x02,&DATA);
      if (vLevel(DEBUG))cout<<" ok!"<<endl;
    } catch (CAENVMEexception &e) {
      std::cout << "HV: " << e.what() << " while getting status." << std::endl;
      return -1;
    }
    return DATA;
}

int hv::comLoop(int data1, int data2){
  try {
    usleep(100000);
    if(getStatus()==0xFFFF&&vLevel(WARNING))cout<<"*  WARNING: Initial status of HV was: error..."<<endl;
    int DATA=0x0001;
    writeData(this->add,&DATA);//Hello  
    DATA=this->hvAdd;
    writeData(this->add,&DATA);//Alim add
    DATA=data1;
    writeData(this->add,&DATA);//Command
    if (data2>-1){
      DATA=data2;
      writeData(this->add,&DATA);//Value 
    }
    DATA=0x0000;
    writeData(this->add+0x04,&DATA); //Send command
    } catch (CAENVMEexception &e) {
      std::cout << "HV: " << e.what() << " while sending "<< std::hex << data1 <<"&"<< data2 << std::dec << std::endl;
      return -1;
    }
    if(getStatus()==0xFFFF){
      if(vLevel(ERROR)) std::cout<<"** ERROR while sending "<< std::hex << data1 <<"&"<< data2 << std::dec << std::endl;
      return -1;
    }
    return 1;
}

int hv::setChState(bool state, int channel){
  if (channel<0){
    int status=1;
    for (int i=0; i<4; i++){
      if(setChState(state,i)<0)status=-1;
    }
    return(status);
  }
  else if(channel>3){
    if(vLevel(WARNING)) cerr<<"*   WARNING: invalid parameter: "<<channel<<". Statement ignored"<<endl;
    return(-1);
  }
  else{
    return(comLoop(channel*256+0x000B-state));
  }
}

int hv::setChV(int volt, int channel){
  if (channel<0){
    int status=1;
    for(int i=0; i<4; i++){
      if(setChV(volt,i)<0) status=-1;
    }
    return(status);
  }
  else if(channel>3){
    if(vLevel(WARNING)) cerr<<"*   WARNING: invalid parameter: "<<channel<<". Statement ignored"<<endl;
    return(-1);
  }
  else{
    usleep(10000);
    return(comLoop(channel*256+0x0003,volt));
  }
}

double ** hv::readValues(double ** val){
  if(val==0){
    cout<<"New value vector"<<endl;
    val=new double * [4]; 
    for(int i=0; i<4; i++) val[i]=new double[4];
  }
  
  if(comLoop(0x01)==-1)return(0);
  int DATA=0;
  usleep(100000);
  getStatus();
  readData(add,&DATA);
  if(DATA){cout<<"No data..."<<endl; return(0);}
  
  for(int i=0; i<4; i++){
    for(int j=0; j<4; j++){
      readData(add,&DATA);
      val[i][j]=DATA;
      }
  }
  if(DATA==0xFFFF){this->reset();}
  return(val);
}
