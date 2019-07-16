#include "TTCvi.h"
#include <iostream>
using namespace std;

TtcVi::TtcVi(VmeController* controller,int address):VmeBoard(controller, address, cvA32_U_DATA, cvD16, true) {
  this->channel=1;
  this->channelFrequency=0;
  LOG_INFO("New TTCvi... ok!");
}

void TtcVi::changeChannel(int channel){
  this->channel=channel;
  int DATA=-1;
  switch (channel){
    case 0:
    case 1:
    case 2:
    case 3:
      LOG_INFO("Set mode to L1A("+to_string(channel)+")...");
      DATA=0x0000;
      break;
    case -1:
      LOG_INFO("Set mode to random with frequence: "+to_string(this->channelFrequency)+" ...");
      DATA=0x0005;
      break;
    case 4:
      LOG_INFO("Set mode to VME function...");
      DATA=0x0004;
      break;
    case 5:
      LOG_INFO("Set mode to random with frequence: "+to_string(this->channelFrequency)+"...");
      DATA=0x0005;
      break;
    case 6:
      LOG_INFO("Set mode to calibration...");
      DATA=0x0006;
      break;
    case 7:
      LOG_INFO("Set mode Disabled...");
      DATA=0x0007;
      break;
    default:
      LOG_WARN("Wrong code to change channel. Expected -1(random),0,1,2,3. Statement ignored");
    }
  if (DATA>-1){
    DATA+=this->channelFrequency*16*16*16;
    writeData(baseAddress()+0x80,&DATA);
    LOG_DEBUG("Sent: " + int_to_hex(DATA) + " to TTCvi (add:" + to_string(baseAddress()) + ")");
  }
}

void TtcVi::changeRandomFrequency(int frequencyId){
  this->channelFrequency=frequencyId;
  if (channel==-1){
    LOG_INFO("Sending new frequency to TTCvi");
    this->changeChannel(-1);
  }
}

int TtcVi::viewMode(){  
  int DATA=0;
  readData(baseAddress()+0x80,&DATA);
  switch(DATA%16){
    case 0:
    case 1:
    case 2:
    case 3:
      LOG_INFO("L1A("+to_string(DATA%16)+")");
      break;
    case 5:
      LOG_INFO("Random, frequency="+to_string(DATA>>12));
      break;
    case 6:
      LOG_INFO("Calibration");
      break;
    case 7:
      LOG_INFO("Disabled");
      break;
    default:
      LOG_WARN("Unknown TTCvi mode.");
  }
  return DATA&0xFFFF000F;
}
