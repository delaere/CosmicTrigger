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

#ifndef __SY527HVMODULE
#define __SY527HVMODULE

#include "HVmodule.h"

// Simple class to represent the status word.
class SY527StatusWord
{
public:
  
  // meaning of the status bits
  typedef enum CVStatusWordBit {
    cvPRESENT = 0, /* The channel is present or not */
    cvINTRIP  = 5, /* The channel has been switched OFF for internal TRIP condition */
    cvKILL    = 6, /* Module KILLed by external pulse still active */
    cvMAXV    = 8, /* The channel has reached the preset MAXV */
    cvEXTRIP  = 9, /* The channel has been switched OFF for external TRIP */
    cvOVV     = 10,/* The channel is in OVV condition */
    cvUNV     = 11,/* The channel is in UNV condition */
    cvOVC     = 12,/* The channel is in OVC condition */
    cvRDW     = 13,/* The channel is ramping down */
    cvRUP     = 14,/* The channel is ramping up */ 
    cvONOFF   = 15,/* The channel is ON(1) or OFF(0) */ 
    cvEXTRIPEN= 25,/* The external trip feature is enabled */
    cvPWDREQ  = 28,/* Password required for console operations */
    cvPOWDOWN = 29,/* Power down behavior: Kill(0) or RDW(1) */
    cvOOEN    = 30,/* On/Off enabled from console */
    cvPOWON   = 31,/* Power-on behavior: off(0) or on(1) */
  } CVStatusWordBit;
  
  SY527StatusWord(uint16_t status):status_(status) {}
  ~SY527StatusWord() {}
  
  // return the status word
  inline uint16_t status() const { return status_; }
  
  // extract a given bit
  inline bool bit(SY527StatusWord::CVStatusWordBit n) const { return ((status_>>n)&1); }
  
private:
  uint16_t status_;
};

class SY527PowerSystem;

// One single HV channel.
class SY527HVChannel: public HVChannel
{
public:
  explicit SY527HVChannel(uint32_t address, HVBoard& board, uint32_t id, CaenetBridge* bridge);
  virtual ~SY527HVChannel() {}
  
  // turn on/off
  void on() override;
  void off() override;
  
  // program the channel
  void setV0(uint32_t v0) override;
  void setV1(uint32_t v1) override;
  void setI0(uint32_t i0) override;
  void setI1(uint32_t i1) override;
  void setRampup(uint32_t rampup) override;
  void setRampdown(uint32_t rampdown) override;
  void setTrip(uint32_t trip) override;
  void setSoftMaxV(uint32_t maxv) override;
  void setPasswordFlag(bool flag);
  void setOnOffFlag(bool flag);
  void setPoweronFlag(bool flag);
  
  // read all parameters from hardware
  void readOperationalParameters() override;
  
  // get the channel name (call readOperationalParameters first)
  inline std::string getName() const { return name_; }
  
  // readOperationalParameters should be called first.
  inline SY527StatusWord getStatus() const { return status_; }

private:
  uint32_t status_; 
  std::string name_;
  
  inline uint16_t chAddress() const { return ((board_->getSlot()<<8) + id_); }
  
  friend class SY527PowerSystem;
};

class SY527PowerSystem: public HVModule 
{
public:
  explicit SY527PowerSystem(uint32_t address, CaenetBridge* bridge);
  ~SY527PowerSystem() {}
  
  void updateStatus();

protected:
  // method to populate the boards map
  virtual void discoverBoards() override;
  
  // method to check that the returned identity is ok for this class
  virtual void assertIdentity() const override;

};

#endif //SY527PowerSystem
