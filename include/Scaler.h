#ifndef __SCALER
#define __SCALER

using namespace std;

#include "VmeBoard.h"


/**
 * \brief Counting unit.
 * 
 * This counting unit alows to monitor the activity on the trigger system.
 * 
 * To do so, this class implements a few basic functions.
 * 
 * 
 * 
 */

class Scaler:public VmeBoard{
public:
  Scaler(VmeController* controller,int address=0x0B0000);///<Constructor. Sets up the address and tests communication.
  int getCount(int channel);
  /**<
   * \brief Getting count value.
   * 
   * This function allows to tell the value counted for a channel. It reads the appropriate register and returns its value.
   * 
   * Returns -1 if the communication failled.
   * 
   */
  int getInfo(void);
  /**<
   * 
   * \brief Gets module's info.
   * 
   * This function reads the module information register and returns its value.
   * 
   * Returns -1 if the communication failled.
   * 
   * This function is used by the constructor to test the communication with the module.
   */
  int reset(void);
  /**<
   * 
   * \brief Sends a reset signal to the module.
   * 
   * This function does a write operation on the reset register of the module. By doing so, all the presets return to their default values and the stored counts go back to 0;
   * 
   * returns 1 if communication went ok, -1 if not.
   * 
   */
  int readPresets(int channel);
  /**<
   * 
   * \brief Reads channel presets.
   * 
   * This function will read a channel's preset register and return its value.
   *
   * If this value is not 0, the module will be in countdown mode from this value.
   *
   * Returns -1 if communication error. 
   * 
   * 
   */
  int setPresets(int channel,int value);
  /**<
   * 
   * \brief Sets the preset for a channel.
   * 
   * This function sets the preset of a given channel to a given value.
   * 
   * If this value is not 0, the module will be in countdown mode from this value.
   * 
   * Returns -1 if communication error. 
   */
};
 
#endif 
 
