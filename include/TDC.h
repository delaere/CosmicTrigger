#ifndef __TDC
#define __TDC

#include "VmeBoard.h"
#include "Event.h"
#include <vector>
#include <sstream>
using namespace std;
/**
 * \brief
 *  This class has a few functions encoding the basic functionalities of the TDC.
 * 
 *  You can control a few basic functionalities of the Time to Digital Converter.
 * 
 *  See different functions for detail.
 * 
 * Here is the  <a href="https://dl.dropboxusercontent.com/u/33459975/cosmicTrigger/datasheets/V1190_REV13.pdf">data sheet</a> of the V1190 unit.
 */

class tdc:public vmeBoard{
public:
  // CONSTRUCTOR
  tdc(vmeController* controller,int address=0x00120000);
  /**<
  * \brief Constructor
  * 
  * Adress must terminate with 0x0000.
  * 
  * If address not given, will assume it's 0x00120000.
  */

  //FUNCTIONS
  int getEvent(event &myEvent);
  /**<
   * \brief gets data from TDC
   * 
   * -Waits for "event ready" status from TDC
   * 
   * -Writes the event in myEvent
   * 
   * This function needs to have FIFO enabled (cf 'enableFIFO()' ). 
   * 
   * It has only been tested in trigger mode.
   */
  void analyseEvent(event myEvent, const char* filename);
  /**
   * \brief writes the fase of the event "myEvent".
   * 
   * -Computes the time difference, in units of TDC precision (cf 'readResolution()'), between the first hit in the channel 
   * 
   * TriggerChannelNumber inside the search window and the preceding clock cycle in the channel ClockChannelNumber.
   *
   * -Writes the phase in 'filename'
   */
  void coutEvent(event myEvent);
  /**
   * \brief desribes explicitely the content of myEvent in the standard output stream
   */
  void ReadStatus();
  /**
   * \brief describes the status of the TDC in the stantdard output stream.
   * 
   * the status includes :
   * 
   * -Event ready/Data not ready
   * 
   * -Output buffer is (not) full
   * 
   * -Operating mode :trigger/continuous
   * */
  void Reset();
  /**
   * \brief resets the board.
   */
  void setMode(bool Trig = 1);
  /**
   * \brief set the acquisition mode : Trigger (1), Continuous (0).
   */
  void setMaxEvPerHit(int Max_ev_per_hit =256 );
  /**
   * \brief sets the maximum number of hits the TDC should memorise in a window
   * 
   * Possible values are : 
   * 
   * -0,1,2,4,8,16,32,64,128 
   * 
   * -256 for no limit
   * 
   * the TDC sends an errorcode if the number is exceeded, but 'getEvent()' doesn't take account of it.
   * */
  void setWindowWidth(unsigned int WidthSetting);
  /**
   * \brief sets the window width in multiples of clockcycle.
   */
  void setWindowOffset(int OffsetSetting);
  /**
   * \brief sets the difference between the beginning of the window and the external trigger in multiples of clockcycle.
   */
  void setExSearchMargin(int ExSearchMrgnSetting );
  /**
   * \brief sets the extra search margin in units of clock cycle.
   */
  void setRejectMargin(int RejectMrgnSetting);
  /**
   * \brief sets the reject margin in units of clock cycle.
   */
  void readWindowConfiguration();
  /**
   * \brief descibes the current match window settings in the standard output stream.
   * 
   * includes :
   * 
   * -match window width
   * 
   * -Window ofset
   * 
   * -Extra search window width
   * 
   * -Reject margin width
   * 
   * -Trigger time substraction
   */
  void readResolution();
  /**
   * \brief writes the resolution in the standard output stream
   * 
   * 0 -> 800ps
   * 
   * 1 -> 200ps
   * 
   * 2 -> 100ps
   * 
   * works only in leading/trailing edge detection mode
   */
  
  void setChannelNumbers(int clock, int trigger);
  /**
   * \brief sets the channel number of the clock and the trigger.
   * 
   * This channel numbers are needed for 'analyseEvent()'
   */
  
  void enableFIFO();
  /**
   * \brief enables FIFO.
   * 
   * This in necessary for 'getEvent()'
   */
  
  void disableTDCHeaderAndTrailer();
  /**
   * \brief disables extra information send from the TDC. 
   * 
   * This information is neglected by 'getEvent()'.
   */
 
  void writeOpcode(unsigned int &data);
  /**
   * \brief writes a command line of 16 bit in the Micro Controller register.
   * 
   * This command includes a wait time for micro controllers 'writing ready' bit.
   */
  void readOpcode(unsigned int &data);
  /**
   * \brief reads a 16 bit word in the Micro Controller register.
   * 
   * This command includes a wait time for micro controllers 'read ready' bit.
   */

private:

  //REGISTER ADRESSES (must depend on add)
  int Opcode;
  int StatusRegister;
  int MicroHandshake;
  int OutputBuffer;
  int EventFIFO;
  int ControlRegister;

  //INTERNAL VARIABLES
  unsigned int ClockChannelNumber;
  unsigned int TriggerChannelNumber;


  //PRIVATE FUNCTIONS
  int waitWrite(void);
  int waitRead(void);
  int waitDataReady(void);


};


#endif
