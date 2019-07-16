#include "include/VmeUsbBridge.h"
#include "include/TTCvi.h"
#include "include/Discri.h"
#include "include/Scaler.h"


int main(){
  VmeUsbBridge myCont(DEBUG);
  Discri myDiscri(&myCont);
  TtcVi myTtc(&myCont);
  
  myTtc.changeChannel(1);
    
  myDiscri.setMultiChannel(0x000F);
  myDiscri.setMajority(4);
  myDiscri.setTh(100);
  
  
}
