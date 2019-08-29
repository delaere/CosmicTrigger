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
#include <set>

// Simple class to represent the status word.
class SY527StatusWord
{
public:
  
  // meaning of the status bits
  typedef enum CVStatusWordBit {
    cvPRESENT = 0, /* The channel is present or not */
    cvABSORBS = 3, /* Channel absorbs current (0=delivers) */
    cvEXDIS   = 4, /* External disable */
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
    cvPOWER   = 27,/* Power flag */
    cvPWDREQ  = 28,/* Password required for console operations */
    cvPOWDOWN = 29,/* Power down behavior: Kill(0) or RDW(1) */
    cvOOEN    = 30,/* On/Off enabled from console */
    cvPOWON   = 31,/* Power-on behavior: off(0) or on(1) */
  } CVStatusWordBit;
  
  SY527StatusWord(uint32_t status):status_(status) {}
  ~SY527StatusWord() {}
  
  // return the status word
  inline uint32_t status() const { return status_; }
  
  // extract a given bit
  inline bool bit(SY527StatusWord::CVStatusWordBit n) const { return ((status_>>n)&1); }
  
private:
  uint32_t status_;
};

class SY527PowerSystem;
class ChannelGroup;

// One single HV channel.
class SY527HVChannel: public HVChannel
{
public:
  explicit SY527HVChannel(uint32_t address, const HVBoard& board, uint32_t id, uint8_t type, CaenetBridge* bridge);
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
  
  // set the channel name
  void setName(std::string name);
  
  // readOperationalParameters should be called first.
  inline SY527StatusWord getStatus() const { return status_; }
  
  // channel priority in a group
  inline unsigned int getPriorityON() const { return priorityOn_; }
  inline unsigned int getPriorityOFF() const { return priorityOff_; }

private:
  std::string name_;
  uint8_t priorityOn_,priorityOff_;
  
  inline uint16_t chAddress() const { return ((board_->getSlot()<<8) + id_); }
  
  inline void setPriorityON( uint8_t priority) { priorityOn_ = priority; }
  inline void setPriorityOFF( uint8_t priority) { priorityOff_ = priority; }
  
  friend class SY527PowerSystem;
  friend class ChannelGroup;
};

class SY527PowerSystem: public HVModule 
{
public:
  
  typedef enum Checksum {
    correctChecksum  = 0,
    wrongHdrChecksum = 1,
    wrongFwChecksum  = 2,
    wrongChecksum    = 3,
    boardAbsent      = 0xFF
  } Checksum;
  
  explicit SY527PowerSystem(uint32_t address, CaenetBridge* bridge);
  ~SY527PowerSystem() {}
  
  void updateStatus();
  
  // checks the hardware status (result of a self-test)
  uint16_t getHWStatus();
  
  // performs a self-test
  void selfTest(bool alwaysRestart = true);
  
  // access groups of channels
  ChannelGroup getGroup(uint n);
  
  // read the general status
  uint32_t getGeneralStatus();
  
  // program alarms
  void programAlarms(bool levelHigh, bool pulsedAlarm, bool OVCalarm, bool OVValarm, bool UNValarm);
  
  // format the CPU EEPROM
  void formatEEPROM();
  
  // clear alarms
  void clearAlarm();
  
  // (un)lock the keyboard
  void lockKeyboard(bool lock=true);
  
  // kill all channels
  void killAll();
  
  // checksums
  std::vector<SY527PowerSystem::Checksum> checksum(bool current = true);
  
protected:
  // method to populate the boards map
  virtual void discoverBoards() override;
  
  // method to check that the returned identity is ok for this class
  virtual void assertIdentity() const override;

};

class ChannelGroup
{
public:
  virtual ~ChannelGroup() {}
  
  // types
  typedef std::vector<SY527HVChannel*>::value_type      value_type;
  typedef std::pair<uint32_t,uint32_t>                  key_type;
  typedef std::vector<SY527HVChannel*>::reference       reference;
  typedef std::vector<SY527HVChannel*>::const_reference const_reference;
  typedef std::vector<SY527HVChannel*>::iterator        iterator;
  typedef std::vector<SY527HVChannel*>::const_iterator  const_iterator; 
  typedef std::vector<SY527HVChannel*>::size_type       size_type;
  typedef std::vector<SY527HVChannel*>::difference_type difference_type;
  typedef std::vector<SY527HVChannel*>::allocator_type  allocator_type;
  typedef std::vector<SY527HVChannel*>::pointer         pointer;
  typedef std::vector<SY527HVChannel*>::const_pointer   const_pointer;

  // iterators
  inline iterator       begin() noexcept { return channels_.begin(); }
  inline const_iterator begin() const noexcept { return channels_.begin(); }
  inline iterator       end() noexcept { return channels_.end(); }
  inline const_iterator end() const noexcept { return channels_.end(); }
 
  // size
  inline size_type size() { return channels_.size(); }
  
  // element access
  
  iterator       find(const value_type& x);
  const_iterator find(const value_type& x) const;
  inline size_type      count(const value_type& x) const { return find(x)!=end(); }
  
  iterator       find(const key_type& x);
  const_iterator find(const key_type& x) const;
  inline size_type      count(const key_type& x) const { return find(x)!=end(); }
  
  inline reference       operator[](size_type n) { return channels_.operator[](n); }
  inline const_reference operator[](size_type n) const { return channels_.operator[](n); }
  inline reference       at(size_type n) { return channels_.at(n); }
  inline const_reference at(size_type n) const { return channels_.at(n); }
  inline reference       front() { return channels_.front(); }
  inline const_reference front() const { return channels_.front(); }
  inline reference       back() { return channels_.back(); }
  inline const_reference back() const { return channels_.back(); }
  
  // modifiers
  std::pair<iterator,bool> insert(const value_type& x);
  iterator erase(const_iterator position);
  size_type erase(const value_type& x);
  size_type erase(const key_type& x);
  iterator erase(const_iterator first, const_iterator last);
  
  // Group name
  inline void setName(std::string name) { name_=name; name_.resize(name_.size()<12 ? name_.size() : 11); setGroupName(); }
  inline std::string getName() const { return name_; }
  
  // Read Vmon/Status/Imon of Channels in a Group
  void readParameters();
  
  // Read V0set/I0set, V1set/I1set, Vmax/ITrip, Rup/Rdwn of Channels in a Group
  void readSettings();
  
  // set all channeles in the group
  void setV0(uint32_t v0);
  void setV1(uint32_t v1);
  void setI0(uint32_t i0);
  void setI1(uint32_t i1);
  void setRampup(uint32_t rampup);
  void setRampdown(uint32_t rampdown);
  void setTrip(uint32_t trip);
  void setSoftMaxV(uint32_t maxv);
  void on();
  void off();
  
  // set channel priority
  void setPriorityON(const value_type& channel, uint16_t priority);
  void setPriorityOFF(const value_type& channel, uint16_t priority);
  void setPriorityON(const key_type& channel, uint16_t priority);
  void setPriorityOFF(const key_type& channel, uint16_t priority);
  void setPriorityON(uint16_t priority);
  void setPriorityOFF(uint16_t priority);
  
private:
  ChannelGroup(uint id, std::string name, CaenetBridge* bridge, uint32_t address);
  
  void setGroupName();
  void addChannel(uint16_t num);
  void removeChannel(uint16_t num);
  
  uint id_;
  std::string name_;
  CaenetBridge* bridge_;
  uint32_t address_;
  std::vector<SY527HVChannel*> channels_;
  
  friend SY527PowerSystem;
};

#endif //SY527PowerSystem
