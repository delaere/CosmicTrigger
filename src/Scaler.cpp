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

#include "Scaler.h"
#include "CommonDef.h"
#include <iostream>
using namespace std;

Scaler::Scaler(VmeController* controller,uint32_t address):VmeBoard(controller,address,cvA24_U_DATA,cvD16,true){
  LOG_DEBUG("Address" + int_to_hex(baseAddress()));
  // check the connection...
  uint16_t data = 0;
  readData(baseAddress()+0xFC,&data);
  info_.moduleType_ = data;
  readData(baseAddress()+0xFE,&data);
  info_.serial_number_ = data;
  readData(baseAddress()+0xFA,&data);
  info_.moduleId_ = data;
  assert(info_.moduleId_==0xFAF5);
  LOG_DEBUG("Lecroy 1151N scaler initialized. " + 
            int_to_hex(info_.moduleType_&0x3FF) + " " + int_to_hex(info_.moduleType_>>9) + " " + 
            int_to_hex(info_.serial_number_&0xFFF) + " " + int_to_hex(info_.serial_number_>>11) + " " + 
            int_to_hex(info_.moduleId_));
}
 
uint32_t Scaler::getCount(uint8_t channel, bool reset){
  uint32_t data=0;
  int completeAdd = baseAddress()+0x40+((!reset)*0x40)+4*channel;
  controller()->mode(cvA24_U_DATA,cvD32)->readData(completeAdd,&data);
  LOG_DEBUG("Count for channel " + to_string(channel) + " = " + to_string(data));
  return data;
}

void Scaler::setPreset(uint8_t channel, uint32_t value){
  uint32_t data=value;
  LOG_INFO("Setting presets to "+to_string(value)+" for channel " + to_string(channel) +"...");
  controller()->mode(cvA24_U_DATA,cvD32)->writeData(baseAddress()+0x40+4*channel,&data);
}

void Scaler::reset(){
  int data=0;
  LOG_INFO("Reseting Scaler...");
  writeData(baseAddress(),&data);
}
