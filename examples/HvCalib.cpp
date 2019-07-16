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
   cout<<endl<<endl<<"Voltage set to "<<voltage<<"V"<<endl;
   myHv.setChV(voltage); 
   system("sleep 10s");
   myScaler.reset();
   cout<<"Scaler reseted, counting data."<<endl;
   system("sleep 100s");
   
   for (int channel=11; channel<17; channel++){
     system("sleep 0.1s");
      ostringstream mystream;
      mystream<<"echo '"<<voltage<<" - "<<myScaler.getCount(channel)<<"' >> Channel"<<channel<<".txt";
      system(mystream.str().c_str());// Printing return value of the scaler in the Channelxx.txt file.
   }
   
  } 
}
