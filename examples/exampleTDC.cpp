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
  myDiscri.setMultiChannel(0x008F);
  myDiscri.setTh(100);
  myDiscri.setWidth(255);
  myDiscri.setDeadTime(0);
  
  myTtc.changeChannel(TtcVi::cvRandom);
  myTtc.changeRandomFrequency(TtcVi::cv10kHz);
  myTtc.viewMode();
  
  LOG_INFO("----------------------------------------------------");
  myTdc.setWindowWidth(50);
  myTdc.setWindowOffset(-25);
  myTdc.setExSearchMargin(1);
  myTdc.setRejectMargin(1);
  myTdc.enableFIFO();
  myTdc.disableTDCHeaderAndTrailer();
  myTdc.readResolution();
  
  for(int i=0;i<5000;i++) {
    myTdc.getEvent().print();
  }
}
