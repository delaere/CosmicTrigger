#include "TTCvi.h"
#include <iostream>
using namespace std;

TtcVi::TtcVi(VmeController* controller,int address):VmeBoard(controller, address, cvA32_U_DATA, cvD16, true) {
  this->channel=1;
  this->channelFrequency=0;
  if(verbosity(NORMAL))cout<<"New TTCvi... ok!"<<endl;
}

void TtcVi::changeChannel(int channel){
  this->channel=channel;
  int DATA=-1;
  switch (channel){
    case 0:
      if (verbosity(NORMAL))cout<<"Set mode to L1A(0)...";
      DATA=0x0000;
      break;
    case 1:
      if (verbosity(NORMAL))cout<<"Set mode to L1A(1)...";
      DATA=0x0001;
      break;
    case 2:
      if (verbosity(NORMAL))cout<<"Set mode to L1A(2)...";
      DATA=0x0002;
      break;
    case 3:
      if (verbosity(NORMAL))cout<<"Set mode to L1A(3)...";
      DATA=0x0003;
      break;
    case -1:
      if (verbosity(NORMAL))cout<<"Set mode to random with frequence: "<<this->channelFrequency<<"...";
      DATA=0x0005;
      break;
    case 4:
      if (verbosity(NORMAL))cout<<"Set mode to VME function...";
      DATA=0x0004;
      break;
    case 5:
      if (verbosity(NORMAL))cout<<"Set mode to random with frequence: "<<this->channelFrequency<<"...";
      DATA=0x0005;
      break;
    case 6:
      if (verbosity(NORMAL))cout<<"Set mode to calibration...";
      DATA=0x0006;
      break;
    case 7:
      if (verbosity(NORMAL))cout<<"Set mode Disabled...";
      DATA=0x0007;
      break;

    default:
      if (verbosity(WARNING))cout<<"*   WARNING: wrong code to change channel. Expected -1(random),0,1,2,3. Statement ignored"<<endl;
    }
  if (DATA>-1){
    DATA+=this->channelFrequency*16*16*16;
    writeData(baseAddress()+0x80,&DATA);
    if (verbosity(NORMAL))std::cout <<" ok!" << std::endl;
    if (verbosity(DEBUG)) std::cout << "Sent: " << std::hex << DATA <<" to TTCvi (add:" << baseAddress() << std::dec <<")" << std::endl;
  }
}

void TtcVi::changeRandomFrequency(int frequencyId){
  this->channelFrequency=frequencyId;
  if (channel==-1){
    if(verbosity(NORMAL))cout<<"Sending new frequency to TTCvi"<<endl;
    this->changeChannel(-1);
  }
}

int TtcVi::viewMode(){  
  int DATA=0;
  readData(baseAddress()+0x80,&DATA);
  switch(DATA%16){
    case 0:
      if (verbosity(NORMAL))cout<<"L1A(0)"<<endl;
      break;
    case 1:
      if (verbosity(NORMAL))cout<<"L1A(1)"<<endl;
      break;
    case 2:
      if (verbosity(NORMAL))cout<<"L1A(2)"<<endl;
      break;
    case 3:
      if (verbosity(NORMAL))cout<<"L1A(3)"<<endl;
      break;
    case 5:
      if (verbosity(NORMAL))cout<<"Random, frequency="<<DATA/(16*16*16)<<endl;
      break;
    case 6:
      if(verbosity(NORMAL))cout<<"Calibration"<<endl;
      break;
    case 7:
      if(verbosity(NORMAL))cout<<"disabled"<<endl;
      break;
    default:
      if(verbosity(WARNING))cerr<<"*   WARNING: unknown TTCvi mode."<<endl;
  }
  return(16*16*16*(DATA/(16*16*16))+DATA%16);
}
