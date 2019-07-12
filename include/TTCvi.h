#ifndef __TTCVI
#define __TTCVI

using namespace std;

#include "VmeBoard.h"

//TODO: functions to change variables


/**
 * \brief This class has a few functions encoding the basic functionalities of the TTCvi.
 * 
 *  You can control a few basic functionalities of the Triger and Time Control unit.
 * 
 *  See different functions for detail.
 * 
 * Here are the datasheets for <a href="https://dl.dropboxusercontent.com/u/33459975/cosmicTrigger/datasheets/TTCviSpec.pdf">TTCvi unit</a> and the <a href="https://dl.dropboxusercontent.com/u/33459975/cosmicTrigger/datasheets/ttcvx.pdf">TTCvx unit</a>
 */

class ttcVi:public vmeBoard{
  
public:  
  
  ttcVi(vmeController* controller,int address=0x555500);
  /**<
   * \brief Constructor.
   * 
   * \param address The address of the module. Is set with 4 switches on the board. See datasheets for details.
  */
  void changeChannel(int channel);
  /**<
   * \brief Allows you to change the trigger channel.
   * 
   * -1 => Random (same as 5)
   * 
   * 0 to 3 => LV1 0 to 3
   * 
   * 4 => VME function
   * 
   * 5 => Random (same as -1)
   * 
   * 6 => calib
   * 
   * 7 => Disabled
   */
  
  void changeRandomFrequency(int frequencyId);
  /**< \brief Changes the frequency of the random trigger.
   * 
   * Will set the esperance of the poisson distribution used to generate a trigger to:
   * 
   * 0 => 1Hz
   * 
   * 1 => 100Hz
   * 
   * 2 => 1kHz
   * 
   * 3 => 5kHz
   * 
   * 4 => 10kHz
   * 
   * 5 => 25kHz
   * 
   * 6 => 50kHz
   * 
   * 7 => 100kHz
   * 
   * /!\ It will change the trigger mode to random if it was not yet used.
   * 
   */
  
  int viewMode(void);
  /** \brief Shows informations about the ttcVi.
   * 
   *  If the verbose level of the vmeController is NORMAL or DEBUG, this function will print the current trigger mode.
   * 
   *  It also returns the value hex value of the mode.
   */
  
private:
  int channel;
  int channelFrequency;
};
 
#endif 
