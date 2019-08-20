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

#include "TDC.h"
#include <vector>
#include "time.h"
#include <iostream>
using namespace std;

unsigned int digit(unsigned int data, int begin, int end) {
    if (begin<end){return -1;}
    else return (((1<<(begin+1))-1)&data)>>end;
}

unsigned int digit(unsigned int data, int position) {
    return (data>>position)&1;
}

Tdc::Tdc(VmeController* controller,uint32_t address):VmeBoard(controller, address, cvA32_U_DATA, cvD16, true) {
  opcode_=baseAddress()+0x102E;
  statusRegister_=baseAddress()+0x1002;
  microHandshake_=baseAddress()+0x1030;
  outputBuffer_=baseAddress()+0x0000;
  eventFIFO_=baseAddress()+0x1038;
  controlRegister_=baseAddress()+0x1000;
  // check configuration ROM and firmware version
  info_.moduletype_ = readData<uint16_t>(baseAddress()+0x403C)&0xFF;
  info_.moduletype_ |= (readData<uint16_t>(baseAddress()+0x4038)&0xFF)<<8;
  info_.moduletype_ |= (readData<uint16_t>(baseAddress()+0x4034)&0xFF)<<16;
  assert(info_.moduletype_==0x04A6);
  info_.version_ = readData<uint16_t>(baseAddress()+0x4030)&0x1;
  info_.manufacturer_ = readData<uint16_t>(baseAddress()+0x402C)&0xFF;
  info_.manufacturer_ |= (readData<uint16_t>(baseAddress()+0x4028)&0xFF)<<8;
  info_.manufacturer_ |= (readData<uint16_t>(baseAddress()+0x4024)&0xFF)<<16;
  assert(info_.manufacturer_==0x40E6);
  // revision, serial number
  info_.serial_number_ = readData<uint16_t>(baseAddress())&0xFF;
  info_.serial_number_ |= (readData<uint16_t>(baseAddress()+0x4080)&0xFF)<<8;
  info_.revision_major_ = readData<uint16_t>(baseAddress()+0x404C)&0xFF;
  info_.revision_major_ |= (readData<uint16_t>(baseAddress()+0x4048)&0xFF)<<8;
  info_.revision_minor_ = readData<uint16_t>(baseAddress()+0x4044)&0xFF;
  info_.revision_minor_ |= (readData<uint16_t>(baseAddress()+0x4040)&0xFF)<<8;
  // firmware version
  info_.firmwareVersion_ = readData<uint16_t>(baseAddress()+0x1026)&0xFF;
  
  LOG_DEBUG("CAEN V1190"+ ((info_.version_&0x1) ? string("B") : string("A")) + " initialized. " + 
            "Serial number: " + int_to_hex(info_.serial_number_) + 
            " rev. " + to_string(info_.revision_major_) + "." + to_string(info_.revision_minor_) + 
            " fw. "  + to_string(info_.firmwareVersion_>>4) + "." + to_string(info_.firmwareVersion_&0xF));
}

V1190ControlRegister Tdc::getControlRegister(){
  return V1190ControlRegister(readData<uint16_t>(controlRegister_));
}

void Tdc::setControlRegister(V1190ControlRegister& reg){
  writeData(controlRegister_, reg.registr());
}

void Tdc::enableFIFO(bool enable) {
  V1190ControlRegister reg = getControlRegister();
  reg.setBit(V1190ControlRegister::cvFIFOEN,enable);
  setControlRegister(reg);
  if(enable) {
    LOG_INFO("FIFO enabled !") 
  } else {
    LOG_INFO("FIFO disabled !");
  }
}

void Tdc::enableBERR(bool enable) {
  V1190ControlRegister reg = getControlRegister();
  reg.setBit(V1190ControlRegister::cvBERREN,enable);
  setControlRegister(reg);
  if(enable) {
    LOG_INFO("BERR enabled !") 
  } else {
    LOG_INFO("BERR disabled !");
  }
}

void Tdc::enableExtdTrigTime(bool enable) {
  V1190ControlRegister reg = getControlRegister();
  reg.setBit(V1190ControlRegister::cvEXTDTTEN,enable);
  setControlRegister(reg);
  if(enable) { 
    LOG_INFO("Extended Trigger Time Tag enabled !") 
  } else {
    LOG_INFO("Extended Trigger Time Tag disabled !");
  }
}

void Tdc::enableCompensation(bool enable) {
  V1190ControlRegister reg = getControlRegister();
  reg.setBit(V1190ControlRegister::cvCOMPEN,enable);
  setControlRegister(reg);
  if(enable) {
    LOG_INFO("Compensation enabled !") 
  } else {
    LOG_INFO("Compensation disabled !");
  }
}

