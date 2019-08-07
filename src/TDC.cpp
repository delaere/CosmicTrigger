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


void TDCEvent::print() const {
  std::stringstream stream;
  stream << "Event number : " << eventNumber() << std::endl;
  stream << "Recorded events :" << std::endl; 
  for (auto measurement : measurements_) {
    stream << "Channel : " << measurement.first << " - Time : " << measurement.second << endl;
  }
  stream << endl;
  LOG_DATA_INFO(stream.str());
}

unsigned int digit(unsigned int data, int begin, int end) {
    if (begin<end){return -1;}
    else return (((1<<(begin+1))-1)&data)>>end;
}

unsigned int digit(unsigned int data, int position) {
    return (data>>position)&1;
}

Tdc::Tdc(VmeController* controller,int address):VmeBoard(controller, address, cvA32_U_DATA, cvD16, true) {
    Opcode=baseAddress()+0x102E;
    StatusRegister=baseAddress()+0x1002;
    MicroHandshake=baseAddress()+0x1030;
    OutputBuffer=baseAddress()+0x0000;
    EventFIFO=baseAddress()+0x1038;
    ControlRegister=baseAddress()+0x10;
}

TDCEvent Tdc::getEvent()
{
    TDCEvent myEvent;
    //works only if FIFO enabled !
    this->waitDataReady(); 
    unsigned int data=0;
    controller()->mode(cvA32_U_DATA,cvD32)->readData(this->EventFIFO,&data);
    unsigned int eventNumberFIFO=digit(data,31,16);
    unsigned int numberOfWords=digit(data,15,0);
    std::vector<unsigned int> dataOutputBuffer;
    for(unsigned int i=numberOfWords; i>0 ;i--) {
      controller()->mode(cvA32_U_DATA,cvD32)->readData(baseAddress(),&data);
      dataOutputBuffer.push_back(data);
    }
    if (!( eventNumberFIFO==digit(dataOutputBuffer[0],26,5) && digit(dataOutputBuffer[0],31,27)==8)) 
      throw_with_trace(CAENVMEexception(cvGenericError));
    myEvent.setEventNumber(eventNumberFIFO);
    for(unsigned int i=0; i<numberOfWords-1 ;i++) { // "-1" because last event is TRAILER
      if (digit(dataOutputBuffer[i],31,27)==0 ) {
        myEvent.addMeasurement({ digit(dataOutputBuffer[i],18,0) , digit(dataOutputBuffer[i],25,19) });
      }
    }
    myEvent.setTime(time(nullptr));
    return myEvent;
}

// Read the status with the
unsigned int Tdc::getStatus(){
    unsigned int data=0;
    waitRead();
    readData(StatusRegister,&data);
    
    if (data&1) {
      LOG_DEBUG("Event Ready");
    } else {
      LOG_DEBUG("No data ready");
    }
    if (data&4) {
      LOG_DEBUG("Output Buffer is Full");
    } else {
      LOG_DEBUG("Output Buffer is not full");
    }
    if (data&8) {
      LOG_DEBUG("Operating Mode : Trigger");
    } else {
      LOG_DEBUG("Operating Mode : Continuous");
    }
    return data;
}

void Tdc::Reset(){
    unsigned int data=0;
    int ADD = baseAddress() +0x1014;
    for(int k = 0; k<3; k++){
      ADD+=2;
      writeData(ADD, &data);
    }
    LOG_INFO(" Module Reset, Software Clear, Software Event Reset");
}

void Tdc::enableFIFO() {
  unsigned int data;
  readData(ControlRegister, &data);
  if (digit(data,8)==0) {
    data+=0x0100;
  }
  writeData(ControlRegister, &data);
  LOG_INFO("FIFO enabled !");
}


void Tdc::setAcquisitionMode(Tdc::CVAcquisitionMode mode) {
  uint16_t opcode = (mode==cvTrigger ? 0x0000 :0x0100);
  writeOpcode(opcode);
  LOG_DEBUG("Trigger Mode : " + to_string(mode));
}
  
