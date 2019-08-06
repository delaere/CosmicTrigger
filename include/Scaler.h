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

// Lecroy 1151N counting unit.
class Scaler:public VmeBoard{
public:
  
  struct moduleInfo {
    uint16_t moduleId_;
    uint16_t serial_number_;
    uint16_t moduleType_;
  };
  
  Scaler(VmeController* controller,uint32_t address=0x0B0000);///<Constructor. Sets up the address and tests communication.
  
  // Getting count value for channel [0,15]. It reads the appropriate register and returns its value.
  uint32_t getCount(uint8_t channel, bool reset = false);

  // Gets module's info.
  inline const moduleInfo getInfo() const { return info_; }

  // Sets the preset for a channel in [0,15].
  // If this value is not 0, the module will be in countdown mode from this value.
  void setPreset(uint8_t channel,uint32_t value);
  
  // Sends a reset signal to the module.
  void reset();
  
  // Set interrupt
  void setInterrupt(uint8_t level, uint8_t vector);

  
private:
  moduleInfo info_;

};
 
#endif 
 
