#include "VmeUsbBridge.h"
#include "TTCvi.h"
#include "Discri.h"
#include "TDC.h"

int main(){
  VmeUsbBridge myCont;
  TtcVi myTtc(&myCont);
  Tdc myTdc(&myCont);
  Discri myDiscri(&myCont);
 
  myDiscri.setMajority(4);
  myDiscri.setChannelMask(0x8F);
  myDiscri.setThreshold(100);
  myDiscri.setWidth(240,0);
  myDiscri.setWidth(240,8);
  myDiscri.setDeadTime(0,0);
  myDiscri.setDeadTime(0,8);
  
  myTtc.changeChannel(TtcVi::cvRandom);
  myTtc.changeRandomFrequency(TtcVi::cv10kHz);
  myTtc.viewMode();
  
  LOG_INFO("----------------------------------------------------");
  Tdc::WindowConfiguration conf;
  conf.width = 50;
  conf.offset = -25;
  conf.extraMargin = 1;
  conf.rejectMargin = 1;
  conf.triggerTimeSubstraction = false;
  myTdc.setTriggerWindow(conf);
  myTdc.enableFIFO(true);
  myTdc.enableTDCHeader(false);
  
  for(int i=0;i<5000;i++) {
    LOG_DATA_INFO(myTdc.getEvent().toString());
  }
}
