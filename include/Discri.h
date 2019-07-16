#ifndef __DISCRI
#define __DISCRI

#include "VmeBoard.h"

/**
 * 
 * \brief Discriminator/Coincidence unit controller.
 * 
 * This class allows the user to control a CAEN V812 Discri/coincidence unit.
 * 
 * The data sheet can be found <a href="https://dl.dropboxusercontent.com/u/33459975/cosmicTrigger/datasheets/v812_rev5.pdf">here</a>.
 * 
 */
class Discri:public VmeBoard{
public:
  Discri(VmeController *controller,int add=0x070000);///<Constructor.
  
  int setChannel(int num=-1, bool newState=true);
  /**<
   * \brief Sets a channel on/off
   * 
   * This function will switch a given channel on/off. If the channel is not specified, it will set all the channels on/off. If the state is not specified, it will set it on.
   * 
   * 
   */
  int setMultiChannel(int code);
    /**<
   * \brief Sets all the channel at once.
   * 
   * Sends the given code to the Discri unit.
   * 
   * The code should be a 16 bits long number, the LSB setting channel 0 and the MSB setting channel 15.
   * 
   * If a bit is on 'true', then the corresponding channel will be on.
   * 
   * 
   */

  int setMajority(int num);
    /**<
   * 
   * \brief Sets the number of channels for a coincidence.
   * 
   * This function will send a number to the appropriate register in the Discriminator to set the minimal number of channels that have to be 'true' to generate a coincidence signal.
   * 
   */

  int setTh(int value,int num=-1);
    /**<
   * 
   * \brief Sets the threshold for the Discriminator
   * 
   * The threshold can be set independently for each channel.
   * 
   * If no channel is given, all the channels will have the same
   * threshold.
   * 
   * Ex: setTh(200,4) => channel 4 will have a threshold of 200 mV.
   * 
   * The value must be between 0 and 255mV.
   * 
   */

  int setWidth(int count,int num=-1);
    /**<
   * 
   * \brief Sets the width of the Discriminated signal
   * 
   * \param count Gives a parameter that will be transformed to a width. The relation between the 2 is not given but can be found in data sheets. For example, 126 is approx 25ns.
   * 
   * \param num Chooses the block of 8 channels to modify. Cannot change the width of a lone channel. The block containing the said channel will have that width. If nothing (or -1) is specified, all channels will be set.
   * 
   * More detail in data sheets.
   */

  int setDeadTime(int value,int num=-1);
    /**<
   * 
   * \brief Sets the dead time for a block of channels.
   * 
   * \param value : Dead time in ns after a signal was Discriminated.
   * 
   * \param num Chooses the block of 8 channels to modify. Cannot change the width of a lone channel. The block containing the said channel will have that width. If nothing (or -1) is specified, all channels will be set.   * 
   * 
   */

  int viewStatus(void);
  /**<
   * 
   * \brief Shows the stored on/off status for all the channels
   * 
   * WARNING: This does not show which channel is on/off, it indicates the stored value, the channels that the program THINKS are on or off.
   * 
   * If another program changes the channels, it will be ignored by this program and the values will be erased.
   * 
   * Example:
   * 
   * This prog: Sets channel 1 and 5 on => stored state = 0x0022 = real state
   * 
   * Other program sets channel 0 on => real state = 0x0023
   *
   * If this prog sets the channel 6 on => goes back to stored state + channel 6 => 0x0032 in stead of 0x0033
   * 
   * All this to say: don't launch 2 programs editing the same parameter!
   * 
   */
  
private:  
  int status_;
};

#endif
