#include "VmeUsbBridge.h" // VmeUsbBridge object
#include "TTCvi.h"	// TTCvi object
#include "Discri.h"	// Discriminator object
#include "Scaler.h"	// Scaler object


int main(){
 
  VmeUsbBridge myCont;// Creates a USB controller
  Discri myDiscri(&myCont); // Creates a Discriminator unit with that controller
  TtcVi myTtc(&myCont);	// Same thing with a TTCvi
  
  myTtc.changeChannel(1); //Chooses the trigger on channel 1 
  
  myDiscri.setMultiChannel(0x000F); //Enables 4 first channels on Discriminator
  myDiscri.setMajority(4);	// Asks for a coincidence of 4 units
  myDiscri.setTh(100);		// Sets the threshold to 100mV
  
}
