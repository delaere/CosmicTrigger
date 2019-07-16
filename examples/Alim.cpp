#include "../include/VmeUsbBridge.h" //Includes USB controller.
#include "../include/HV.h"	//Includes High Voltage class


int main(){
  
  VmeUsbBridge myCont(NORMAL); //Creates a usb controller with NORMAL verbosity.
  hv myHv(&myCont);	//Creates a high voltage object with the controller.
  
  cout<<"Setting all channels to 1250V...";
  if(myHv.setChV(1250)) cout<<" Ok!"<<endl;
  
  cout<<"Setting channel 3 to 1400V...";
  if(myHv.setChV(1400,3))  cout<<" Ok!"<<endl;
  cout<<"Setting all channels on...";
  if(myHv.setChState(1))  cout<<" Ok!"<<endl;
  
  cout<<"Waiting for Ctrl-C"<<endl;
  system("sleep 100h"); // I know, this is ugly, but easy!
  
  cout<<"Shutting down..."<<endl;
  if(myHv.setChState(0))  cout<<" Ok!"<<endl; 
} 
