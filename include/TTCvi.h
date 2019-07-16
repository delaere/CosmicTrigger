#ifndef __TTCVI
#define __TTCVI

using namespace std;

#include "VmeBoard.h"

//TODO: functions to change variables

// This class has a few functions encoding the basic functionalities of the TTCvi, to be used in conjunction with a TTCvx.
class TtcVi:public VmeBoard{
  
public:  
  
  TtcVi(VmeController* controller,int address=0x555500);

  // Change the trigger channel.
  // -1 => Random (same as 5)
  // 0 to 3 => LV1 0 to 3
  // 4 => VME function
  // 5 => Random (same as -1)
  // 6 => calib
  // 7 => Disabled
  void changeChannel(int channel);
  
  // Changes the frequency of the random trigger.
  // Will set the esperance of the poisson distribution used to generate a trigger to:
  // 0 => 1Hz
  // 1 => 100Hz
  // 2 => 1kHz
  // 3 => 5kHz
  // 4 => 10kHz
  // 5 => 25kHz
  // 6 => 50kHz
  // 7 => 100kHz
  void changeRandomFrequency(int frequencyId);

  // If the verbose level of the VmeController is NORMAL or DEBUG, this function will print the current trigger mode.
  // It also returns the value hex value of the mode.
  int viewMode(void);

private:
  int channel;
  int channelFrequency;
};
 
#endif 