V1190StatusRegister Tdc::getStatus() {
  return V1190StatusRegister(readData<uint16_t>(statusRegister_));
}

void Tdc::setInterrupt(uint8_t level, uint16_t vector) {
  writeData(baseAddress()+0x100A, (uint16_t)level);
  writeData(baseAddress()+0x100C, vector);
}

void Tdc::reset(bool moduleReset, bool softClear, bool softEvtReset) {
  if(moduleReset) { writeData(baseAddress() +0x1014, 0); LOG_INFO("Module Reset"); }
  if(softClear) { writeData(baseAddress() +0x1016, 0); LOG_INFO("Software Clear"); }
  if(softEvtReset) { writeData(baseAddress() +0x1018, 0); LOG_INFO("Software Event Reset"); }
}

void Tdc::trigger() {
  writeData(baseAddress() +0x101A, 0);
  LOG_INFO("Software Trigger generated.");
}

uint32_t Tdc::eventCount() {
  return controller()->mode(cvA32_U_DATA,cvD32)->readData<uint32_t>(baseAddress() +0x101C);
}

uint16_t Tdc::storedEventCount() {
  return readData<uint16_t>(baseAddress() +0x1020);
}

void Tdc::setAlmostFullLevel(uint16_t level) {
  writeData(baseAddress() +0x1022,level);
  LOG_INFO("Set almost-full level to " + to_string(level));
}

uint16_t Tdc::getAlmostFullLevel() {
  return readData<uint16_t>(baseAddress() +0x1022);
}

std::pair<uint16_t,uint16_t> Tdc::readFIFO() {
  uint32_t data = controller()->mode(cvA32_U_DATA,cvD32)->readData<uint32_t>(baseAddress() +0x1038);
  return make_pair(data>>16,data&0xFFFF);
}

uint16_t Tdc::getFIFOCount() {
  return readData<uint16_t>(baseAddress() +0x103C)&0x7FF;
}

uint8_t Tdc::getFIFOStatus() {
  return readData<uint16_t>(baseAddress() +0x103E)&0x3;
}

V1190Event Tdc::getEvent(bool useFIFO) {
  waitDataReady();
  V1190Event event;
  TDCEvent tdc;
  uint32_t data;
  bool tdcHeadersEnabled = false;
  uint16_t eventId, nwords;
  if(useFIFO) {
    std::tie(eventId, nwords) = readFIFO();
  }
  // in D32 readout, read until we get to the trailer and fill progressively the event record
  for(uint16_t i=0; !(useFIFO&&i) || (i<nwords);++i) { 
    data = controller()->mode(cvA32_U_DATA,cvD32)->readData<uint32_t>(baseAddress());
    switch(data>>27) {
      case 0x8: // global header
        event = V1190Event(data);
        if(useFIFO && event.getEventCount()!=eventId)
          LOG_WARN("Event Count mismatch: Expected " + to_string(eventId) + " from FIFO but got " 
                   + to_string(event.getEventCount()) + " in the output buffer.");
        break;
      case 0x10: // global trailer
        event.setStatus((data>>24)&0x7);
        return event;
      case 0x11: // extended trigger time
        event.setExtdTriggerTime(data);
        break;
      case 0x1: // TDC header
        tdc = TDCEvent(data);
        tdcHeadersEnabled = true;
        break;
      case 0x0: // measurement
        if(tdcHeadersEnabled) {
          tdc.addHit(TDCHit(data));
        } else {
          event.addHit(TDCHit(data));
        }
        break;
      case 0x4: // TDC Error
        tdc.setErrorFlags(data);
        break;
      case 0x3: // TDC trailer
        event.addTDCEvent(tdc);
        break;
      case 0x18: // Filler... this should never happen.
        break;
    }
  }
  LOG_WARN("Read " + to_string(nwords) + " words from the output buffer without getting the Global Trailer.");
  return event;
}

