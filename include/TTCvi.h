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

using namespace std;

#include "VmeBoard.h"

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
  
  // Change the trigger channel.
  void changeChannel(CVTriggerChannel channel);
  
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
  
  // Changes the frequency of the random trigger.
  // Will set the esperance of the poisson distribution used to generate a random trigger
  void changeRandomFrequency(CVTriggerRate frequencyId);

  // Returns the value of the mode.
  int viewMode() const;

private:
  CVTriggerChannel channel_;
  CVTriggerRate frequency_;
  
  void program() const;
};
 
#endif 
