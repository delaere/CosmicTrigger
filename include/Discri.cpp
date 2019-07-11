#include "Discri.h"
#include <iostream>
#include <cmath>
using namespace std;

discri::discri(vmeController *controller,int add):vmeBoard(controller,cvA32_U_DATA,cvD16){
  this->add=add;
  this->status=0x0000;//All channels off
  int DATA;
  try {
    readData(add+0xFE,&DATA);
    if(vLevel(NORMAL)) std::cout << "Connexion to discri... ok!" << std::endl;
  } catch (CAENVMEexception &e) {
    std::cout << e.what() << " while contacting discriminator at addess " << std::hex << add << std::dec << std::endl;
  }
  this->setMultiChannel(this->status);
}

int discri::setChannel(int num, bool newState){
  if (num==-1){
    if(newState)return(setMultiChannel(0xFFFF));
    else return(setMultiChannel(0x0000));
  }
  
  bool curState=(this->status%(int)pow(2,num+1))/pow(2,num);  
  if(!newState && curState)this->status-=pow(2,num);
  else if (newState && !curState) this->status+=pow(2,num);
  int DATA=status;
  try {
    writeData(this->add+0x4A,&DATA);
    if (vLevel(NORMAL))cout<<"New status for channel "<<num<<": "<<(this->status%(int)pow(2,num+1))/pow(2,num)<<endl;
  } catch (CAENVMEexception &e) {
    std::cout << "Discri: " << e.what() << " while Changing channel status." << std::endl;
    return -1;
  }
  return 1;
}

int discri::setMultiChannel(int code){
  this->status=code;
  int DATA=code;
  try {
    writeData(this->add+0x4A,&DATA);
    if (vLevel(NORMAL)) cout<<"Channels changed. Code:"<<code<<endl;
  } catch (CAENVMEexception &e) {
    std::cout << "Discri: " << e.what() << " while changing channel status" << std::endl;
    return -1;
  }
  return 1;
}

int discri::setMajority(int num){
  double nRound=(num*50-25.0)/4.0;
  int round;
  if ((nRound+0.5)>int(nRound)){ round=(int)nRound+1;}
  else{ round=(int)nRound;}
  try {
    writeData(this->add+0x48,&round);
    if(vLevel(NORMAL))cout<<"Set majority level to "<<num<<"(sent: "<<round<<")"<<endl;
  } catch (CAENVMEexception &e) {
    std::cout << "Discri: " << e.what() << " while setting majority" << std::endl;
    return -1;
  }
  return 1;
}

int discri::setTh(int value,int num){
  if(value>255 || value<0){
    if(vLevel(WARNING)) cerr<<"*  WARNING: illegal value , command ignored"<<endl;
    return(-1);
  }
  else{
    if (num==-1){
      int status=1;
      if(vLevel(NORMAL))cout<<"Setting all thresholds to "<<value<<endl;
      for (int i=0; i<16; i++) if(this->setTh(value,i)<0)status=-1;
      return(status);
    }
    else{
      try {
        if(vLevel(DEBUG)) cout<<"Setting threshold to "<<value<<" on channel "<<num<<"...";
        writeData(add+2*num,&value);
        if (vLevel(DEBUG))cout<<" ok!"<<endl;
        return 1;
      } catch (CAENVMEexception &e) {
        std::cout << "Discri: " << e.what() << " while setting threshold" << std::endl;
        return -1;
      }
    }
  }
  return(-1); //Never happens, normally.
}

int discri::setWidth(int value,int num){
  if(value>255 || value<1){
    if(vLevel(WARNING))cerr<<"*  WARNING: illegal value , command ignored"<<endl;
    return(-1);
  }
  else{
      int DATA=value;
      if(vLevel(NORMAL)) cout<<"Setting output width to"<<value<<"...";
      try {
        if (num<8) writeData(this->add+0x40,&DATA);
        if (num<0||num>7) writeData(this->add+0x42,&DATA);
        if (vLevel(NORMAL)) cout<<" ok!"<<endl;
      } catch (CAENVMEexception &e) {
        std::cout << "Discri: " << e.what() << " while setting width" << std::endl;
        return -1;
      }
      return 1;
  }
  return(-1); //Never happens, normally.
}

int discri::viewStatus(void){
  if(vLevel(NORMAL)) std::cout<< std::hex << this->status << std::dec <<endl;
  return(this->status);
}

int discri::setDeadTime(int value,int num){
  if(value>255 || value<0){
    if(vLevel(WARNING))cerr<<"*  WARNING: illegal value , command ignored"<<endl;
    return(-1);
  }
  else {
    int DATA=value;
    if(vLevel(NORMAL))cout<<"Setting dead time to "<<value<<"...";
    try {
      if (num<8) writeData(this->add+0x44,&DATA);
      if (num<0||num>7) writeData(this->add+0x46,&DATA);
      if (vLevel(NORMAL)) cout<<" ok!"<<endl;
    } catch (CAENVMEexception &e) {
      std::cout << "Discri: " << e.what() << " while setting dead time" << std::endl;
      return -1;
    }
    return 1;
  }
  return(-1); //Never happens, normally.
}
