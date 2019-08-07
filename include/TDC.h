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
#include <bitset>
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

  // acquisition modes
  typedef enum CVAcquisitionMode {
    cvContinuous = 0,
    cvTrigger = 1,
  } CVAcquisitionMode;
  
  // configurations
  typedef enum CVConfiguration {
    cvDefault = 0,
    cvUser = 1,
  } CVConfiguration;
  
  typedef enum CVEdgeDetection {
    cvPairMode = 0,
    cvTrailing = 1,
    cvLeading  = 2,
    cvBoth     = 3,
  } CVEdgeDetection;
  
  typedef enum CVEdgeLSB {
    cv800ps = 0,
    cv200ps = 1,
    cv100ps = 2,
  } CVEdgeLSB;
  
  typedef enum CVPairModeEdgeLSB {
    cvpme100ps = 0,
    cvpme200ps = 1,
    cvpme400ps = 2,
    cvpme800ps = 3,
    cvpme1600ps = 4,
    cvpme3120ps = 5,
    cvpme6250ps = 6,
    cvpme12500ps = 7,
  } CVPairModeEdgeLSB;
  
  typedef enum CVPairModeWidthLSB {
    cvpmw100ps = 0,
    cvpmw200ps = 1,
    cvpmw400ps = 2,
    cvpmw800ps = 3,
    cvpmw1600ps = 4,
    cvpmw3200ps = 5,
    cvpmw6250ps = 6,
    cvpmw12500ps = 7,
    cvpmw25ns = 8,
    cvpmw50ns = 9,
    cvpmw100ns = 10,
    cvpmw200ns = 11,
    cvpmw400ns = 12,
    cvpmw800ns = 13,
  } CVPairModeWidthLSB;
  
  typedef enum CVDeadTime {
    cvdt5ns = 0,
    cvdt10ns = 1,
    cvdt30ns = 2,
    cvdt100ns = 3,
  } CVDeadTime;
  
  // trigger window configuration
  struct WindowConfiguration {
    uint16_t width; // ns
    uint16_t offset; // ns (signed coded on 12 bits; use helper below)
    uint16_t extraMargin;
    uint16_t rejectMargin;
    bool triggerTimeSubstraction;
    
    static uint16_t computeOffset(int16_t signedOffset) {
      assert(signedOffset>=-2048 && signedOffset<=40);
      return signedOffset<0 ? 0xF000+abs(signedOffset) : abs(signedOffset);
    }
  };
  
  Tdc(VmeController* controller,int address=0x00120000); //TODO

  // Gets data from TDC
  TDCEvent getEvent(); //TODO

  // Get the TDC status
  unsigned int getStatus(); //TODO

  // Reset the board
  void Reset(); //TODO
  
  // Enables FIFO  
  void enableFIFO(); //TODO

  /////////////////////////
  //// Generic opcode methods
  /////////////////////////
  
  // Write  a command line of 16 bit in the Micro Controller register.
  void writeOpcode(uint16_t &data);

  // Read a 16 bit word in the Micro Controller register.
  void readOpcode(uint16_t &data);

  /////////////////////////
  //// Acquisition mode
  /////////////////////////
  
  // Set the acquisition mode : Trigger (1), Continuous (0).
  void setAcquisitionMode(Tdc::CVAcquisitionMode mode = cvTrigger);
  
  // Get the acquisition mode
  Tdc::CVAcquisitionMode getAcquisitionMode();
  
  // Keep the token or not (TDC chip buffer access)
  void keepToken(bool keep=true);
  
  // save configuration
  void saveUserConfiguration();
  
  // load configuration
  void LoadConfiguration(Tdc::CVConfiguration conf);
  
  // set startup configuration
  void setStartupConfiguration(Tdc::CVConfiguration conf);

  /////////////////////////
  // Trigger
  /////////////////////////
  
  // set the trigger window configuration
  void setTriggerWindow(Tdc::WindowConfiguration &conf);
  
  // read the trigger window configuration
  Tdc::WindowConfiguration getTriggerWindow();
  
  /////////////////////////
  // TDC edge detection and resolution
  /////////////////////////
  
  // Set edge detection configuration
  void setEdgeDetectionConfiguration(Tdc::CVEdgeDetection conf);
  
  // Read edge detection configuration
  Tdc::CVEdgeDetection getEdgeDetectionConfiguration();
  
  // Set LSB of leading/trailing edge
  void setEdgeLSB(Tdc::CVEdgeLSB lsb);
  
  // Set leading time and width resolution when pair
  void setPairResolution(Tdc::CVPairModeEdgeLSB edge, Tdc::CVPairModeWidthLSB width);
  
  // Read resolution 
  Tdc::CVEdgeLSB getResolution();
  std::pair<Tdc::CVPairModeEdgeLSB, Tdc::CVPairModeWidthLSB> getPairResolution();
  
  // set dead time
  void setDeadTime(Tdc::CVDeadTime dt);
  
  // get dead time
  Tdc::CVDeadTime getDeadTime();
  
  /////////////////////////
  // TDC Readout 
  /////////////////////////
  
  // enable/disable the TDC header and trailer
  void enableTDCHeader(bool enable);
  
  // check if TDC header and trailer are enabled
  bool isTDCHeaderEnabled();
  
  // set the maximum number of hits per event
  // can be 0, a power of 2 (up to 128) or -1 (infinity)
  void setMaxHitsPerEvent(int numHits);
  
  // get the maximum number of hits per event (-1 = no limits)
  int getMaxHitsPerEvent();
  
  // configure TDC readout. Refer tp the manual for the meaning of internalErrorTypes and fifoSize
  void configureTDCReadout(bool enableErrorMask, bool enableBypass, uint16_t internalErrorTypes, uint16_t fifoSize);
  
  // Read enabled TDC internal error internalErrorTypes
  uint16_t getInternalErrorTypes();
  
  // Read effective size of readout FIFO
  uint16_t getFifoSize();
  
  /////////////////////////
  // Channel enable/disable
  /////////////////////////

  // Enable/Disable channel. 128 or higher means "all"
  void enableChannel(uint8_t channel, bool enable);
  
  // Write Enable pattern
  void writeEnablePattern(std::bitset<128> &pattern);
  
  // Read Enable pattern
  std::bitset<128> readEnablePattern();
  
  /////////////////////////
  // Other Opcodes
  /////////////////////////

  // Adjust opcode, Misc opcodes and Advanced opcode are not implemented.
  // The user should refer to the manual and use directly the writeOpcode and readOpcode methods.

  /////////////////////////


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
