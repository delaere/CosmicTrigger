#include "Discri.h"
#include <iostream>
#include <cmath>
using namespace std;

Discri::Discri(VmeController *controller,int add):VmeBoard(controller, add, cvA32_U_DATA, cvD16, true) {
  this->status_=0x0000;//All channels off
  int DATA;
  readData(add+0xFE,&DATA);
  LOG_INFO("Connexion to Discri... ok!");
  this->setMultiChannel(this->status_);
}

void Discri::setChannel(int num, bool newState){
  if (num==-1){
    setMultiChannel( newState ? 0xFFFF : 0x0000);
  }
  if (newState != ((status_>>num)&1))
    status_ ^= (1u << num);
  // write the result to the proper register
  int DATA = status_;
  writeData(baseAddress()+0x4A,&DATA);
  LOG_INFO("New status for channel " + to_string(num) +": "+ to_string(newState));
}

void Discri::setMultiChannel(int code){
  status_=code;
  int DATA=code;
  writeData(baseAddress()+0x4A,&DATA);
  LOG_INFO("Channels changed. Code:" + to_string(code));
}

void Discri::setMajority(int num){
  double nRound=(num*50-25.0)/4.0;
  int round;
  if ((nRound+0.5)>int(nRound)){ round=(int)nRound+1;}
  else{ round=(int)nRound;}
  writeData(baseAddress()+0x48,&round);
  LOG_INFO("Set majority level to " + to_string(num) + "(sent: " + to_string(round) + ")");
}

void Discri::setTh(uint8_t value,int num){
  if (num==-1){
    LOG_INFO("Setting all thresholds to "+to_string(value));
    for (int i=0; i<16; i++) 
      this->setTh(value,i);
  } else {
    LOG_DEBUG("Setting threshold to " + to_string(value) + " on channel " + to_string(num));
    writeData(baseAddress()+2*num,&value);
  }
}

void Discri::setWidth(uint8_t value,int num){
  int DATA=value;
  LOG_INFO("Setting output width to "+ to_string(value));
  if (num<8) 
    writeData(baseAddress()+0x40,&DATA);
  if (num<0||num>7) 
    writeData(baseAddress()+0x42,&DATA);
}

void Discri::setDeadTime(uint8_t value,int num){
  int DATA=value;
  LOG_INFO("Setting dead time to " + to_string(value));
  if (num<8) 
    writeData(baseAddress()+0x44,&DATA);
  if (num<0||num>7) 
    writeData(baseAddress()+0x46,&DATA);
}
