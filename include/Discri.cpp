#include "Discri.h"
#include <iostream>
#include <cmath>
using namespace std;

Discri::Discri(VmeController *controller,int add):VmeBoard(controller, add, cvA32_U_DATA, cvD16, true) {
  this->status_=0x0000;//All channels off
  int DATA;
  readData(add+0xFE,&DATA);
  if(verbosity(NORMAL)) std::cout << "Connexion to Discri... ok!" << std::endl;
  this->setMultiChannel(this->status_);
}

int Discri::setChannel(int num, bool newState){
  if (num==-1){
    if(newState)return(setMultiChannel(0xFFFF));
    else return(setMultiChannel(0x0000));
  }
  
  bool curState=(status_%(int)pow(2,num+1))/pow(2,num);  
  if(!newState && curState)status_-=pow(2,num);
  else if (newState && !curState) status_+=pow(2,num);
  int DATA=status_;
  writeData(baseAddress()+0x4A,&DATA);
  if (verbosity(NORMAL)) cout<<"New status for channel "<<num<<": "<<(status_%(int)pow(2,num+1))/pow(2,num)<<endl;
  return 1;
}

int Discri::setMultiChannel(int code){
  status_=code;
  int DATA=code;
  writeData(baseAddress()+0x4A,&DATA);
  if (verbosity(NORMAL)) cout<<"Channels changed. Code:"<<code<<endl;
  return 1;
}

int Discri::setMajority(int num){
  double nRound=(num*50-25.0)/4.0;
  int round;
  if ((nRound+0.5)>int(nRound)){ round=(int)nRound+1;}
  else{ round=(int)nRound;}
  writeData(baseAddress()+0x48,&round);
  if(verbosity(NORMAL))cout<<"Set majority level to "<<num<<"(sent: "<<round<<")"<<endl;
  return 1;
}

int Discri::setTh(int value,int num){
  if(value>255 || value<0){
    if(verbosity(WARNING)) cerr<<"*  WARNING: illegal value , command ignored"<<endl;
    return(-1);
  }
  else{
    if (num==-1){
      int status=1;
      if(verbosity(NORMAL))cout<<"Setting all thresholds to "<<value<<endl;
      for (int i=0; i<16; i++) if(this->setTh(value,i)<0)status=-1;
      return(status);
    }
    else{
      if(verbosity(DEBUG)) cout<<"Setting threshold to "<<value<<" on channel "<<num<<"...";
      writeData(baseAddress()+2*num,&value);
      if (verbosity(DEBUG))cout<<" ok!"<<endl;
      return 1;
    }
  }
  return(-1); //Never happens, normally.
}

int Discri::setWidth(int value,int num){
  if(value>255 || value<1){
    if(verbosity(WARNING))cerr<<"*  WARNING: illegal value , command ignored"<<endl;
    return(-1);
  }
  else{
      int DATA=value;
      if(verbosity(NORMAL)) cout<<"Setting output width to"<<value<<"...";
      if (num<8) writeData(baseAddress()+0x40,&DATA);
      if (num<0||num>7) writeData(baseAddress()+0x42,&DATA);
      if (verbosity(NORMAL)) cout<<" ok!"<<endl;
      return 1;
  }
  return(-1); //Never happens, normally.
}

int Discri::viewStatus(void){
  if(verbosity(NORMAL)) std::cout<< std::hex << status_ << std::dec <<endl;
  return(status_);
}

int Discri::setDeadTime(int value,int num){
  if(value>255 || value<0){
    if(verbosity(WARNING))cerr<<"*  WARNING: illegal value , command ignored"<<endl;
    return(-1);
  }
  else {
    int DATA=value;
    if(verbosity(NORMAL))cout<<"Setting dead time to "<<value<<"...";
    if (num<8) writeData(baseAddress()+0x44,&DATA);
    if (num<0||num>7) writeData(baseAddress()+0x46,&DATA);
    if (verbosity(NORMAL)) cout<<" ok!"<<endl;
    return 1;
  }
  return(-1); //Never happens, normally.
}
