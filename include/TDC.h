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

#ifndef __TDC
#define __TDC

#include "VmeBoard.h"
#include <vector>
#include <sstream>
#include "time.h"

typedef std::pair<uint32_t,uint32_t> TDCHit;

class TDCEvent
{
public:
  TDCEvent() {}
  ~TDCEvent() {}
  
  void setTime(time_t t) { time_ = t; }
  time_t getTime() const { return time_; }
  void setEventNumber(uint32_t n) { eventNumber_ = n; }
  uint32_t eventNumber() const { return eventNumber_; }
  
  void addMeasurement(TDCHit hit) { measurements_.push_back(hit); }
  void setMeasurements(const std::vector<TDCHit>& hits) { measurements_.clear(); std::copy(hits.begin(),hits.end(),measurements_.begin()); }
  auto getMeasurements() const { return measurements_; }
  uint32_t getNmeasurements() const { return measurements_.size(); }
  
  void print() const;
  
private:
  uint32_t eventNumber_;
  time_t time_;
  std::vector<TDCHit> measurements_;
};

// V1190 TDC unit
class Tdc:public VmeBoard
{
public:

  Tdc(VmeController* controller,int address=0x00120000);

  // Gets data from TDC
  TDCEvent getEvent();

  // Get the TDC status
  unsigned int getStatus();

  // Reset the board
  void Reset();

  // Set the acquisition mode : Trigger (1), Continuous (0).
  void setMode(bool Trig = 1);

  // Set the maximum number of hits the TDC should memorise in a window
  // Possible values are : -0,1,2,4,8,16,32,64,128, -256 for no limit
  // the TDC sends an errorcode if the number is exceeded, but 'getEvent()' doesn't take account of it.
  void setMaxEvPerHit(int Max_ev_per_hit =256 );

  // Set the window width in multiples of clockcycle.
  void setWindowWidth(unsigned int WidthSetting);

  // Set the difference between the beginning of the window and the external trigger in multiples of clockcycle.
  void setWindowOffset(int OffsetSetting);

  // Set the extra search margin in units of clock cycle.
  void setExSearchMargin(int ExSearchMrgnSetting );

  // Set the reject margin in units of clock cycle.
  void setRejectMargin(int RejectMrgnSetting);

  // Get the current match window settings 
  void readWindowConfiguration();

  // Print the resolution
  // 0 -> 800ps, 1 -> 200ps, 2 -> 100ps
  // works only in leading/trailing edge detection mode
  void readResolution();

  // Enables FIFO  
  void enableFIFO();

  //  Disable extra information send from the TDC. 
  void disableTDCHeaderAndTrailer();

  // Write  a command line of 16 bit in the Micro Controller register.
  void writeOpcode(unsigned int &data);

  // Read a 16 bit word in the Micro Controller register.
  void readOpcode(unsigned int &data);

private:

  //REGISTER ADRESSES (must depend on add)
  int Opcode;
  int StatusRegister;
  int MicroHandshake;
  int OutputBuffer;
  int EventFIFO;
  int ControlRegister;

  //PRIVATE FUNCTIONS
  void waitWrite();
  void waitRead();
  void waitDataReady();
};

#endif