std::vector<V1190Event> Tdc::getEvents(bool useFIFO) {
  waitDataReady();
  unsigned char* buffer = NULL;
  std::vector<V1190Event> output;
  std::vector<uint32_t> input;
  V1190Event event;
  TDCEvent tdc;
  bool tdcHeadersEnabled = false;
  uint32_t nwords = 256;
  int count = 0;
  // if FIFO is enabled: compute exact nwords
  if(useFIFO) {
    // Calculate Nw: number of words which compose the events
    nwords=0;
    uint16_t fifoCount = getFIFOCount();
    for(int i=0; i< fifoCount; ++i) {
      nwords += readFIFO().second;
    }
  }
  // read all
  bool done = false;
  while(!done) {
    std::vector<uint32_t> tmp; 
    // read n 32 bits words (from FIFO or default)
    try {
      tmp = controller()->mode(cvA32_U_BLT,cvD32)->blockReadData<uint32_t>(baseAddress(), nwords);
    } catch(CAENVMEexception &e) {
      // note: BERR stop condition should be avoided, since this implementation would discard the last BLT read.
      done = true;
    }
    // copy to the input vector
    input.insert(input.end(),tmp.begin(),tmp.end());
    // stop conditions: BERR (above), useFIFO (one BLT is enough), or buffer empty
    if(useFIFO) done = true;
    done = (!(readData<uint16_t>(this->statusRegister_)&0x1));
  }
  // then loop on the data retrieved and create events
  for(auto data : input) {
    switch(data>>27) {
      case 0x8: // global header
        event = V1190Event(data);
        tdcHeadersEnabled = false;
        break;
      case 0x10: // global trailer
        event.setStatus((data>>24)&0x7);
        output.push_back(event);
        break;
      case 0x11: // extended trigger time
        event.setExtdTriggerTime(data);
        break;
      case 0x1: // TDC header
        tdc = TDCEvent(data);
        tdcHeadersEnabled = true;
        break;
      case 0x0: // measurement
        if(tdcHeadersEnabled) {
          tdc.addHit(TDCHit(data));
        } else {
          event.addHit(TDCHit(data));
        }
        break;
      case 0x4: // TDC Error
        tdc.setErrorFlags(data);
        break;
      case 0x3: // TDC trailer
        event.addTDCEvent(tdc);
        break;
      case 0x18: // Filler... 
        break;
    }
  }
  return output;
}

TDCHit Tdc::getHit()
{
  waitDataReady();
  // read one word, interpret it as a hit and return the result
  return TDCHit(controller()->mode(cvA32_U_DATA,cvD32)->readData<uint32_t>(baseAddress()));
}

void Tdc::setAcquisitionMode(Tdc::CVAcquisitionMode mode) {
  writeOpcode((mode==cvTrigger ? 0x0000 :0x0100));
  LOG_DEBUG("Trigger Mode : " + to_string(mode));
}
  
Tdc::CVAcquisitionMode Tdc::getAcquisitionMode(){
  writeOpcode(0x0200);
  return CVAcquisitionMode(readOpcode()&0x1);
}
  
void Tdc::keepToken(bool keep) {
  writeOpcode((keep ? 0x0300 :0x0400));
  LOG_DEBUG("Keep Token : " + to_string(keep));
}
  
void Tdc::saveUserConfiguration() {
  writeOpcode(0x0600);
  LOG_DEBUG("User configuration saved.");
}
  
void Tdc::loadConfiguration(Tdc::CVConfiguration conf) {
  writeOpcode((conf==Tdc::cvDefault ? 0x0500 : 0x0700));
  LOG_DEBUG("Loaded " + std::string(conf==Tdc::cvDefault ? "default" : "user") + " configuration.");
}
  
void Tdc::setStartupConfiguration(Tdc::CVConfiguration conf) {
  writeOpcode((conf==cvDefault ? 0x0900 : 0x0800));
  LOG_DEBUG("Set auto load " + std::string(conf==cvDefault ? "default" : "user") + " configuration.");
}

void Tdc::setTriggerWindow(Tdc::WindowConfiguration &conf) {
  writeOpcode(0x1000);
  writeOpcode(conf.width);
  writeOpcode(0x1100);
  writeOpcode(conf.offset);
  writeOpcode(0x1200);
  writeOpcode(conf.extraMargin);
  writeOpcode(0x1300);
  writeOpcode(conf.rejectMargin);
  writeOpcode((conf.triggerTimeSubstraction ? 0x1400 : 0x1500));
  LOG_DEBUG("Trigger Window configured.");
}
  
Tdc::WindowConfiguration Tdc::getTriggerWindow() {
  writeOpcode(0x1600);
  Tdc::WindowConfiguration conf;
  conf.width = readOpcode();
  conf.offset = readOpcode();
  conf.extraMargin = readOpcode();
  conf.rejectMargin = readOpcode();
  conf.triggerTimeSubstraction = (readOpcode() & 0x1);
  return conf;
}

void Tdc::setEdgeDetectionConfiguration(Tdc::CVEdgeDetection conf){
  writeOpcode(0x2200);
  writeOpcode(conf);
}
  
Tdc::CVEdgeDetection Tdc::getEdgeDetectionConfiguration(){
  writeOpcode(0x2300);
  return Tdc::CVEdgeDetection(readOpcode()&0x3);
}
  
void Tdc::setEdgeLSB(Tdc::CVEdgeLSB lsb){
  writeOpcode(0x2400);
  writeOpcode(lsb);
}
  
