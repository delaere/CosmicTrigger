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

Tdc::Tdc(VmeController* controller,int address):VmeBoard(controller, address, cvA32_U_DATA, cvD16, true) {
    Opcode=baseAddress()+0x102E;
    StatusRegister=baseAddress()+0x1002;
    MicroHandshake=baseAddress()+0x1030;
    OutputBuffer=baseAddress()+0x0000;
    EventFIFO=baseAddress()+0x1038;
    ControlRegister=baseAddress()+0x10;
}

void Tdc::waitRead(void) {
  unsigned int data=0;
  while(1){
    readData(this->MicroHandshake,&data);
    if(data%4==3 || data%4==2){
      break;
    }
  }
}

void Tdc::waitWrite(void) {
  unsigned int data=0;
  while(1){
    readData(this->MicroHandshake,&data);
    if(data%2==1) {
      break;
    }
  }
}

void Tdc::waitDataReady(void) {
  unsigned int data=0;
  while(1){
    readData(this->StatusRegister,&data);
    if(data%2==1){
      break;
    }
  }
}
 
Event Tdc::getEvent()
{
    Event myEvent;
    //works only if FIFO enabled !
    this->waitDataReady(); 
    unsigned int data=0;
    controller()->mode(cvA32_U_DATA,cvD32)->readData(this->EventFIFO,&data);
    unsigned int eventNumberFIFO=digit(data,31,16);
    unsigned int numberOfWords=digit(data,15,0);
    vector<unsigned int> dataOutputBuffer;
    for(unsigned int i=numberOfWords; i>0 ;i--) {
      controller()->mode(cvA32_U_DATA,cvD32)->readData(baseAddress(),&data);
      dataOutputBuffer.vector::push_back(data);
    }
    if (!( eventNumberFIFO==digit(dataOutputBuffer[0],26,5) && digit(dataOutputBuffer[0],31,27)==8)) 
      throw_with_trace(CAENVMEexception(cvGenericError));
    myEvent.eventNumber=eventNumberFIFO;
    Hit temporaryHit;
    for(unsigned int i=0; i<numberOfWords-1 ;i++) { // "-1" because last event is TRAILER
      if (digit(dataOutputBuffer[i],31,27)==0 ) {
        temporaryHit.time=digit(dataOutputBuffer[i],18,0);
        temporaryHit.channel=digit(dataOutputBuffer[i],25,19);
        myEvent.measurements.vector::push_back(temporaryHit);
      }
    }
    time(&myEvent.time);
    return myEvent;
}

void Tdc::coutEvent(Event myEvent)
{  
  std::stringstream stream;
  stream << "Event number : " << myEvent.eventNumber << std::endl;
  stream << "Recorded events :" << std::endl; 
  for (unsigned int i=0; i<myEvent.measurements.vector::size(); i++) {
    stream << "Channel : "<<myEvent.measurements[i].channel<<" - Time : "<<myEvent.measurements[i].time<<endl;
  }
  stream << endl;
  LOG_INFO(stream.str());
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

void Tdc::setMode(bool trig){
    unsigned int data=0;
    if(trig) data = 0x0000;
    else data =0x0100;
    writeOpcode(data);
    LOG_DEBUG("Trigger Mode : " +to_string(trig));
}

void Tdc::setMaxEvPerHit(int Max_ev_per_hit){
    for(int k=0; k<8;k++)
    if (Max_ev_per_hit== (2^(k))) {
      if (k == 8) LOG_INFO("No limit on Maximum number of hits per event");
      unsigned int data=0x3300; // MEPH = maximum events per hits
      writeOpcode(data);
      data = k+1;
      writeOpcode(data);
    } else if (Max_ev_per_hit==0) {
      unsigned int data=0x3300;
      writeOpcode(data);
      data=0;
      writeOpcode(data);
    }
    else
      LOG_WARN("Not a valid set  ! value of Max number of hits per event must be 0 or a power of 2 (1 2 4 .. 128) or 256 for NO LIMIT");
   }

void Tdc::setWindowWidth(unsigned int WidthSetting) {   
  if (WidthSetting > 4095 ) {
    LOG_WARN("Width Setting must be a integer in the range from 1 to 4095");
  } else {
    unsigned int data=0x1000;
    writeOpcode(data);
    data = WidthSetting;
    writeOpcode(data);
    LOG_DEBUG("Window Width set to"+to_string(WidthSetting));
  }
}

void Tdc::setWindowOffset(int OffsetSetting) {   
  if (OffsetSetting > 40 || OffsetSetting < -2048) {
    LOG_WARN("Offset Setting must be a integer in the range from -2048 to +40");
  } else {
    unsigned int data = 0x1100;
    writeOpcode(data);
    data = OffsetSetting;
    writeOpcode(data);
    LOG_DEBUG("Window Width set to" + to_string(OffsetSetting));
  }
}

void Tdc::setExSearchMargin(int ExSearchMrgnSetting ) {
  if (ExSearchMrgnSetting > 50) {
    LOG_WARN("50*25ns is the maximal value. Extra Search Margin Setting must be a integer in the range from 0 to 50");
  } else {
    unsigned int data = 0x1200;
    writeOpcode(data);
    data = ExSearchMrgnSetting;
    writeOpcode(data);
    LOG_DEBUG("Extra Search Margin Width set to"+to_string(ExSearchMrgnSetting));
  }
}

void Tdc::setRejectMargin(int RejectMrgnSetting) {
  if (RejectMrgnSetting > 4095) {
    LOG_WARN("Offset Setting must be a integer in the range from -2048 to +40");
  } else {
    unsigned int data = 0x1300;
    writeOpcode(data);
    data = RejectMrgnSetting;
    writeOpcode(data);
    LOG_DEBUG("Reject Margin set to"+to_string(RejectMrgnSetting));
  }
}

void Tdc::readWindowConfiguration() {
  std::stringstream stream;
  unsigned int data=0x1600;
  writeOpcode(data);
  readOpcode(data);
  stream <<" Match window width : "<<digit(data,11,0);
  readOpcode(data);
  stream << " Window ofset : "<<digit(data,11,0)-4096;
  readOpcode(data);
  stream << " Extra search window width: "<<digit(data,11,0);  
  readOpcode(data);
  stream <<" Reject margin width: "<<digit(data,11,0);
  readOpcode(data);
  stream << " Trigger time substraction : "<<digit(data,0) << std::endl;
  LOG_INFO(stream.str());
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
  
void Tdc::disableTDCHeaderAndTrailer() {
  unsigned int data = 0x3100;
  writeOpcode(data);
  data = 0x3200;
  writeOpcode(data);
  readOpcode(data);
  if (data%2==0) LOG_INFO("TDC Header and Trailer disabled");
}
  
void Tdc::readResolution() {
  unsigned int data=0x2600;
  writeOpcode(data);
  readOpcode(data);
  LOG_INFO("Resolution : " + to_string(digit(data,1,0)));
}
  
void Tdc::writeOpcode(unsigned int &data) {
  waitWrite();
  writeData(Opcode,&data);
}

void Tdc::readOpcode(unsigned int &data)
{
  waitRead();
  readData(Opcode,&data);
}