Tdc::CVAcquisitionMode Tdc::getAcquisitionMode(){
  uint16_t opcode=0x0200;
  writeOpcode(opcode);
  readOpcode(opcode);
  return CVAcquisitionMode(opcode&0x1);
}
  
void Tdc::keepToken(bool keep) {
  uint16_t opcode = (keep ? 0x0300 :0x0400);
  writeOpcode(opcode);
  LOG_DEBUG("Keep Token : " + to_string(keep));
}
  
void Tdc::saveUserConfiguration() {
  uint16_t opcode=0x0600;
  writeOpcode(opcode);
  LOG_DEBUG("User configuration saved.");
}
  
void Tdc::LoadConfiguration(Tdc::CVConfiguration conf) {
  uint16_t opcode = (conf==Tdc::cvDefault ? 0x0500 : 0x0700);
  writeOpcode(opcode);
  LOG_DEBUG("Loaded " + std::string(conf==Tdc::cvDefault ? "default" : "user") + " configuration.");
}
  
void Tdc::setStartupConfiguration(Tdc::CVConfiguration conf) {
  uint16_t opcode = (conf==cvDefault ? 0x0900 : 0x0800);
  writeOpcode(opcode);
  LOG_DEBUG("Set auto load " + std::string(conf==cvDefault ? "default" : "user") + " configuration.");
}

void Tdc::setTriggerWindow(Tdc::WindowConfiguration &conf) {
  uint16_t opcode = 0x1000; writeOpcode(opcode);
  writeOpcode(conf.width);
  opcode = 0x1100; writeOpcode(opcode);
  writeOpcode(conf.offset);
  opcode = 0x1200; writeOpcode(opcode);
  writeOpcode(conf.extraMargin);
  opcode = 0x1300; writeOpcode(opcode);
  writeOpcode(conf.rejectMargin);
  opcode = (conf.triggerTimeSubstraction ? 0x1400 : 0x1500);
  writeOpcode(opcode);
  LOG_DEBUG("Trigger Window configured.");
}
  
Tdc::WindowConfiguration Tdc::getTriggerWindow() {
  uint16_t opcode = 0x1600;  writeOpcode(opcode);
  Tdc::WindowConfiguration conf;
  readOpcode(conf.width);
  readOpcode(conf.offset);
  readOpcode(conf.extraMargin);
  readOpcode(conf.rejectMargin);
  readOpcode(opcode); conf.triggerTimeSubstraction = (opcode & 0x1);
  return conf;
}

void Tdc::setEdgeDetectionConfiguration(Tdc::CVEdgeDetection conf){
  uint16_t opcode = 0x2200;  writeOpcode(opcode);
  opcode = (uint16_t)conf; writeOpcode(opcode);
}
  
Tdc::CVEdgeDetection Tdc::getEdgeDetectionConfiguration(){
  uint16_t opcode = 0x2300;  writeOpcode(opcode);
  readOpcode(opcode);
  return Tdc::CVEdgeDetection(opcode&0x3);
}
  
void Tdc::setEdgeLSB(Tdc::CVEdgeLSB lsb){
  uint16_t opcode = 0x2400;  writeOpcode(opcode);
  opcode = (uint16_t)lsb; writeOpcode(opcode);
}
  
void Tdc::setPairResolution(Tdc::CVPairModeEdgeLSB edge, Tdc::CVPairModeWidthLSB width){
  uint16_t opcode = 0x2500;  writeOpcode(opcode);
  opcode = uint16_t(edge) + (uint16_t(width)<<8); writeOpcode(opcode);
}
  
Tdc::CVEdgeLSB Tdc::getResolution(){
  uint16_t opcode = 0x2600;  writeOpcode(opcode);
  readOpcode(opcode);
  // assumes to be in leading/trailing edge mode
  return Tdc::CVEdgeLSB(opcode&0x3);
}

std::pair<Tdc::CVPairModeEdgeLSB, Tdc::CVPairModeWidthLSB> Tdc::getPairResolution(){
  uint16_t opcode = 0x2600;  writeOpcode(opcode);
  readOpcode(opcode);
  // assumes to be in pair mode
  return make_pair(Tdc::CVPairModeEdgeLSB(opcode&0x7),Tdc::CVPairModeWidthLSB((opcode>>8)&0xF));
}
  
