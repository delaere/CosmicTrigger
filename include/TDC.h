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
#include "V1190Event.h"
#include <vector>
#include <sstream>
#include <bitset>
#include "time.h"

// Simple class to represent the control register.
class V1190ControlRegister
{
public:
  
  // meaning of the control bits
  typedef enum CVRegisterWordBit {
    cvBERREN  = 0, /* Bus Error enable bit. Used in D32 and Block Transfer mode */
    cvTERM    = 1, /* Set the software termination status */
    cvTERM_SW = 2, /* Allows to select the termination mode */
    cvEMPTYEVT= 3, /* Allows empty events */
    cvALIGN64 = 4, /* Align to 64 bits */
    cvCOMPEN  = 5, /* Compensation enable */
    cvTESTEN  = 6, /* Test FIFO enable */
    cvRCSEN   = 7, /* Read Compensation SRAM enable */
    cvFIFOEN  = 8, /* Event FIFO enable */
    cvEXTDTTEN= 9, /* Extended Trigger Time Tag enable */ 
    cv16MBMEB = 12,/* 16MB address range MEB access enable */ 
  } CVRegisterWordBit;
  
  V1190ControlRegister(uint32_t reg):register_(reg) {}
  ~V1190ControlRegister() {}
  
  // return the status word
  inline uint16_t registr() const { return register_; }
  
  // extract a given bit
  inline bool bit(V1190ControlRegister::CVRegisterWordBit n) const { return ((register_>>n)&1); }
  
  // set a bit
  inline void setBit(V1190ControlRegister::CVRegisterWordBit n, bool enable=true) { if (enable^bit(n)) register_ ^= 1 << n;}
  
private:
  uint16_t register_;
};

// Simple class to represent the status register.
class V1190StatusRegister
{
public:
  
  // meaning of the status bits
  typedef enum CVRegisterWordBit {
    cvDATA_READY= 0, /* Data present in the output buffer */
    cvALM_FULL  = 1, /* Almost full level reached */
    cvFULL      = 2, /* Output buffer full */
    cvTRG_MATCH = 3, /* Trigger matching mode */
    cvHEADER_EN = 4, /* TDC header enabled */
    cvTERM_ON   = 5, /* Termination on/off */
    cvERROR0    = 6, /* error in TDC 0 */
    cvERROR1    = 7, /* error in TDC 1 */
    cvERROR2    = 8, /* error in TDC 2 */
    cvERROR3    = 9, /* error in TDC 3 */ 
    cvBERR_FLAG = 10,/* Bus error */ 
    cvPURG      = 11,/* Purged */
    cvRES_0     = 12,/* Resolution LSB */
    cvRES_1     = 13,/* Resolution MSB */
    cvPAIR      = 14,/* Pair mode */
    cvTRIGLOST  = 15,/* Trigger lost */
  } CVRegisterWordBit;
  
  V1190StatusRegister(uint32_t reg):register_(reg) {}
  ~V1190StatusRegister() {}
  
  // return the status word
  inline uint16_t registr() const { return register_; }
  
  // extract a given bit
  inline bool bit(V1190StatusRegister::CVRegisterWordBit n) const { return ((register_>>n)&1); }
  
private:
  uint16_t register_;
};

// V1190 TDC unit
class Tdc:public VmeBoard
{
public:

  // module info
  struct ModuleInfo {
    uint64_t manufacturer_;
    uint64_t moduletype_;
    uint8_t version_;
    uint32_t serial_number_;
    uint32_t revision_minor_;
    uint32_t revision_major_;
    uint8_t firmwareVersion_;
  };
  
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
  
  // edge detection mode
  typedef enum CVEdgeDetection {
    cvPairMode = 0,
    cvTrailing = 1,
    cvLeading  = 2,
    cvBoth     = 3,
  } CVEdgeDetection;
  
  // edge detection resolution
  typedef enum CVEdgeLSB {
    cv800ps = 0,
    cv200ps = 1,
    cv100ps = 2,
  } CVEdgeLSB;
  
  // pair mode edge resolution
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
  
  // pair mode width resolution
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
  
  // dead time
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
  
  Tdc(VmeController* controller,int address=0x00120000);

  /////////////////////////
  //// Configuration, status, info, ...
  /////////////////////////
  
  // Get module info
  inline ModuleInfo getModuleInfo() const { return info_; }
  
  // Reads the control register
  V1190ControlRegister getControlRegister();
  
  // Writes the control register
  void setControlRegister(V1190ControlRegister& reg);
  
  // Enables FIFO  
  void enableFIFO(bool enable);
  
  // Enables BERR
  void enableBERR(bool enable);
  
  // Enables Extd trigger time
  void enableExtdTrigTime(bool enable);
  
  // Enables Compensation
  void enableCompensation(bool enable);

  // Get the TDC status
  V1190StatusRegister getStatus();
  
  // Program interrupt
  void setInterrupt(uint8_t level=0X0, uint16_t vector = 0xDD);

  // Reset the board
  void Reset(bool moduleReset=true, bool softClear=true, bool softEvtReset=true);
  
  // Generates a software trigger
  void trigger(); 
  
  // get the event count
  uint32_t eventCount();
  
  // get the stored event count
  uint16_t storedEventCount();
  
  // set the almost-full level
  void setAlmostFullLevel(uint16_t level);
  
  // get the almost-full level
  uint16_t getAlmostFullLevel();
  
  // reads the event FIFO
  std::pair<uint16_t,uint16_t> readFIFO();
  
  // reads the FIFO count
  uint16_t getFIFOCount();
  
  // reads the FIFO status
  uint8_t getFIFOStatus();
  
  /////////////////////////
  //// Read data from the board
  /////////////////////////
  
  // Gets data from TDC in trigger mode
  V1190Event getEvent(bool useFIFO=true);

  // Gets data from TDC in trigger mode
  std::vector<V1190Event> getEvents(bool useFIFO=true);
  
  // Gets data from TDC in countinuous mode
  TDCHit getHit();

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

private:

  //REGISTER ADRESSES (must depend on add)
  int opcode_;
  int statusRegister_;
  int microHandshake_;
  int outputBuffer_;
  int eventFIFO_;
  int controlRegister_;
  
  // Module info
  ModuleInfo info_;

  //PRIVATE FUNCTIONS
  void waitWrite();
  void waitRead();
  void waitDataReady();
};

#endif
