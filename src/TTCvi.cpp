/*
 *  VeheMencE: a simple library for VME access
 *  Copyright (C) 2019 Universite catholique de Louvain (UCLouvain), Belgium
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "TTCvi.h"
#include <iostream>
using namespace std;

TtcVi::TtcVi(VmeController* controller,int address):VmeBoard(controller, address, cvA32_U_DATA, cvD16, true),channel_(cvL1A0),frequency_(cv1Hz) {
  LOG_INFO("New TTCvi... ok!");
}

void TtcVi::changeChannel(CVTriggerChannel channel){
  channel_=channel;
  LOG_INFO("Set mode to " + to_string(channel));
  program();
}

void TtcVi::changeRandomFrequency(CVTriggerRate frequencyId){
  frequency_ = frequencyId;
  LOG_INFO("Set frequency to " + to_string(frequencyId));
  program();
}
  
void TtcVi::program() const {
  uint16_t data = channel_ + (frequency_<<12);
  writeData(baseAddress()+0x80,&data);
  LOG_DEBUG("Sent: " + int_to_hex(data) + " to TTCvi (add:" + to_string(baseAddress()) + ")");
}

int TtcVi::viewMode() const {  
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
