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

void Discri::setChannel(int num, bool newState){
  if (num==-1){
    setMultiChannel( newState ? 0xFFFF : 0x0000);
  }
  if (newState != ((status_>>num)&1))
    status_ ^= (1u << num);
  // write the result to the proper register
  int DATA = status_;
  writeData(baseAddress()+0x4A,&DATA);
  if (verbosity(NORMAL)) 
    std::cout << "New status for channel " << num <<": "<< newState << std::endl;
}

void Discri::setMultiChannel(int code){
  status_=code;
  int DATA=code;
  writeData(baseAddress()+0x4A,&DATA);
  if (verbosity(NORMAL)) 
    std::cout << "Channels changed. Code:" << code << std::endl;
}

void Discri::setMajority(int num){
  double nRound=(num*50-25.0)/4.0;
  int round;
  if ((nRound+0.5)>int(nRound)){ round=(int)nRound+1;}
  else{ round=(int)nRound;}
  writeData(baseAddress()+0x48,&round);
  if(verbosity(NORMAL))
    std::cout << "Set majority level to " << num << "(sent: " << round << ")" << std::endl;
}

void Discri::setTh(uint8_t value,int num){
  if (num==-1){
      if(verbosity(NORMAL))
        cout<<"Setting all thresholds to "<<value<<endl;
      for (int i=0; i<16; i++) 
        this->setTh(value,i);
  } else{
      if(verbosity(DEBUG)) 
        cout<<"Setting threshold to "<<value<<" on channel "<<num<<"...";
      writeData(baseAddress()+2*num,&value);
      if (verbosity(DEBUG))
        cout<<" ok!"<<endl;
   }
}

void Discri::setWidth(uint8_t value,int num){
  int DATA=value;
  if(verbosity(NORMAL)) 
    cout<<"Setting output width to"<<value<<"...";
  if (num<8) 
    writeData(baseAddress()+0x40,&DATA);
  if (num<0||num>7) 
    writeData(baseAddress()+0x42,&DATA);
  if (verbosity(NORMAL)) 
    cout<<" ok!"<<endl;
}

void Discri::setDeadTime(uint8_t value,int num){
  int DATA=value;
  if(verbosity(NORMAL))
    cout<<"Setting dead time to "<<value<<"...";
  if (num<8) 
    writeData(baseAddress()+0x44,&DATA);
  if (num<0||num>7) 
    writeData(baseAddress()+0x46,&DATA);
  if (verbosity(NORMAL)) 
    cout<<" ok!"<<endl;
}
