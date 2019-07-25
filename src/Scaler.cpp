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

Scaler::Scaler(VmeController* controller,int address):VmeBoard(controller,address,cvA24_U_DATA,cvD16,true){
  LOG_DEBUG("Address" + int_to_hex(baseAddress()));
  // call getInfo to check the connection...
  getInfo();
}
 
int Scaler::getCount(int channel){
  int data=0;
  int completeAdd=baseAddress()+0x80+4*(channel-1);
  controller()->mode(cvA24_U_DATA,cvD32)->readData(completeAdd,&data);
  LOG_DEBUG("Count=" + to_string(data) + "(" + int_to_hex(data) + ") at add:" + int_to_hex(completeAdd));
  return data;
}

int Scaler::getInfo(){ 
  int data=0;
  LOG_INFO("Getting Scaler information...");
  readData(baseAddress()+0xFE,&data);
  readData(baseAddress()+0xFE,&data);
  return data;
}

void Scaler::reset(){
  int data=0;
  LOG_INFO("Reseting Scaler...");
  writeData(baseAddress(),&data);
}

int Scaler::readPresets(int channel){
  int data=0;
  readData(baseAddress()+0x40+4*(channel-1),&data);
  LOG_INFO("Preset: "+to_string(data));
  return data ;
}

void Scaler::setPresets(int channel,int value){
  int data=value;
  LOG_INFO("Setting presets to "+to_string(value)+"...");
  writeData(baseAddress()+0x40+4*(channel-1),&data);
}
