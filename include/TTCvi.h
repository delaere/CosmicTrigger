/*
 *  VeheMencE: a simple library for VME access
 *  Copyright (C) 2019 Universite catholique de Louvain (UCLouvain), Belgium
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __TTCVI
#define __TTCVI

#include "VmeBoard.h"
#include <utility>
#include <bitset>

// This class has a few functions encoding the basic functionalities of the TTCvi, to be used in conjunction with a TTCvx.
class TtcVi:public VmeBoard{
  
public:  
  
  TtcVi(VmeController* controller,int address=0x555500);

  typedef enum CVTriggerChannel {
    cvL1A0    = 0,
    cvL1A1    = 1,
    cvL1A2    = 2,
    cvL1A3    = 3,
    cvVME     = 4,
    cvRandom  = 5,
    cvCalib   = 6,
    cvDisable = 7
  } CVTriggerChannel;
  
  typedef enum CVTriggerRate {
    cv1Hz    = 0,
    cv100Hz  = 1,
    cv1kHz   = 2,
    cv5kHz   = 3,
    cv10kHz  = 4,
    cv25kHz  = 5,
    cv50kHz  = 6,
    cv100kHz = 7
  } CVTriggerRate;
  
  // module info
  struct ModuleInfo {
    uint32_t manufacturer_;
    uint32_t serial_number_;
    uint32_t revision_;
  };
  
  ////////////////////////////////
  // Basic functions
  ////////////////////////////////
  
  // resets the module
  void reset();
  
  // generates a software trigger
  void trigger();
  
  // resets the (event or orbit) counter
  void resetCounter();
  
  // get the counter
  uint32_t getEventNumber();
  
  // set the counter
  void setEventCounter(uint32_t count);
  
  // set the counter mode 0:event count; 1:orbit count
  void setCounterMode(bool orbit);
  
  ////////////////////////////////
  // Channel A functions
  ////////////////////////////////
  
  // get the active trigger channel
  CVTriggerChannel getTriggerChannel();
  
  // set the active trigger channel
  void setTriggerChannel(CVTriggerChannel channel);
  
  // get the programmed random trigger rate
  CVTriggerRate getRandomTriggerRate();
  
  // set the programmed random trigger rate
  void setRandomTriggerRate(CVTriggerRate rate);
  
  // get L1 FIFO status 1=full, 2=empty, 0=other
  uint8_t getFIFOStatus();
  
  // reset the L1 FIFO
  void resetL1FIFO();
  
  // read the BC delay
  uint8_t getBC0Delay();
  
  ////////////////////////////////
  // Channel B functions
  ////////////////////////////////
  
  //Short-format asynchronous cycles
  void channelBAsyncCommand(uint8_t command);
  
  // Long-format asynchronous cycles
  void channelBAsyncCommand(uint8_t addr, uint8_t subAddr, uint8_t data, bool internal=false);
  
  // get the inhibit<n> configuration
  std::pair<uint8_t, uint8_t> getInhibit(unsigned int n);
  
  // set the inhibit<n> configuration
  void setInhibit(unsigned int n,uint8_t delay,uint8_t duration);
  
  // get the B-Go mode
  std::bitset<4> getBGo(unsigned int n);
  
  // set the B-Go mode
  void setBGo(unsigned int n,bool softTrigger, bool asynchronous, bool repeat, bool autoTrigger);
  
  // trigger a B-Go signal
  void triggerBGo(unsigned int n);
  
private:  
  // Module info
  ModuleInfo info_;
  
  // methods
  uint16_t readCSR1();
  uint16_t readCSR2();
  void writeCSR1(uint16_t word);
  void writeCSR2(uint16_t word);
  
};
 
#endif 