void Tdc::setPairResolution(Tdc::CVPairModeEdgeLSB edge, Tdc::CVPairModeWidthLSB width){
  writeOpcode(0x2500);
  writeOpcode(uint16_t(edge) + (uint16_t(width)<<8));
}
  
Tdc::CVEdgeLSB Tdc::getResolution(){
  writeOpcode(0x2600);
  // assumes to be in leading/trailing edge mode
  return Tdc::CVEdgeLSB(readOpcode()&0x3);
}

std::pair<Tdc::CVPairModeEdgeLSB, Tdc::CVPairModeWidthLSB> Tdc::getPairResolution(){
  writeOpcode(0x2600);
  uint16_t opcode = readOpcode();
  // assumes to be in pair mode
  return make_pair(Tdc::CVPairModeEdgeLSB(opcode&0x7),Tdc::CVPairModeWidthLSB((opcode>>8)&0xF));
}
  
void Tdc::setDeadTime(Tdc::CVDeadTime dt){
  writeOpcode(0x2800);
  writeOpcode(dt);
}
  
Tdc::CVDeadTime Tdc::getDeadTime(){
  writeOpcode(0x2900);
  return Tdc::CVDeadTime(readOpcode()&0x3);
}

void Tdc::enableTDCHeader(bool enable){
  writeOpcode((enable? 0x3000 : 0x3100));
}
  
bool Tdc::isTDCHeaderEnabled(){
  writeOpcode(0x3200);
  return (readOpcode()&0x1);
}
  
void Tdc::setMaxHitsPerEvent(int numHits){
  writeOpcode(0x3300);
  uint16_t opcode;
  if(numHits<0) { opcode=9;}
  else if(numHits==0) { opcode=0; }
  else {
    // silently ignore wrong inputs... take the leading bit.
    numHits = uint8_t(numHits);
    opcode = 1; while(numHits >>=1) ++opcode;
  }
  writeOpcode(opcode);
}
  
int Tdc::getMaxHitsPerEvent(){
  writeOpcode(0x3400);
  uint16_t opcode = readOpcode()&0xF;
  if(opcode==0) return 0;
  if(opcode==9) return -1;
  return 1<<(opcode-1);
}
  
void Tdc::configureTDCReadout(bool enableErrorMask, bool enableBypass, uint16_t internalErrorTypes, uint16_t fifoSize){
  writeOpcode((enableErrorMask? 0x3500 : 0x3600));
  writeOpcode((enableBypass? 0x3700 : 0x3800));
  writeOpcode(0x3900);
  writeOpcode(internalErrorTypes);
  writeOpcode(0x3B00);
  writeOpcode(fifoSize);
}
  
uint16_t Tdc::getInternalErrorTypes(){
  writeOpcode(0x3A00);
  return readOpcode();
}
  
uint16_t Tdc::getFifoSize(){
  writeOpcode(0x3C00);
  return readOpcode();
}

void Tdc::enableChannel(uint8_t channel, bool enable){
  uint16_t opcode = (enable? 0x4000 : 0x4100)+(channel&0x7F);
  if(channel&128) opcode = (enable? 0x4200 : 0x4300);
  writeOpcode(opcode);
}
  
void Tdc::writeEnablePattern(std::bitset<128> &pattern){
  writeOpcode(0x4400);
  int nTDC = (info_.version_&0x1) ? 4 : 8;
  for (int i=0; i<nTDC; ++i) {
    writeOpcode((pattern>>(16*i)).to_ulong());  // shift and truncate
  }
}
  
std::bitset<128> Tdc::readEnablePattern(){
  writeOpcode(0x4500);
  std::bitset<128> pattern = 0;
  std::bitset<128> tmp;
  int nTDC = (info_.version_&0x1) ? 4 : 8;
  for (int i=0; i<nTDC; ++i) {
    tmp = readOpcode(); tmp <<=(16*i);
    pattern |= tmp;
  }
  return pattern;
}

/////  UTILITIES ///////

void Tdc::waitRead(void) {
  uint16_t data;
  do {
    data = readData<uint16_t>(this->microHandshake_);
  } while(!(data&0x2));
}

void Tdc::waitWrite(void) {
  uint16_t data;
  do {
    data = readData<uint16_t>(this->microHandshake_);
  } while(!(data&0x1));
}

void Tdc::waitDataReady(void) {
  uint16_t data;
  do {
    data = readData<uint16_t>(this->statusRegister_);
  } while(!(data&0x1));
}

void Tdc::writeOpcode(uint16_t data) {
  waitWrite();
  writeData(opcode_,data);
}

uint16_t Tdc::readOpcode()
{
  waitRead();
  return readData<uint16_t>(opcode_);
}
