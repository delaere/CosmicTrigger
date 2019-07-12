#include "HV.h"
#include <iostream>
#include <unistd.h>
using namespace std;

hv::hv(vmeController *controller, int bridgeAdd, int hvAdd):vmeBoard(controller, bridgeAdd, cvA24_S_DATA, cvD16, true),hvAdd(hvAdd) { }

int hv::reset(void){
  int DATA=0x0000;
  if(verbosity(NORMAL))cout<<"Reseting HV...";
  try {
    writeData(baseAddress()+0x06,&DATA);
    if (verbosity(NORMAL))cout<<" ok!"<<endl;
  } catch (CAENVMEexception &e) {
    std::cout << "HV: " << e.what() << " while resetting." << std::endl;
    return -1;
  }
  return 1;
}

int hv::getStatus(){
    int DATA=0;
    if(verbosity(DEBUG))cout<<"Getting HV status...";
    try {
      readData(baseAddress()+0x02,&DATA);
      if (verbosity(DEBUG))cout<<" ok!"<<endl;
    } catch (CAENVMEexception &e) {
      std::cout << "HV: " << e.what() << " while getting status." << std::endl;
      return -1;
    }
    return DATA;
}

int hv::comLoop(int data1, int data2){
  try {
    usleep(100000);
    if(getStatus()==0xFFFF&&verbosity(WARNING))cout<<"*  WARNING: Initial status of HV was: error..."<<endl;
    int DATA=0x0001;
    writeData(baseAddress(),&DATA);//Hello  
    DATA=this->hvAdd;
    writeData(baseAddress(),&DATA);//Alim add
    DATA=data1;
    writeData(baseAddress(),&DATA);//Command
    if (data2>-1){
      DATA=data2;
      writeData(baseAddress(),&DATA);//Value 
    }
    DATA=0x0000;
    writeData(baseAddress()+0x04,&DATA); //Send command
    } catch (CAENVMEexception &e) {
      std::cout << "HV: " << e.what() << " while sending "<< std::hex << data1 <<"&"<< data2 << std::dec << std::endl;
      return -1;
    }
    if(getStatus()==0xFFFF){
      if(verbosity(ERROR)) std::cout<<"** ERROR while sending "<< std::hex << data1 <<"&"<< data2 << std::dec << std::endl;
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
    if(verbosity(WARNING)) cerr<<"*   WARNING: invalid parameter: "<<channel<<". Statement ignored"<<endl;
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
    if(verbosity(WARNING)) cerr<<"*   WARNING: invalid parameter: "<<channel<<". Statement ignored"<<endl;
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
  readData(baseAddress(),&DATA);
  if(DATA){cout<<"No data..."<<endl; return(0);}
  
  for(int i=0; i<4; i++){
    for(int j=0; j<4; j++){
      readData(baseAddress(),&DATA);
      val[i][j]=DATA;
      }
  }
  if(DATA==0xFFFF){this->reset();}
  return(val);
}
