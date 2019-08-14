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

TtcVi::TtcVi(VmeController* controller,int address):VmeBoard(controller, address, cvA32_U_DATA, cvD16, true) {
  uint16_t data;
  
  readData(baseAddress()+0x2E,&data);
  info_.manufacturer_ = data&0xFF;
  readData(baseAddress()+0x2A,&data);
  info_.manufacturer_ |= (data&0xFF)<<8;
  readData(baseAddress()+0x26,&data);
  info_.manufacturer_ |= (data&0xFF)<<16;
  assert(info_.manufacturer_==0x80030);
  readData(baseAddress()+0x3E,&data);
  info_.serial_number_ = data&0xFF;
  readData(baseAddress()+0x3A,&data);
  info_.serial_number_ |= (data&0xFF)<<8;
  readData(baseAddress()+0x36,&data);
  info_.serial_number_ |= (data&0xFF)<<16;
  readData(baseAddress()+0x32,&data);
  info_.serial_number_ |= (data&0xFF)<<24;
  readData(baseAddress()+0x4E,&data);
  info_.revision_ = data&0xFF;
  readData(baseAddress()+0x4A,&data);
  info_.revision_ |= (data&0xFF)<<8;
  readData(baseAddress()+0x46,&data);
  info_.revision_ |= (data&0xFF)<<16;
  readData(baseAddress()+0x42,&data);
  info_.revision_ |= (data&0xFF)<<24;
  
  LOG_INFO("TTCvi initialized. Serial number: " + int_to_hex(info_.serial_number_) + 
            " rev. " + to_string(info_.revision_) );
}

void TtcVi::reset(){
  uint16_t data = 0;
  writeData(baseAddress()+0x84,&data);
}

void TtcVi::trigger(){
  uint16_t data = 0;
  writeData(baseAddress()+0x86,&data);
}

void TtcVi::resetCounter(){
  uint16_t data = 0;
  writeData(baseAddress()+0x8C,&data);
}

uint32_t TtcVi::getEventNumber(){
  uint32_t event = 0;
  uint16_t data = 0;
  readData(baseAddress()+0x8A,&data);
  event = data;
  readData(baseAddress()+0x88,&data);
  event |= ((data&0xFF)<<16);
  return event;
}

void TtcVi::setEventCounter(uint32_t count){
  uint16_t data = count&0xFFFF;
  writeData(baseAddress()+0x8A,&data);
  data = (count>>16)&0xFF;
  writeData(baseAddress()+0x88,&data);
}

void TtcVi::setCounterMode(bool orbit){
  uint16_t csr1 = readCSR1();
  if(orbit) { csr1 = csr1 |= (1<<15); }
  else { csr1 = csr1 &= 0x7FFF; }
  writeCSR1(csr1);
}

TtcVi::CVTriggerChannel TtcVi::getTriggerChannel(){
  uint16_t csr1 = readCSR1();
  return TtcVi::CVTriggerChannel(csr1&0x7);
}
  
void TtcVi::setTriggerChannel(TtcVi::CVTriggerChannel channel){
  uint16_t csr1 = readCSR1();
  csr1 = (csr1&0xFFF8)|int(channel);
  writeCSR1(csr1);
}
  
TtcVi::CVTriggerRate TtcVi::getRandomTriggerRate(){
  uint16_t csr1 = readCSR1();
  return TtcVi::CVTriggerRate((csr1>>12)&0x7);
}
  
void TtcVi::setRandomTriggerRate(TtcVi::CVTriggerRate rate){
  uint16_t csr1 = readCSR1();
  csr1 = (csr1&0x8FFF)|(int(rate)<<12);
  writeCSR1(csr1);
}
  
uint8_t TtcVi::getFIFOStatus(){
  uint16_t csr1 = readCSR1();
  return (csr1>>4)&0x3;
}
  
void TtcVi::resetL1FIFO(){
  uint16_t csr1 = readCSR1();
  csr1 |= 0x40;
  writeCSR1(csr1);
}
  
uint8_t TtcVi::getBC0Delay(){
  uint16_t csr1 = readCSR1();
  return (csr1>>8)&0xF;
}

void TtcVi::channelBAsyncCommand(uint8_t command){
  uint16_t data = command;
  writeData(baseAddress()+0x8b,&data);
}
  
void TtcVi::channelBAsyncCommand(uint8_t addr, uint8_t subAddr, uint8_t data, bool internal){
  uint16_t dta = 0x8000 | (addr<<1) | (internal ? 0x0 : 0x1);
  writeData(baseAddress()+0xC0,&dta);
  dta = (subAddr<<8) | data;
  writeData(baseAddress()+0xC2,&dta);
}
  
std::pair<uint8_t, uint8_t> TtcVi::getInhibit(unsigned int n){
  uint8_t delay,duration;
  uint16_t data;
  assert(n<4);
  uint32_t address = baseAddress()+0x92+n*0x8;
  readData(address,&data);
  delay = data&0xFFF;
  readData(address+0x2,&data);
  duration = data&0xFF;
  return std::make_pair(delay,duration);
}
  
void TtcVi::setInhibit(unsigned int n,uint8_t delay,uint8_t duration){
  uint16_t data = delay;
  assert(n<4);
  uint32_t address = baseAddress()+0x92+n*0x8;
  writeData(address,&data);
  data = duration;
  writeData(address+0x2,&data);
}
  
std::bitset<4> TtcVi::getBGo(unsigned int n){
  uint16_t data;
  assert(n<4);
  uint32_t address = baseAddress()+0x90+n*0x8;
  readData(address,&data);
  return std::bitset<4>(data&0xF);
}
  
void TtcVi::setBGo(unsigned int n,bool softTrigger, bool asynchronous, bool repeat, bool autoTrigger){
  uint16_t data;
  assert(n<4);
  uint32_t address = baseAddress()+0x90+n*0x8;
  data = (softTrigger<<3) | (asynchronous<<2) | (repeat<<1) | autoTrigger;
  writeData(address,&data);
}
  
void TtcVi::triggerBGo(unsigned int n){
  uint16_t data = 1;
  assert(n<4);
  uint32_t address = baseAddress()+0x96+n*0x8;
  writeData(address,&data);
}

uint16_t TtcVi::readCSR1(){
  uint16_t data;
  readData(baseAddress()+0x80,&data);
  return data;
}

uint16_t TtcVi::readCSR2(){
  uint16_t data;
  readData(baseAddress()+0x82,&data);
  return data;
}

void TtcVi::writeCSR1(uint16_t word){
  uint16_t data = word;
  writeData(baseAddress()+0x80,&data);
}

void TtcVi::writeCSR2(uint16_t word){
  uint16_t data = word;
  writeData(baseAddress()+0x82,&data);
}
