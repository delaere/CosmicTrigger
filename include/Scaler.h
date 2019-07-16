#ifndef __SCALER
#define __SCALER

#include "VmeBoard.h"

// Counting unit.
class Scaler:public VmeBoard{
public:
  Scaler(VmeController* controller,int address=0x0B0000);///<Constructor. Sets up the address and tests communication.
  
  // Getting count value. It reads the appropriate register and returns its value.
  int getCount(int channel);

  // Gets module's info.
  // reads the module information register and returns its value. Returns -1 if the communication failled.
  int getInfo();
  
  // Sends a reset signal to the module.
  // returns 1 if communication went ok, -1 if not.
  void reset();
  
  // Reads channel presets.
  // If this value is not 0, the module will be in countdown mode from this value.
  int readPresets(int channel);
 
  // Sets the preset for a channel.
  // If this value is not 0, the module will be in countdown mode from this value.
  void setPresets(int channel,int value);

};
 
#endif 
 
