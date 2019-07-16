#ifndef __TDC
#define __TDC

#include "VmeBoard.h"
#include <vector>
#include <sstream>
#include "time.h"

struct Hit
{
  unsigned int channel;
  unsigned int time;
};

struct Event
{
  unsigned int eventNumber;
  time_t time;
  std::vector<Hit> measurements;
};

// V1190 TDC unit
class Tdc:public VmeBoard
{
public:

  Tdc(VmeController* controller,int address=0x00120000);

  // Gets data from TDC
  Event getEvent();
  
  // Print the event content
  static void coutEvent(Event myEvent);

  // Get the TDC status
  unsigned int getStatus();

  // Reset the board
  void Reset();

  // Set the acquisition mode : Trigger (1), Continuous (0).
  void setMode(bool Trig = 1);

  // Set the maximum number of hits the TDC should memorise in a window
  // Possible values are : -0,1,2,4,8,16,32,64,128, -256 for no limit
  // the TDC sends an errorcode if the number is exceeded, but 'getEvent()' doesn't take account of it.
  void setMaxEvPerHit(int Max_ev_per_hit =256 );

  // Set the window width in multiples of clockcycle.
  void setWindowWidth(unsigned int WidthSetting);

  // Set the difference between the beginning of the window and the external trigger in multiples of clockcycle.
  void setWindowOffset(int OffsetSetting);

  // Set the extra search margin in units of clock cycle.
  void setExSearchMargin(int ExSearchMrgnSetting );

  // Set the reject margin in units of clock cycle.
  void setRejectMargin(int RejectMrgnSetting);

  // Get the current match window settings 
  void readWindowConfiguration();

  // Print the resolution
  // 0 -> 800ps, 1 -> 200ps, 2 -> 100ps
  // works only in leading/trailing edge detection mode
  void readResolution();

  // Enables FIFO  
  void enableFIFO();

  //  Disable extra information send from the TDC. 
  void disableTDCHeaderAndTrailer();

  // Write  a command line of 16 bit in the Micro Controller register.
  void writeOpcode(unsigned int &data);

  // Read a 16 bit word in the Micro Controller register.
  void readOpcode(unsigned int &data);

private:

  //REGISTER ADRESSES (must depend on add)
  int Opcode;
  int StatusRegister;
  int MicroHandshake;
  int OutputBuffer;
  int EventFIFO;
  int ControlRegister;

  //PRIVATE FUNCTIONS
  void waitWrite();
  void waitRead();
  void waitDataReady();
};

#endif