void Tdc::setDeadTime(Tdc::CVDeadTime dt){
  uint16_t opcode = 0x2800;  writeOpcode(opcode);
  opcode = (uint16_t)dt; writeOpcode(opcode);
}
  
Tdc::CVDeadTime Tdc::getDeadTime(){
  uint16_t opcode = 0x2900;  writeOpcode(opcode);
  readOpcode(opcode);
  return Tdc::CVDeadTime(opcode&0x3);
}

void Tdc::enableTDCHeader(bool enable){
  uint16_t opcode = (enable? 0x3000 : 0x3100);
  writeOpcode(opcode);
}
  
bool Tdc::isTDCHeaderEnabled(){
  uint16_t opcode = 0x3200;  writeOpcode(opcode);
  readOpcode(opcode);
  return (opcode&0x1);
}
  
void Tdc::setMaxHitsPerEvent(int numHits){
  uint16_t opcode = 0x3300;  writeOpcode(opcode);
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
  uint16_t opcode = 0x3400;  writeOpcode(opcode);
  readOpcode(opcode); opcode &= 0xF;
  if(opcode==0) return 0;
  if(opcode==9) return -1;
  return 1<<(opcode-1);
}
  
void Tdc::configureTDCReadout(bool enableErrorMask, bool enableBypass, uint16_t internalErrorTypes, uint16_t fifoSize){
  uint16_t opcode = (enableErrorMask? 0x3500 : 0x3600); writeOpcode(opcode);
  opcode = (enableBypass? 0x3700 : 0x3800); writeOpcode(opcode);
  opcode = 0x3900; writeOpcode(opcode);
  opcode = internalErrorTypes; writeOpcode(opcode);
  opcode = 0x3B00; writeOpcode(opcode);
  opcode = fifoSize; writeOpcode(opcode);
}
  
uint16_t Tdc::getInternalErrorTypes(){
  uint16_t opcode = 0x3A00; writeOpcode(opcode);
  readOpcode(opcode);
  return opcode;
}
  
uint16_t Tdc::getFifoSize(){
  uint16_t opcode = 0x3C00; writeOpcode(opcode);
  readOpcode(opcode);
  return opcode;
}

void Tdc::enableChannel(uint8_t channel, bool enable){
  uint16_t opcode = (enable? 0x4000 : 0x4100)+(channel&0x7F);
  if(channel&128) opcode = (enable? 0x4200 : 0x4300);
  writeOpcode(opcode);
}
  
void Tdc::writeEnablePattern(std::bitset<128> &pattern){
  uint16_t opcode = 0x4400; writeOpcode(opcode);
  for (int i=0; i<8; ++i) {
    opcode = (pattern>>(16*i)).to_ulong(); // shift and truncate
    writeOpcode(opcode);
  }
}
  
std::bitset<128> Tdc::readEnablePattern(){
  uint16_t opcode = 0x4500; writeOpcode(opcode);
  std::bitset<128> pattern = 0;
  std::bitset<128> tmp = 0;
  for (int i=0; i<8; ++i) {
    readOpcode(opcode);
    tmp = opcode; tmp <<=(16*i);
    pattern |= tmp;
  }
  return pattern;
}

/////  UTILITIES ///////

void Tdc::waitRead(void) {
  uint16_t data = 0;
  do {
    readData(this->MicroHandshake,&data);
  } while(!(data&0x2));
}

void Tdc::waitWrite(void) {
  uint16_t data = 0;
  do {
    readData(this->MicroHandshake,&data);
  } while(!(data&0x1));
}

void Tdc::waitDataReady(void) {
  uint16_t data = 0;
  do {
    readData(this->StatusRegister,&data);
  } while(!(data&0x1));
}

void Tdc::writeOpcode(uint16_t &data) {
  waitWrite();
  writeData(Opcode,&data);
}

void Tdc::readOpcode(uint16_t &data)
{
  waitRead();
  readData(Opcode,&data);
}
