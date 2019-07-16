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
  unsigned int DATA=0;
  while(1){
    readData(this->MicroHandshake,&DATA);
    if(DATA%4==3 || DATA%4==2){
      break;
    }
  }
}

void Tdc::waitWrite(void) {
  unsigned int DATA=0;
  while(1){
    readData(this->MicroHandshake,&DATA);
    if(DATA%2==1) {
      break;
    }
  }
}

void Tdc::waitDataReady(void) {
  unsigned int DATA=0;
  while(1){
    readData(this->StatusRegister,&DATA);
    if(DATA%2==1){
      break;
    }
  }
}
 
Event Tdc::getEvent()
{
    Event myEvent;
    //works only if FIFO enabled !
    this->waitDataReady(); 
    unsigned int DATA=0;
    controller()->mode(cvA32_U_DATA,cvD32)->readData(this->EventFIFO,&DATA);
    unsigned int eventNumberFIFO=digit(DATA,31,16);
    unsigned int numberOfWords=digit(DATA,15,0);
    vector<unsigned int> dataOutputBuffer;
    for(unsigned int i=numberOfWords; i>0 ;i--) {
      controller()->mode(cvA32_U_DATA,cvD32)->readData(baseAddress(),&DATA);
      dataOutputBuffer.vector::push_back(DATA);
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
    unsigned int DATA=0;
    waitRead();
    readData(StatusRegister,&DATA);
    
    if (DATA&1) {
      LOG_DEBUG("Event Ready");
    } else {
      LOG_DEBUG("No data ready");
    }
    if (DATA&4) {
      LOG_DEBUG("Output Buffer is Full");
    } else {
      LOG_DEBUG("Output Buffer is not full");
    }
    if (DATA&8) {
      LOG_DEBUG("Operating Mode : Trigger");
    } else {
      LOG_DEBUG("Operating Mode : Continuous");
    }
    return DATA;
}

void Tdc::Reset(){
    unsigned int DATA=0;
    int ADD = baseAddress() +0x1014;
    for(int k = 0; k<3; k++){
      ADD+=2;
      writeData(ADD, &DATA);
    }
    LOG_INFO(" Module Reset, Software Clear, Software Event Reset");
}

void Tdc::setMode(bool trig){
    unsigned int DATA=0;
    if(trig) DATA = 0x0000;
    else DATA =0x0100;
    writeOpcode(DATA);
    LOG_DEBUG("Trigger Mode : " +to_string(trig));
}

void Tdc::setMaxEvPerHit(int Max_ev_per_hit){
    for(int k=0; k<8;k++)
    if (Max_ev_per_hit== (2^(k))) {
      if (k == 8) LOG_INFO("No limit on Maximum number of hits per event");
      unsigned int DATA=0x3300; // MEPH = maximum events per hits
      writeOpcode(DATA);
      DATA = k+1;
      writeOpcode(DATA);
    } else if (Max_ev_per_hit==0) {
      unsigned int DATA=0x3300;
      writeOpcode(DATA);
      DATA=0;
      writeOpcode(DATA);
    }
    else
      LOG_WARN("Not a valid set  ! value of Max number of hits per event must be 0 or a power of 2 (1 2 4 .. 128) or 256 for NO LIMIT");
   }

void Tdc::setWindowWidth(unsigned int WidthSetting) {   
  if (WidthSetting > 4095 ) {
    LOG_WARN("Width Setting must be a integer in the range from 1 to 4095");
  } else {
    unsigned int DATA=0x1000;
    writeOpcode(DATA);
    DATA = WidthSetting;
    writeOpcode(DATA);
    LOG_DEBUG("Window Width set to"+to_string(WidthSetting));
  }
}

void Tdc::setWindowOffset(int OffsetSetting) {   
  if (OffsetSetting > 40 || OffsetSetting < -2048) {
    LOG_WARN("Offset Setting must be a integer in the range from -2048 to +40");
  } else {
    unsigned int DATA = 0x1100;
    writeOpcode(DATA);
    DATA = OffsetSetting;
    writeOpcode(DATA);
    LOG_DEBUG("Window Width set to" + to_string(OffsetSetting));
  }
}

void Tdc::setExSearchMargin(int ExSearchMrgnSetting ) {
  if (ExSearchMrgnSetting > 50) {
    LOG_WARN("50*25ns is the maximal value. Extra Search Margin Setting must be a integer in the range from 0 to 50");
  } else {
    unsigned int DATA = 0x1200;
    writeOpcode(DATA);
    DATA = ExSearchMrgnSetting;
    writeOpcode(DATA);
    LOG_DEBUG("Extra Search Margin Width set to"+to_string(ExSearchMrgnSetting));
  }
}

void Tdc::setRejectMargin(int RejectMrgnSetting) {
  if (RejectMrgnSetting > 4095) {
    LOG_WARN("Offset Setting must be a integer in the range from -2048 to +40");
  } else {
    unsigned int DATA = 0x1300;
    writeOpcode(DATA);
    DATA = RejectMrgnSetting;
    writeOpcode(DATA);
    LOG_DEBUG("Reject Margin set to"+to_string(RejectMrgnSetting));
  }
}

void Tdc::readWindowConfiguration() {
  std::stringstream stream;
  unsigned int DATA=0x1600;
  writeOpcode(DATA);
  readOpcode(DATA);
  stream <<" Match window width : "<<digit(DATA,11,0);
  readOpcode(DATA);
  stream << " Window ofset : "<<digit(DATA,11,0)-4096;
  readOpcode(DATA);
  stream << " Extra search window width: "<<digit(DATA,11,0);  
  readOpcode(DATA);
  stream <<" Reject margin width: "<<digit(DATA,11,0);
  readOpcode(DATA);
  stream << " Trigger time substraction : "<<digit(DATA,0) << std::endl;
  LOG_INFO(stream.str());
}

void Tdc::enableFIFO() {
  unsigned int DATA;
  readData(ControlRegister, &DATA);
  if (digit(DATA,8)==0) {
    DATA+=0x0100;
  }
  writeData(ControlRegister, &DATA);
  LOG_INFO("FIFO enabled !");
}
  
void Tdc::disableTDCHeaderAndTrailer() {
  unsigned int DATA = 0x3100;
  writeOpcode(DATA);
  DATA = 0x3200;
  writeOpcode(DATA);
  readOpcode(DATA);
  if (DATA%2==0) LOG_INFO("TDC Header and Trailer disabled");
}
  
void Tdc::readResolution() {
  unsigned int DATA=0x2600;
  writeOpcode(DATA);
  readOpcode(DATA);
  LOG_INFO("Resolution : " + to_string(digit(DATA,1,0)));
}
  
void Tdc::writeOpcode(unsigned int &DATA) {
  waitWrite();
  writeData(Opcode,&DATA);
}

void Tdc::readOpcode(unsigned int &DATA)
{
  waitRead();
  readData(Opcode,&DATA);
}
