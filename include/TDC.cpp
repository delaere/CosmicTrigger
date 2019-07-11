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

tdc::tdc(vmeController* controller,int address):vmeBoard(controller,cvA32_U_DATA,cvD16){
    this->add=address;
    Opcode=add+0x102E;
    StatusRegister=add+0x1002;
    MicroHandshake=add+0x1030;
    OutputBuffer=add+0x0000;
    EventFIFO=add+0x1038;
    ControlRegister=add+0x10;
}

int tdc::waitRead(void)
{
    unsigned int DATA=0;
    try {
      while(1){
        readData(this->MicroHandshake,&DATA,cvA32_U_DATA,cvD16);
        if(DATA%4==3 || DATA%4==2){
          break;
        }
      }
    } catch (CAENVMEexception &e) {
      std::cout << "TDC: " << e.what() << " while wait_read." << std::endl;
    }
    return 0;
}

int tdc::waitWrite(void)
{
  unsigned int DATA=0;
  try {
    while(1){
      readData(this->MicroHandshake,&DATA,cvA32_U_DATA,cvD16);
      if(DATA%2==1) {
        break;
      }
    }
  } catch (CAENVMEexception &e) {
      std::cout << "TDC: " << e.what() << " while wait_write." << std::endl;
  }
   return 0;
}

int tdc::waitDataReady(void)
{
  unsigned int DATA=0;
  try {
    while(1){
      readData(this->StatusRegister,&DATA,cvA32_U_DATA,cvD16);
      if(DATA%2==1){
        break;
      }
    }
  } catch (CAENVMEexception &e) {
      std::cout << "TDC: " << e.what() << " while wait_data." << std::endl;
  }
  return 0;
}
 
int tdc::getEvent(event &myEvent)
{
    //works only if FIFO enabled !
    this->waitDataReady(); 
    unsigned int DATA=0;
    try {
      readData(this->EventFIFO,&DATA,cvA32_U_DATA,cvD32);
      unsigned int eventNumberFIFO=digit(DATA,31,16);
      unsigned int numberOfWords=digit(DATA,15,0);
      vector<unsigned int> dataOutputBuffer;
      for(unsigned int i=numberOfWords; i>0 ;i--) {
        readData(this->add,&DATA,cvA32_U_DATA,cvD32);
        dataOutputBuffer.vector::push_back(DATA);
      }
      if (!( eventNumberFIFO==digit(dataOutputBuffer[0],26,5) && digit(dataOutputBuffer[0],31,27)==8)) return -1;
      myEvent.eventNumber=eventNumberFIFO;
      hit temporaryHit;
      for(unsigned int i=0; i<numberOfWords-1 ;i++) { // "-1" because last event is TRAILER
        if (digit(dataOutputBuffer[i],31,27)==0 ) {
          temporaryHit.time=digit(dataOutputBuffer[i],18,0);
          temporaryHit.channel=digit(dataOutputBuffer[i],25,19);
          myEvent.measurements.vector::push_back(temporaryHit);
        }
      }
      time(&myEvent.time);
    } catch (CAENVMEexception &e) {
      std::cout << "TDC: " << e.what() << " while reading data." << std::endl;
      return -1;
    }
    return 0;
}

void tdc::analyseEvent(event myEvent, string filename)
{
    //Getting trigger time
    unsigned int triggerTime=0;
    for(unsigned int i=0; i<myEvent.measurements.vector::size();i++) {
        if (myEvent.measurements[i].channel==TriggerChannelNumber) {
          triggerTime=myEvent.measurements[i].time;
          break;
        }
    }
    
    //Getting closest clock
    unsigned int nextClock=0;
    for(unsigned int i=0; i<myEvent.measurements.vector::size();i++) {
      if (myEvent.measurements[i].channel==ClockChannelNumber) {
        if (myEvent.measurements[i].time>triggerTime){nextClock=myEvent.measurements[i].time;break;}
      }
    }
    
    float phase=nextClock-triggerTime;
    
    //completer ICI !
    stringstream mystream;
    mystream<<"echo '"<<asctime(localtime(&myEvent.time))<<" - "<<phase<<"'>> eventsLong.txt"<<endl;
    system(mystream.str().c_str());
    mystream.str("");
    mystream<<"echo '"<<phase<<"'>> eventsShort.txt"<<endl;
    system(mystream.str().c_str());
    
    //Getting all clock ticks
    unsigned int previousClock=0;
    for(unsigned int i=0; i<myEvent.measurements.vector::size();i++) {
      if (myEvent.measurements[i].channel==ClockChannelNumber) {
        if(previousClock!=0){
          mystream.str("");
          mystream<<"echo '"<<myEvent.measurements[i].time-previousClock<<" - Event "<<i<<"'>> clocksLong.txt"<<endl;
          system(mystream.str().c_str());
          mystream.str("");
          mystream<<"echo '"<<myEvent.measurements[i].time-previousClock<<"'>> clocksShort.txt"<<endl;
          system(mystream.str().c_str());
        }
        previousClock=myEvent.measurements[i].time;
      }
    }
}

