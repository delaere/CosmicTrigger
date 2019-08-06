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

#include "Discri.h"
#include <iostream>
#include <cmath>
using namespace std;

std::vector<float> Discri::widths_    = {11.32,12.34,13.47,14.75,16.07,17.51,19.03,21.29,23.69,26.71,30.61,35.20,41.83,51.02,64.53,87.47,130.70,240.70};
std::vector<float> Discri::wcounts_   = {0,15,30,45,60,75,90,105,120,135,150,165,180,195,210,225,240,255};
std::vector<float> Discri::deadtimes_ = {150,2000};
std::vector<float> Discri::dtcounts_  = {0,255};

Discri::Discri(VmeController *controller,int add):VmeBoard(controller, add, cvA32_U_DATA, cvD16, true),status_(0x0000) {
  // check the connection...
  uint16_t data = 0;
  readData(baseAddress()+0xFC,&data);
  info_.moduleType_ = data;
  readData(baseAddress()+0xFE,&data);
  info_.serial_number_ = data;
  readData(baseAddress()+0xFA,&data);
  info_.moduleId_ = data;
  assert(info_.moduleId_==0xFAF5);
  
  // initial config
  setChannelMask(status_);
  
  LOG_DEBUG("CAEN V812 initialized. " + 
            int_to_hex(info_.moduleType_&0x3FF) + " " + int_to_hex(info_.moduleType_>>9) + " " + 
            int_to_hex(info_.serial_number_&0xFFF) + " " + int_to_hex(info_.serial_number_>>11) + " " + 
            int_to_hex(info_.moduleId_));
}

void Discri::enableChannel(uint8_t channel) {
  assert(channel<16);
  setChannel(channel,1);
}
  
void Discri::disableChannel(uint8_t channel) {
  assert(channel<16);
  setChannel(channel,0);
}
  
void Discri::setChannelMask(uint16_t mask) {
  status_ = mask;
  writeData(baseAddress()+0x4A,&status_);
  LOG_INFO("Channels changed. Mask:" + to_string(status_));
}

void Discri::setChannel(uint8_t channel, bool newState){
  if (newState != ((status_>>channel)&1)) {
    status_ ^= (1u << channel);
    // write the result to the proper register
    writeData(baseAddress()+0x4A,&status_);
  }
  LOG_INFO("New status for channel " + to_string(channel) +": "+ to_string(newState));
}

void Discri::setMajority(uint8_t num){
  assert(num>0 && num<21);
  int majthr = round((num*50.-25.)/4.);
  writeData(baseAddress()+0x48,&majthr);
  LOG_INFO("Set majority level to " + to_string(num) + "(sent: " + to_string(majthr) + ")");
}

void Discri::setThreshold(uint8_t value,int8_t channel){
  if (channel==-1){
    LOG_INFO("Setting all thresholds to "+to_string(value));
    for (int i=0; i<16; i++) 
      this->setThreshold(value,i);
  } else {
    assert(channel>=0 && channel<16);
    LOG_DEBUG("Setting threshold to " + to_string(value) + " on channel " + to_string(channel));
    writeData(baseAddress()+2*channel,&value);
  }
}

void Discri::setWidth(float ns,uint8_t channel){
  assert(channel<16);
  uint16_t wcount = (uint16_t)Discri::getWCount(ns);
  LOG_INFO("Setting output width to "+ to_string(wcount));
  if (channel<8) {
    writeData(baseAddress()+0x40,&wcount);
  } else {
    writeData(baseAddress()+0x42,&wcount);
  }
}

void Discri::setDeadTime(float ns,uint8_t channel){
  assert(channel<16);
  uint16_t dtcount = (uint16_t)Discri::getDTCount(ns);
  LOG_INFO("Setting dead time to " + to_string(dtcount));
  if (channel<8) {
    writeData(baseAddress()+0x44,&dtcount);
  } else { 
    writeData(baseAddress()+0x46,&dtcount);
  }
}

void Discri::testPulse() {
  LOG_INFO("Generating a test pulse.");
  writeData(baseAddress()+0x4C,&status_);
}

float Discri::interpolate( std::vector<float> &xData, std::vector<float> &yData, float x, bool extrapolate ) {
   int size = xData.size();

   int i = 0;                                                                  // find left end of interval for interpolation
   if ( x >= xData[size - 2] ) {                                               // special case: beyond right end
      i = size - 2;
   } else {
      while ( x > xData[i+1] ) i++;
   }
   float xL = xData[i], yL = yData[i], xR = xData[i+1], yR = yData[i+1];      // points on either side (unless beyond ends)
   if ( !extrapolate ) {                                                       // if beyond ends of array and not extrapolating
      if ( x < xL ) yR = yL;
      if ( x > xR ) yL = yR;
   }
   float dydx = ( yR - yL ) / ( xR - xL );                                    // gradient
   return yL + dydx * ( x - xL );                                              // linear interpolation
}
