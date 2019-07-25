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

#ifndef __SCALER
#define __SCALER

#include "VmeBoard.h"

// Counting unit.
class Scaler:public VmeBoard{
public:
  Scaler(VmeController* controller,int address=0x0B0000);///<Constructor. Sets up the address and tests communication.
  
  // Getting count value. It reads the appropriate register and returns its value.
  int getCount(int channel);

  // Gets module's info.
  // reads the module information register and returns its value.
  int getInfo();
  
  // Sends a reset signal to the module.
  void reset();
  
  // Reads channel presets.
  // If this value is not 0, the module will be in countdown mode from this value.
  int readPresets(int channel);
 
  // Sets the preset for a channel.
  // If this value is not 0, the module will be in countdown mode from this value.
  void setPresets(int channel,int value);

};
 
#endif 
 