void tdc::coutEvent(event myEvent)
{  
  if(vLevel(NORMAL))cout<<"Event number : "<<myEvent.eventNumber<<endl;
  if(vLevel(NORMAL))cout<<"Recorded events :"<<endl;
  for (unsigned int i=0; i<myEvent.measurements.vector::size(); i++) {
    if(vLevel(NORMAL))cout<<"Channel : "<<myEvent.measurements[i].channel<<" - Time : "<<myEvent.measurements[i].time<<endl;
  }
  cout<<endl;
}

// Read the status with the
void tdc::ReadStatus(){
    unsigned int DATA=0;
    waitRead();
    try { 
      readData(StatusRegister,&DATA);
    } catch (CAENVMEexception &e) {
      std::cout << "TDC: " << e.what() << " while reading status." << std::endl;
    }
    if (DATA%2 > 0){ if(vLevel(NORMAL))cout << "Event Ready"<<endl;}
    else {if(vLevel(NORMAL))cout<< "No data ready"<<endl;}
    if (DATA%8 >3) { if(vLevel(NORMAL))cout<< " Output Buffer is Full"<< endl;}
    else {if(vLevel(NORMAL))cout<< " Output Buffer is not full"<<endl;}
    if (DATA%16 >7 ){if(vLevel(NORMAL))cout<< " Operating Mode : Trigger "<<endl;}
    else{ if(vLevel(NORMAL))cout<< "Operating Mode : Continuous"<<endl;}
}

void tdc::Reset(){
    unsigned int DATA=0;
    int ADD = this->add +0x1014;
    try {
      for(int k = 0; k<3; k++){
        ADD+=2;
        writeData(ADD, &DATA);
      }
    } catch (CAENVMEexception &e) {
      std::cout << "TDC: " << e.what() << " while resetting." << std::endl;
      return;
    }
    if(vLevel(NORMAL))cout<< " Module Reset... " << endl << " Software Clear... " << endl <<  " Software Event Reset... " <<endl;
}

void tdc::setMode(bool Trig){
    unsigned int DATA=0;
    if(Trig) DATA = 0x0000;
    else DATA =0x0100;
    writeOpcode(DATA);
    if(vLevel(DEBUG))cout << "Trigger Mode : " << Trig<< endl;
}

void tdc::setMaxEvPerHit(int Max_ev_per_hit){
    for(int k=0; k<8;k++)
    if (Max_ev_per_hit== (2^(k)))
    {
        if(vLevel(NORMAL))if (k == 8) cout << "No limit on Maximum number of hits per event";
        unsigned int DATA=0x3300; // MEPH = maximum events per hits
        writeOpcode(DATA);
        DATA = k+1;
        writeOpcode(DATA);
    }
    else if (Max_ev_per_hit==0)
    {
	unsigned int DATA=0x3300;
        writeOpcode(DATA);
	DATA=0;
        writeOpcode(DATA);
    }
    else if(vLevel(WARNING))cout<< "Not a valid set  ! value of Max number of hits per event must be 0 or a power of 2 (1 2 4 .. 128) or 256 for NO LIMIT";
   }

void tdc::setWindowWidth(unsigned int WidthSetting) {   
  if (WidthSetting > 4095 )
    {if(vLevel(WARNING))cout << "Width Setting must be a integer in the range from 1 to 4095" << endl;}
  else {
    unsigned int DATA=0x1000;
    writeOpcode(DATA);
    DATA = WidthSetting;
    writeOpcode(DATA);
    cout << "Window Width set to"<< WidthSetting<< endl;
  }
}

