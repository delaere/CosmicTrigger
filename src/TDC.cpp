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
#include "PythonModule.h"

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
  info_.serial_number_ = readData<uint16_t>(baseAddress()+0x4084)&0xFF;
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
    LOG_INFO("FIFO enabled !");
  } else {
    LOG_INFO("FIFO disabled !");
  }
}

void Tdc::enableBERR(bool enable) {
  V1190ControlRegister reg = getControlRegister();
  reg.setBit(V1190ControlRegister::cvBERREN,enable);
  setControlRegister(reg);
  if(enable) {
    LOG_INFO("BERR enabled !");
  } else {
    LOG_INFO("BERR disabled !");
  }
}

void Tdc::enableExtdTrigTime(bool enable) {
  V1190ControlRegister reg = getControlRegister();
  reg.setBit(V1190ControlRegister::cvEXTDTTEN,enable);
  setControlRegister(reg);
  if(enable) {
    LOG_INFO("Extended Trigger Time Tag enabled !");
  } else {
    LOG_INFO("Extended Trigger Time Tag disabled !");
  }
}

void Tdc::enableCompensation(bool enable) {
  V1190ControlRegister reg = getControlRegister();
  reg.setBit(V1190ControlRegister::cvCOMPEN,enable);
  setControlRegister(reg);
  if(enable) {
    LOG_INFO("Compensation enabled !");
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

using namespace boost::python;

template<> void exposeToPython<TDCHit>() {
  class_<TDCHit>("TDCHit")
    .add_property("channel",&TDCHit::getChannel, &TDCHit::setChannel)
    .add_property("type",&TDCHit::getType, &TDCHit::setType)
    .add_property("measurement",&TDCHit::getMeasurement, &TDCHit::setMeasurement)
    .def("toString",&TDCHit::toString)
  ;
  class_<std::vector<TDCHit> >("vec_hit")
    .def(vector_indexing_suite<std::vector<TDCHit> >())
    .def("__iter__", boost::python::iterator<std::vector<TDCHit> >())
  ;
}

template<> void exposeToPython<TDCEvent>() {
  class_<TDCEvent>("TDCEvent")
    .add_property("tdcId", &TDCEvent::getTDCId, &TDCEvent::setTDCId)
    .add_property("eventId", &TDCEvent::getEventId, &TDCEvent::setEventId)
    .add_property("bunchId", &TDCEvent::getBunchId, &TDCEvent::setBunchId)
    .add_property("errorFlags", &TDCEvent::getErrorFlags, &TDCEvent::setErrorFlags)
    .def("getHits", &TDCEvent::getHits,return_value_policy<copy_const_reference>())
    .def("addHit", &TDCEvent::addHit)
    .def("toString", &TDCEvent::toString)
  ;
  class_<std::vector<TDCEvent> >("vec_tdc")
    .def(vector_indexing_suite<std::vector<TDCEvent> >())
    .def("__iter__", boost::python::iterator<std::vector<TDCEvent> >())
  ;
}

template<> void exposeToPython<V1190Event>() {
  class_<V1190Event>("V1190Event")
    .add_property("eventCount", &V1190Event::getEventCount, &V1190Event::setEventCount)
    .add_property("geo", &V1190Event::getGeo, &V1190Event::setGeo)
    .add_property("status", &V1190Event::getStatus, &V1190Event::setStatus)
    .add_property("extdTriggerTime", &V1190Event::getExtdTriggerTime, &V1190Event::setExtdTriggerTime)
    .def("getHits",&V1190Event::getHits,return_value_policy<copy_const_reference>())
    .def("getTDCEvents",&V1190Event::getTDCEvents,return_value_policy<copy_const_reference>())
    .def("addHit",&V1190Event::addHit)
    .def("addTDCEvent",&V1190Event::addTDCEvent)
    .def("toString",&V1190Event::toString)
  ;
  class_<std::vector<V1190Event> >("vec_v1190event")
    .def(vector_indexing_suite<std::vector<V1190Event> >())
    .def("__iter__", boost::python::iterator<std::vector<V1190Event> >())
  ;
}

template<> void exposeToPython<V1190ControlRegister>() {
  scope in_V1190ControlRegister = class_<V1190ControlRegister>("V1190ControlRegister",init<uint32_t>())
     .add_property("registr",&V1190ControlRegister::registr)
     .add_property("bit",&V1190ControlRegister::bit,&V1190ControlRegister::setBit)
  ;

  enum_<V1190ControlRegister::CVRegisterWordBit>("CVRegisterWordBit")
    .value("cvBERREN", V1190ControlRegister::cvBERREN)
    .value("cvTERM", V1190ControlRegister::cvTERM)
    .value("cvTERM_SW", V1190ControlRegister::cvTERM_SW)
    .value("cvEMPTYEVT", V1190ControlRegister::cvEMPTYEVT)
    .value("cvALIGN64", V1190ControlRegister::cvALIGN64)
    .value("cvCOMPEN", V1190ControlRegister::cvCOMPEN)
    .value("cvTESTEN", V1190ControlRegister::cvTESTEN)
    .value("cvRCSEN", V1190ControlRegister::cvRCSEN)
    .value("cvFIFOEN", V1190ControlRegister::cvFIFOEN)
    .value("cvEXTDTTEN", V1190ControlRegister::cvEXTDTTEN)
    .value("cv16MBMEB", V1190ControlRegister::cv16MBMEB)
  ;
}

template<> void exposeToPython<V1190StatusRegister>() {
  scope in_V1190StatusRegister = class_<V1190StatusRegister>("V1190StatusRegister",init<uint32_t>())
    .add_property("registr",&V1190StatusRegister::registr)
    .add_property("bit",&V1190StatusRegister::bit)
  ;

  enum_<V1190StatusRegister::CVRegisterWordBit>("CVRegisterWordBit")
    .value("cvDATA_READY", V1190StatusRegister::cvDATA_READY)
    .value("cvALM_FULL", V1190StatusRegister::cvALM_FULL)
    .value("cvFULL", V1190StatusRegister::cvFULL)
    .value("cvTRG_MATCH", V1190StatusRegister::cvTRG_MATCH)
    .value("cvHEADER_EN", V1190StatusRegister::cvHEADER_EN)
    .value("cvTERM_ON", V1190StatusRegister::cvTERM_ON)
    .value("cvERROR0", V1190StatusRegister::cvERROR0)
    .value("cvERROR1", V1190StatusRegister::cvERROR1)
    .value("cvERROR2", V1190StatusRegister::cvERROR2)
    .value("cvERROR3", V1190StatusRegister::cvERROR3)
    .value("cvBERR_FLAG", V1190StatusRegister::cvBERR_FLAG)
    .value("cvPURG", V1190StatusRegister::cvPURG)
    .value("cvRES_0", V1190StatusRegister::cvRES_0)
    .value("cvRES_1", V1190StatusRegister::cvRES_1)
    .value("cvPAIR", V1190StatusRegister::cvPAIR)
    .value("cvTRIGLOST", V1190StatusRegister::cvTRIGLOST)
  ;
}

template<> void exposeToPython<Tdc>() {
  scope in_Tdc = class_<Tdc>("Tdc",init<VmeController*,uint32_t>())
    .def("getModuleInfo", &Tdc::getModuleInfo)
    .def("getControlRegister", &Tdc::getControlRegister)
    .def("setControlRegister", &Tdc::setControlRegister)
    .def("enableFIFO", &Tdc::enableFIFO)
    .def("enableBERR", &Tdc::enableBERR)
    .def("enableExtdTrigTime", &Tdc::enableExtdTrigTime)
    .def("enableCompensation", &Tdc::enableCompensation)
    .def("getStatus", &Tdc::getStatus)
    .def("setInterrupt", &Tdc::setInterrupt)
    .def("reset", &Tdc::reset)
    .def("trigger", &Tdc::trigger)
    .def("eventCount", &Tdc::eventCount)
    .def("storedEventCount", &Tdc::storedEventCount)
    .def("setAlmostFullLevel", &Tdc::setAlmostFullLevel)
    .def("getAlmostFullLevel", &Tdc::getAlmostFullLevel)
    .def("readFIFO", &Tdc::readFIFO)
    .def("getFIFOCount", &Tdc::getFIFOCount)
    .def("getFIFOStatus", &Tdc::getFIFOStatus)
    .def("getEvent", &Tdc::getEvent)
    .def("getEvents", &Tdc::getEvents)
    .def("getHit", &Tdc::getHit)
    .def("writeOpcode", &Tdc::writeOpcode)
    .def("readOpcode", &Tdc::readOpcode)
    .def("setAcquisitionMode", &Tdc::setAcquisitionMode)
    .def("getAcquisitionMode", &Tdc::getAcquisitionMode)
    .def("keepToken", &Tdc::keepToken)
    .def("saveUserConfiguration", &Tdc::saveUserConfiguration)
    .def("loadConfiguration", &Tdc::loadConfiguration)
    .def("setStartupConfiguration", &Tdc::setStartupConfiguration)
    .def("setTriggerWindow", &Tdc::setTriggerWindow)
    .def("getTriggerWindow", &Tdc::getTriggerWindow)
    .def("setEdgeDetectionConfiguration", &Tdc::setEdgeDetectionConfiguration)
    .def("getEdgeDetectionConfiguration", &Tdc::getEdgeDetectionConfiguration)
    .def("setEdgeLSB", &Tdc::setEdgeLSB)
    .def("setPairResolution", &Tdc::setPairResolution)
    .def("getResolution", &Tdc::getResolution)
    .def("setDeadTime", &Tdc::setDeadTime)
    .def("getDeadTime", &Tdc::getDeadTime)
    .def("enableTDCHeader", &Tdc::enableTDCHeader)
    .def("isTDCHeaderEnabled", &Tdc::isTDCHeaderEnabled)
    .def("setMaxHitsPerEvent", &Tdc::setMaxHitsPerEvent)
    .def("getMaxHitsPerEvent", &Tdc::getMaxHitsPerEvent)
    .def("configureTDCReadout", &Tdc::configureTDCReadout)
    .def("getInternalErrorTypes", &Tdc::getInternalErrorTypes)
    .def("getFifoSize", &Tdc::getFifoSize)
    .def("enableChannel", &Tdc::enableChannel)
  ;
  class_<Tdc::ModuleInfo>("ModuleInfo")
    .def_readwrite("manufacturer", &Tdc::ModuleInfo::manufacturer_)
    .def_readwrite("moduletype", &Tdc::ModuleInfo::moduletype_)
    .def_readwrite("version", &Tdc::ModuleInfo::version_)
    .def_readwrite("serial_number", &Tdc::ModuleInfo::serial_number_)
    .def_readwrite("revision_minor", &Tdc::ModuleInfo::revision_minor_)
    .def_readwrite("revision_major", &Tdc::ModuleInfo::revision_major_)
    .def_readwrite("firmwareVersion", &Tdc::ModuleInfo::firmwareVersion_)
  ;
  class_<Tdc::WindowConfiguration>("WindowConfiguration")
    .def_readwrite("width", &Tdc::WindowConfiguration::width)
    .def_readwrite("offset", &Tdc::WindowConfiguration::offset)
    .def_readwrite("extraMargin", &Tdc::WindowConfiguration::extraMargin)
    .def_readwrite("rejectMargin", &Tdc::WindowConfiguration::rejectMargin)
    .def_readwrite("triggerTimeSubstraction", &Tdc::WindowConfiguration::triggerTimeSubstraction)
    .def("computeOffset",&Tdc::WindowConfiguration::computeOffset)
    .staticmethod("computeOffset")
  ;
  enum_<Tdc::CVAcquisitionMode>("CVAcquisitionMode")
    .value("cvContinuous", Tdc::CVAcquisitionMode::cvContinuous)
    .value("cvTrigger", Tdc::CVAcquisitionMode::cvTrigger)
  ;
  enum_<Tdc::CVConfiguration>("CVConfiguration")
    .value("cvDefault", Tdc::CVConfiguration::cvDefault)
    .value("cvUser", Tdc::CVConfiguration::cvUser)
  ;
  enum_<Tdc::CVEdgeDetection>("CVEdgeDetection")
    .value("cvPairMode", Tdc::CVEdgeDetection::cvPairMode)
    .value("cvTrailing", Tdc::CVEdgeDetection::cvTrailing)
    .value("cvLeading", Tdc::CVEdgeDetection::cvLeading)
    .value("cvBoth", Tdc::CVEdgeDetection::cvBoth)
  ;
  enum_<Tdc::CVEdgeLSB>("CVEdgeLSB")
    .value("cv800ps", Tdc::CVEdgeLSB::cv800ps)
    .value("cv200ps", Tdc::CVEdgeLSB::cv200ps)
    .value("cv100ps", Tdc::CVEdgeLSB::cv100ps)
  ;
  enum_<Tdc::CVPairModeEdgeLSB>("CVPairModeEdgeLSB")
    .value("cvpme100ps", Tdc::CVPairModeEdgeLSB::cvpme100ps)
    .value("cvpme200ps", Tdc::CVPairModeEdgeLSB::cvpme200ps)
    .value("cvpme400ps", Tdc::CVPairModeEdgeLSB::cvpme400ps)
    .value("cvpme800ps", Tdc::CVPairModeEdgeLSB::cvpme800ps)
    .value("cvpme1600ps", Tdc::CVPairModeEdgeLSB::cvpme1600ps)
    .value("cvpme3120ps", Tdc::CVPairModeEdgeLSB::cvpme3120ps)
    .value("cvpme6250ps", Tdc::CVPairModeEdgeLSB::cvpme6250ps)
    .value("cvpme12500ps", Tdc::CVPairModeEdgeLSB::cvpme12500ps)
  ;
  enum_<Tdc::CVPairModeWidthLSB>("CVPairModeWidthLSB")
    .value("cvpmw100ps", Tdc::CVPairModeWidthLSB::cvpmw100ps)
    .value("cvpmw200ps", Tdc::CVPairModeWidthLSB::cvpmw200ps)
    .value("cvpmw400ps", Tdc::CVPairModeWidthLSB::cvpmw400ps)
    .value("cvpmw800ps", Tdc::CVPairModeWidthLSB::cvpmw800ps)
    .value("cvpmw1600ps", Tdc::CVPairModeWidthLSB::cvpmw1600ps)
    .value("cvpmw3200ps", Tdc::CVPairModeWidthLSB::cvpmw3200ps)
    .value("cvpmw6250ps", Tdc::CVPairModeWidthLSB::cvpmw6250ps)
    .value("cvpmw12500ps", Tdc::CVPairModeWidthLSB::cvpmw12500ps)
    .value("cvpmw25ns", Tdc::CVPairModeWidthLSB::cvpmw25ns)
    .value("cvpmw50ns", Tdc::CVPairModeWidthLSB::cvpmw50ns)
    .value("cvpmw100ns", Tdc::CVPairModeWidthLSB::cvpmw100ns)
    .value("cvpmw200ns", Tdc::CVPairModeWidthLSB::cvpmw200ns)
    .value("cvpmw400ns", Tdc::CVPairModeWidthLSB::cvpmw400ns)
    .value("cvpmw800ns", Tdc::CVPairModeWidthLSB::cvpmw800ns)
  ;
  enum_<Tdc::CVDeadTime>("CVDeadTime")
    .value("cvdt5ns", Tdc::CVDeadTime::cvdt5ns)
    .value("cvdt10ns", Tdc::CVDeadTime::cvdt10ns)
    .value("cvdt30ns", Tdc::CVDeadTime::cvdt30ns)
    .value("cvdt100ns", Tdc::CVDeadTime::cvdt100ns)
  ;
  class_<std::pair<uint16_t,uint16_t> >("FIFO")
    .def_readwrite("first", &std::pair<uint16_t,uint16_t>::first)
    .def_readwrite("second", &std::pair<uint16_t,uint16_t>::second)
  ;
}
