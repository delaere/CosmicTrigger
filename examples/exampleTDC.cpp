#include "../include/VmeUsbBridge.h"
#include "../include/TTCvi.h"
#include "../include/Discri.h"
#include "../include/HV.h"
#include "../include/TDC.h"

int main(){
  VmeUsbBridge myCont;
  TtcVi myTtc(&myCont);
  Tdc myTdc(&myCont);
  hv myHv(&myCont);
  Discri myDiscri(&myCont);
  
  //myHv.setChV(1250);
  //myHv.setChV(1400,3);
  //myHv.setChState(1);
  
  myDiscri.setMajority(4);
  myDiscri.setMultiChannel(0x008F);
  myDiscri.setTh(100);
  myDiscri.setWidth(255);
  myDiscri.setDeadTime(0);
  
  
  myTtc.changeChannel(2);
  myTtc.changeRandomFrequency(4);
  myTtc.viewMode();
  
  cout<<"----------------------------------------------------"<<endl;
  myTdc.setChannelNumbers(1,23);
  myTdc.setWindowWidth(50);
  myTdc.setWindowOffset(-25);
  myTdc.setExSearchMargin(1);
  myTdc.setRejectMargin(1);
  myTdc.readTriggerConfiguration();
  myTdc.enableFIFO();
  myTdc.disableTDCHeaderAndTrailer();
  myTdc.readResolution();
  
  

  for(int i=0;i<5000;i++)
  {
    vector<unsigned int> data;
    myTdc.getEvent(data);
    myTdc.coutEvent(data);
    myTdc.analyseEvent(data,"coucou");
    
  }
  

  
}