void tdc::setWindowOffset(int OffsetSetting) {   
  if (OffsetSetting > 40 || OffsetSetting < -2048)
    {if(vLevel(WARNING))cout << "Offset Setting must be a integer in the range from -2048 to +40" << endl;}
  else {
    unsigned int DATA = 0x1100;
    writeOpcode(DATA);
    DATA = OffsetSetting;
    writeOpcode(DATA);
    cout << "Window Width set to"<< OffsetSetting<< endl;
  }
}

void tdc::setExSearchMargin(int ExSearchMrgnSetting ) {
  if (ExSearchMrgnSetting > 50)
    {if(vLevel(WARNING)) cout << " 50*25ns is the maximal value. Extra Search Margin Setting must be a integer in the range from 0 to 50" << endl;}
  else {
    unsigned int DATA = 0x1200;
    writeOpcode(DATA);
    DATA = ExSearchMrgnSetting;
    writeOpcode(DATA);
    if(vLevel(NORMAL))cout << "Extra Search Margin Width set to"<< ExSearchMrgnSetting<< endl;
  }
}

void tdc::setRejectMargin(int RejectMrgnSetting) {
  if (RejectMrgnSetting > 4095) {
    if(vLevel(WARNING))cout << "Offset Setting must be a integer in the range from -2048 to +40" << endl;}
  else {
    unsigned int DATA = 0x1300;
    writeOpcode(DATA);
    DATA = RejectMrgnSetting;
    writeOpcode(DATA);
    if(vLevel(NORMAL))cout << "Reject Margin set to"<< RejectMrgnSetting<< endl;
  }
}

void tdc::readWindowConfiguration() {
  unsigned int DATA=0x1600;
  writeOpcode(DATA);
  readOpcode(DATA);
  if(vLevel(NORMAL))cout<<" Match window width : "<<digit(DATA,11,0);
  readOpcode(DATA);
  if(vLevel(NORMAL))cout<<" Window ofset : "<<digit(DATA,11,0)-4096;
  readOpcode(DATA);
  if(vLevel(NORMAL))cout<<" Extra search window width: "<<digit(DATA,11,0);
  readOpcode(DATA);
  if(vLevel(NORMAL))cout<<" Reject margin width: "<<digit(DATA,11,0);
  readOpcode(DATA);
  if(vLevel(NORMAL))cout<<" Trigger time substraction : "<<digit(DATA,0);
}

void tdc::setChannelNumbers(int clock, int trigger) {
  ClockChannelNumber=clock;
  TriggerChannelNumber=trigger;
}

void tdc::enableFIFO() {
  unsigned int DATA;
  try {
    readData(ControlRegister, &DATA);
    if (digit(DATA,8)==0) {
      DATA+=0x0100;
    }
    writeData(ControlRegister, &DATA);
  } catch (CAENVMEexception &e) {
    std::cout << "TDC: " << e.what() << " while enabling FIFO." << std::endl;
    return;
  }
  if(vLevel(NORMAL))cout<<"FIFO enabled !"<<endl;
}
  
void tdc::disableTDCHeaderAndTrailer() {
  unsigned int DATA = 0x3100;
  writeOpcode(DATA);
  DATA = 0x3200;
  writeOpcode(DATA);
  readOpcode(DATA);
  if (DATA%2==0)
  if(vLevel(NORMAL))cout << "TDC Header and Trailer disabled"<< endl;
}
  
void tdc::readResolution() {
  unsigned int DATA=0x2600;
  writeOpcode(DATA);
  readOpcode(DATA);
  if(vLevel(NORMAL))cout<<" resolution : "<<digit(DATA,1,0)<<endl;;
}
  
void tdc::writeOpcode(unsigned int &DATA) {
  waitWrite();
  try {
    writeData(Opcode,&DATA);
  } catch (CAENVMEexception &e) {
    std::cout << "TDC: " << e.what() << " while writing OPCODE." << std::endl;
  }
}

void tdc::readOpcode(unsigned int &DATA)
{
  waitRead();
  try {
    readData(Opcode,&DATA);
  } catch (CAENVMEexception &e) {
    std::cout << "TDC: " << e.what() << " while reading OPCODE." << std::endl;
  }
}
