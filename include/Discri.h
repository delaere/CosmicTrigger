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

#ifndef __DISCRI
#define __DISCRI

#include "VmeBoard.h"
#include <vector>

// Discriminator/Coincidence unit controller.
// This class allows the user to control a CAEN V812 Discri/coincidence unit.
class Discri:public VmeBoard{
public:
  
  struct moduleInfo {
    uint16_t moduleId_;
    uint16_t serial_number_;
    uint16_t moduleType_;
  };
  
  Discri(VmeController *controller,int add=0x070000);
  
  // Enable channel
  void enableChannel(uint8_t channel);
  
  // Disable channel
  void disableChannel(uint8_t channel);
  
  // Set the global channel mask
  void setChannelMask(uint16_t mask);
  
  // Get the global channel mask
  inline uint16_t getChannelMask() const { return status_; }
  
  // Sets the number of channels for a coincidence.
  // This function will send a number to the appropriate register in the Discriminator 
  // to set the minimal number of channels that have to be 'true' to generate a coincidence signal.
  void setMajority(uint8_t num);
  
  // Sets the threshold for the Discriminator (mV)
  void setThreshold(uint8_t value,int8_t channel=-1 /*-1 = all */);
  
  // Sets the width of the Discriminated signal, in ns
  void setWidth(float ns,uint8_t channel);

  // Sets the dead time for a block of 8 channels.
  void setDeadTime(float ns,uint8_t channel);

  // Generate a test pulse
  void testPulse();
  
  // Gets module's info.
  inline const moduleInfo getInfo() const { return info_; }
  
private:  
  void setChannel(uint8_t channel, bool newState);
  
  int status_;
  
  static std::vector<float> widths_;
  static std::vector<float> wcounts_;
  static std::vector<float> deadtimes_;
  static std::vector<float> dtcounts_;
  static float interpolate( std::vector<float> &xData, std::vector<float> &yData, float x, bool extrapolate );
  static float getWCount(float width) { return interpolate(widths_,wcounts_,width,false); }
  static float getDTCount(float dt) { return interpolate(deadtimes_,dtcounts_,dt,false); }
  
  moduleInfo info_;

};

#endif
