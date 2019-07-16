#ifndef __DISCRI
#define __DISCRI

#include "VmeBoard.h"

// Discriminator/Coincidence unit controller.
// This class allows the user to control a CAEN V812 Discri/coincidence unit.
class Discri:public VmeBoard{
public:
  Discri(VmeController *controller,int add=0x070000);
  
  // Sets a channel on/off
  // If the channel is not specified, it will set all the channels on/off. If the state is not specified, it will set it
  void setChannel(int num=-1, bool newState=true);

  // Sets all the channel at once.
  // The code should be a 16 bits long number, the LSB setting channel 0 and the MSB setting channel 15.
  void setMultiChannel(int code);

  // Sets the number of channels for a coincidence.
  // This function will send a number to the appropriate register in the Discriminator 
  // to set the minimal number of channels that have to be 'true' to generate a coincidence signal.
  void setMajority(int num);

  // Sets the threshold for the Discriminator
  // If no channel is given, all the channels will have the same threshold.
  // Ex: setTh(200,4) => channel 4 will have a threshold of 200 mV.
  // The value must be between 0 and 255mV.
  void setTh(uint8_t value,int num=-1);

  // Sets the width of the Discriminated signal
  // count gives a parameter that will be transformed to a width. 
  //   The relation between the 2 is not given but can be found in data sheets. For example, 126 is approx 25ns.
  // num chooses the block of 8 channels to modify. Cannot change the width of a lone channel. 
  //   The block containing the said channel will have that width. If nothing (or -1) is specified, all channels will be set.
  void setWidth(uint8_t count,int num=-1);

  // Sets the dead time for a block of 8 channels.
  void setDeadTime(uint8_t value,int num=-1);

  // Shows the stored on/off status for all the channels
  // This does not show which channel is on/off, it indicates the stored value, no data is retrieved from the module.
  inline int getStatus() { return status_; }
  
private:  
  int status_;
};

#endif
