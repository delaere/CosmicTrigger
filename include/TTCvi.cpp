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
  int data=-1;
  switch (channel){
    case 0:
    case 1:
    case 2:
    case 3:
      LOG_INFO("Set mode to L1A("+to_string(channel)+")...");
      data=0x0000;
      break;
    case -1:
      LOG_INFO("Set mode to random with frequence: "+to_string(this->channelFrequency)+" ...");
      data=0x0005;
      break;
    case 4:
      LOG_INFO("Set mode to VME function...");
      data=0x0004;
      break;
    case 5:
      LOG_INFO("Set mode to random with frequence: "+to_string(this->channelFrequency)+"...");
      data=0x0005;
      break;
    case 6:
      LOG_INFO("Set mode to calibration...");
      data=0x0006;
      break;
    case 7:
      LOG_INFO("Set mode Disabled...");
      data=0x0007;
      break;
    default:
      LOG_WARN("Wrong code to change channel. Expected -1(random),0,1,2,3. Statement ignored");
    }
  if (data>-1){
    data+=(this->channelFrequency<<12);
    writeData(baseAddress()+0x80,&data);
    LOG_DEBUG("Sent: " + int_to_hex(data) + " to TTCvi (add:" + to_string(baseAddress()) + ")");
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
  int data=0;
  readData(baseAddress()+0x80,&data);
  switch(data%16){
    case 0:
    case 1:
    case 2:
    case 3:
      LOG_INFO("L1A("+to_string(data%16)+")");
      break;
    case 5:
      LOG_INFO("Random, frequency="+to_string(data>>12));
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
  return data&0xFFFF000F;
}
