#include "../include/VmeUsbBridge.h"
#include "../include/TTCvi.h"
#include "../include/Discri.h"
#include "../include/HV.h"
#include "../include/Scaler.h"


int main(){
  VmeUsbBridge myCont;
  hv myHv(&myCont);
  Discri myDiscri(&myCont);
  
  myDiscri.setMultiChannel(0x000F);
  myDiscri.setMajority(4);
  myDiscri.setTh(100);
  
  Scaler myScaler(&myCont,0xCCCC00);  
  myScaler.reset();
  
  myHv.setChState(1);
  
  for (int voltage=1250; voltage<1500; voltage+=50){
   LOG_INFO("Voltage set to " + to_string(voltage)+"V");
   myHv.setChV(voltage); 
   system("sleep 10s");
   myScaler.reset();
   LOG_INFO("Scaler reseted, counting data.");
   system("sleep 100s");
   for (int channel=11; channel<17; channel++){
     system("sleep 0.1s");
     LOG_DATA_INFO("Channel"+to_string(channel)+" : "+to_string(voltage)+" - "+to_string(myScaler.getCount(channel)));
     //note: LOG_DATA_INFO uses a trivial logger. We could also specify a dedicated logger with channel as parameter.
   }
  } 
}
