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
  info_.manufacturer_ = readData<uint16_t>(baseAddress()+0x2E)&0xFF;
  info_.manufacturer_ |= (readData<uint16_t>(baseAddress()+0x2A)&0xFF)<<8;
  info_.manufacturer_ |= (readData<uint16_t>(baseAddress()+0x26)&0xFF)<<16;
  assert(info_.manufacturer_==0x80030);
  info_.serial_number_ = readData<uint16_t>(baseAddress()+0x3E)&0xFF;
  info_.serial_number_ |= (readData<uint16_t>(baseAddress()+0x3A)&0xFF)<<8;
  info_.serial_number_ |= (readData<uint16_t>(baseAddress()+0x36)&0xFF)<<16;
  info_.serial_number_ |= (readData<uint16_t>(baseAddress()+0x32)&0xFF)<<24;
  info_.revision_ = readData<uint16_t>(baseAddress()+0x4E)&0xFF;
  info_.revision_ |= (readData<uint16_t>(baseAddress()+0x4A)&0xFF)<<8;
  info_.revision_ |= (readData<uint16_t>(baseAddress()+0x46)&0xFF)<<16;
  info_.revision_ |= (readData<uint16_t>(baseAddress()+0x42)&0xFF)<<24;
  
  LOG_INFO("TTCvi initialized. Serial number: " + int_to_hex(info_.serial_number_) + 
            " rev. " + to_string(info_.revision_) );
}

void TtcVi::reset(){
  writeData(baseAddress()+0x84,0);
}

void TtcVi::trigger(){
  writeData(baseAddress()+0x86,0);
}

void TtcVi::resetCounter(){
  writeData(baseAddress()+0x8C,0);
}

uint32_t TtcVi::getEventNumber(){
  uint32_t event = readData<uint16_t>(baseAddress()+0x8A);
  event |= ((readData<uint16_t>(baseAddress()+0x88)&0xFF)<<16);
  return event;
}

void TtcVi::setEventCounter(uint32_t count){
  writeData(baseAddress()+0x8A,count&0xFFFF);
  writeData(baseAddress()+0x88,(count>>16)&0xFF);
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
  writeData(baseAddress()+0x8b,(uint16_t)command);
}
  
void TtcVi::channelBAsyncCommand(uint8_t addr, uint8_t subAddr, uint8_t data, bool internal){
  uint16_t dta = 0x8000 | (addr<<1) | (internal ? 0x0 : 0x1);
  writeData(baseAddress()+0xC0,dta);
  dta = (subAddr<<8) | data;
  writeData(baseAddress()+0xC2,dta);
}
  
std::pair<uint8_t, uint8_t> TtcVi::getInhibit(unsigned int n){
  uint8_t delay,duration;
  assert(n<4);
  uint32_t address = baseAddress()+0x92+n*0x8;
  delay = readData<uint16_t>(address)&0xFFF;
  duration = readData<uint16_t>(address+0x2)&0xFF;
  return std::make_pair(delay,duration);
}
  
void TtcVi::setInhibit(unsigned int n,uint8_t delay,uint8_t duration){
  assert(n<4);
  uint32_t address = baseAddress()+0x92+n*0x8;
  writeData(address,(uint16_t)delay);
  writeData(address+0x2,(uint16_t)duration);
}
  
std::bitset<4> TtcVi::getBGo(unsigned int n){
  assert(n<4);
  uint32_t address = baseAddress()+0x90+n*0x8;
  return std::bitset<4>(readData<uint16_t>(address)&0xF);
}
  
void TtcVi::setBGo(unsigned int n,bool softTrigger, bool asynchronous, bool repeat, bool autoTrigger){
  assert(n<4);
  uint32_t address = baseAddress()+0x90+n*0x8;
  uint16_t data = (softTrigger<<3) | (asynchronous<<2) | (repeat<<1) | autoTrigger;
  writeData(address,data);
}
  
void TtcVi::triggerBGo(unsigned int n){
  assert(n<4);
  uint32_t address = baseAddress()+0x96+n*0x8;
  writeData(address,1);
}

uint16_t TtcVi::readCSR1(){
  return readData<uint16_t>(baseAddress()+0x80);
}

uint16_t TtcVi::readCSR2(){
  return readData<uint16_t>(baseAddress()+0x82);
}

void TtcVi::writeCSR1(uint16_t word){
  writeData(baseAddress()+0x80,word);
}

void TtcVi::writeCSR2(uint16_t word){
  writeData(baseAddress()+0x82,word);
}
