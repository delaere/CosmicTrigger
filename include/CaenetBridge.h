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

#ifndef __VMECAENETBOARD
#define __VMECAENETBOARD

#include "VmeBoard.h"
#include <vector>

// V288 CAEN board
class CaenetBridge:public VmeBoard{
public:
  CaenetBridge(VmeController *cont, uint32_t bridgeAdd, uint8_t interrupt = 0);
  
  // Performs a reset
  void reset();
  
  // Check if the last operation was valid (returns the opposite of the status register LSB)
  bool validStatus();
  
  // Send data to caenet
  void write(const std::vector<uint32_t>& data);
  
  // Read back the response from the slave
  std::pair<uint32_t, std::vector<uint32_t> > readResponse();

private:
  uint8_t interrupt_;
  
};

#endif //__VMECAENETBOARD
