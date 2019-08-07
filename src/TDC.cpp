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
  opcode_=baseAddress()+0x102E;
  statusRegister_=baseAddress()+0x1002;
  microHandshake_=baseAddress()+0x1030;
  outputBuffer_=baseAddress()+0x0000;
  eventFIFO_=baseAddress()+0x1038;
  controlRegister_=baseAddress()+0x10;
  // check configuration ROM and firmware version
  uint16_t data;
  readData(baseAddress()+0x403C, &data);
  info_.moduletype_ = data;
  readData(baseAddress()+0x4038, &data);
  info_.moduletype_ += (data<<16);
  readData(baseAddress()+0x4034, &data);
  info_.moduletype_ += (data<<32);
  assert(info_.moduletype_==0x04A6);
  readData(baseAddress()+0x4030, &data);
  info_.version_ = data&0x1;
  readData(baseAddress()+0x402C, &data);
  info_.manufacturer_ = data;
  readData(baseAddress()+0x4028, &data);
  info_.manufacturer_ += (data<<16);
  readData(baseAddress()+0x4024, &data);
  info_.manufacturer_ += (data<<32);
  assert(info_.manufacturer_==0x40E6);
  // revision, serial number
  readData(baseAddress()+0x4084, &data);
  info_.serial_number_ = data;
  readData(baseAddress()+0x4080, &data);
  info_.serial_number_ += (data<<16);
  readData(baseAddress()+0x404C, &data);
  info_.revision_major_ = data;
  readData(baseAddress()+0x4048, &data);
  info_.revision_major_ += (data<<16);
  readData(baseAddress()+0x4044, &data);
  info_.revision_minor_ = data;
  readData(baseAddress()+0x4040, &data);
  info_.revision_minor_ += (data<<16);
  // firmware version
  readData(baseAddress()+0x1026, &data);
  info_.firmwareVersion_ = data&0xFF;
  
  LOG_DEBUG("CAEN V1190"+ (info_.version_&0x1 ? string("B") : string("A")) + " initialized. " + 
            int_to_hex(info_.manufacturer_) + " " + int_to_hex(info_.moduletype_) + " " + int_to_hex(info_.serial_number_) + 
            " rev. " + to_string(info_.revision_major_) + "." + to_string(info_.revision_minor_) + 
            " fw. "  + to_string(info_.firmwareVersion_>>4) + "." + to_string(info_.firmwareVersion_&0xF));
}

V1190ControlRegister Tdc::getControlRegister(){
  uint16_t data;
  readData(controlRegister_, &data);
  return V1190ControlRegister(data);
}

void Tdc::setControlRegister(V1190ControlRegister& reg){
  uint16_t data = reg.register();
  writeData(controlRegister_, &data);
}

void Tdc::enableFIFO(bool enable) {
  V1190ControlRegister reg = getControlRegister().setBit(V1190ControlRegister::cvFIFOEN,enable);
  setControlRegister(reg);
  if(enable) LOG_INFO("FIFO enabled !") else LOG_INFO("FIFO disabled !");
}

void Tdc::enableBERR(bool enable) {
  V1190ControlRegister reg = getControlRegister().setBit(V1190ControlRegister::cvBERREN,enable);
  setControlRegister(reg);
  if(enable) LOG_INFO("BERR enabled !") else LOG_INFO("BERR disabled !");
}

void Tdc::enableExtdTrigTime(bool enable) {
    V1190ControlRegister reg = getControlRegister().setBit(V1190ControlRegister::cvEXTDTTEN,enable);
  setControlRegister(reg);
  if(enable) LOG_INFO("Extended Trigger Time Tag enabled !") else LOG_INFO("Extended Trigger Time Tag disabled !");
}

void Tdc::enableCompensation(bool enable) {
  V1190ControlRegister reg = getControlRegister().setBit(V1190ControlRegister::cvCOMPEN,enable);
  setControlRegister(reg);
  if(enable) LOG_INFO("Compensation enabled !") else LOG_INFO("Compensation disabled !");
}

V1190StatusRegister Tdc::getStatus() {
  uint16_t data;
  readData(statusRegister_,&data);
  return V1190StatusRegister(data);

void Tdc::setInterrupt(uint8_t level, uint16_t vector) {
  uint16_t data = level;
  writeData(baseAddress()+0x100A, &data);
  data = vector;
  writeData(baseAddress()+0x100C, &data);
}

void Tdc::Reset(bool moduleReset=true, bool softClear=true, softEvtReset=true) {
  uint16_t data = 0;
  if(moduleReset) { writeData(baseAddress() +0x1014, &data); LOG_INFO("Module Reset"); }
  if(softClear) { writeData(baseAddress() +0x1016, &data); LOG_INFO("Software Clear"); }
  if(softEvtReset) { writeData(baseAddress() +0x1018, &data); LOG_INFO("Software Event Reset"); }
}

void Tdc::trigger() {
  uint16_t data = 0;
  writeData(baseAddress() +0x101A, &data);
  LOG_INFO("Software Trigger generated.");
}

uint32_t eventCount() {
  uint32_t data = 0;
  controller()->mode(cvA32_U_DATA,cvD32)->readData(baseAddress() +0x101C,&data);
  return data;
}

uint16_t storedEventCount() {
  uint16_t data = 0;
  readData(baseAddress() +0x1020,&data);
  return data;
}

void setAlmostFullLevel(uint16_t level) {
  uint16_t data = level;
  writeData(baseAddress() +0x1022,&data);
  LOG_INFO("Set almost-full level to " + to_string(level));
}

uint16_t getAlmostFullLevel() {
  uint16_t data = 0;
  readData(baseAddress() +0x1022,&data);
  return data;
}

std::pair<uint16_t,uint16_t> readFIFO() {
  uint32_t data = 0;
  controller()->mode(cvA32_U_DATA,cvD32)->readData(baseAddress() +0x1038,&data);
  return make_pair(data>>16,data&0xFFFF);
}

uint16_t getFIFOCount() {
  uint16_t data = 0;
  readData(baseAddress() +0x103C,&data);
  return data&0x7FF;
}

uint8_t getFIFOStatus() {
  uint16_t data = 0;
  readData(baseAddress() +0x103E,&data);
  return data&0x3;
}

TDCEvent Tdc::getEvent()
{
    TDCEvent myEvent;
    //works only if FIFO enabled !
    this->waitDataReady(); 
    unsigned int data=0;
    controller()->mode(cvA32_U_DATA,cvD32)->readData(this->eventFIFO_,&data);
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

TDCHit getHit()
{
  
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
  int nTDC = info_.version_&0x1 ? 4 : 8;
  for (int i=0; i<nTDC; ++i) {
    opcode = (pattern>>(16*i)).to_ulong(); // shift and truncate
    writeOpcode(opcode);
  }
}
  
std::bitset<128> Tdc::readEnablePattern(){
  uint16_t opcode = 0x4500; writeOpcode(opcode);
  std::bitset<128> pattern = 0;
  std::bitset<128> tmp = 0;
  int nTDC = info_.version_&0x1 ? 4 : 8;
  for (int i=0; i<nTDC; ++i) {
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
    readData(this->microHandshake_,&data);
  } while(!(data&0x2));
}

void Tdc::waitWrite(void) {
  uint16_t data = 0;
  do {
    readData(this->microHandshake_,&data);
  } while(!(data&0x1));
}

void Tdc::waitDataReady(void) {
  uint16_t data = 0;
  do {
    readData(this->statusRegister_,&data);
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
