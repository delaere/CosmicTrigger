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
  int viewMode();

private:
  int channel;
  int channelFrequency;
};
 
#endif 
