#ifndef __HVMODULE
#define __HVMODULE

#include "CaenetBridge.h"

// meaning of the status bits
typedef enum CVStatusWordBit {
  cvONOFF = 0,  /* The channel is ON(1) or OFF(0) */ 
  cvOVC   = 1,  /* The channel is in OVC condition */
  cvOVV   = 2,  /* The channel is in OVV condition */
  cvUNV   = 3,  /* The channel is in UNV condition */
  cvTRIP  = 4,  /* The channel has been switched OFF for TRIP condition */
  cvRUP   = 5,  /* The channel is ramping up */
  cvRDW   = 6,  /* The channel is ramping down */
  cvMAXV  = 7,  /* The channel has reached the preset MAXV */
  cvPOL   = 8,  /* Positive channel(0) or Negative channel(1) */
  cvVSEL  = 9,  /* Vset = V0 or Vset = V1 */
  cvISEL  = 10, /* Iset = I0 or Iset = I1 */
  cvKILL  = 11, /* Module KILLed by external pulse still active */
  cvHVEN  = 12, /* Module enabled to supply HV by the front panel switch */
  cvNIMTTL= 13, /* NIM(0) or TTL(1) standard selected */
  cvOUTCAL= 14, /* Non calibrated module  */
  cvALARM = 15  /* Module in alarm condition */
} CVStatusWordBit;

// Simple class to represent the status word.
class N470StatusWord
{
public:
  N470StatusWord(uint16_t status):status_(status) {}
  ~N470StatusWord() {}
  
  // return the status word
  inline uint16_t status() const { return status_; }
  
  // extract a given bit
  inline bool bit(CVStatusWordBit n) const { return ((status_>>n)&1); }
  
private:
  uint16_t status_;
};

class HVModule;

// One single HV channel.
class HVChannel
{
public:
  explicit HVChannel(uint32_t address, uint32_t id, CaenetBridge* bridge);
  ~HVChannel() {}
  
  // the channel id
  inline uint32_t id() const { return id_; }
  
  // turn on/off
  void on();
  void off();
  
  // program the channel
  void setV0(uint32_t v0);
  void setV1(uint32_t v1);
  void setI0(uint32_t i0);
  void setI1(uint32_t i1);
  void setRampup(uint32_t rampup);
  void setRampdown(uint32_t rampdown);
  void setTrip(uint32_t trip);
  
  // read all parameters from hardware
  void readOperationalParameters();
  
  // get cached values (as last programmed, or read back before)
  // readOperationalParameters should be called first.
  inline uint32_t getVmon() const { return vmon_; }
  inline uint32_t getImon() const { return imon_; }
  inline uint32_t getV0() const { return v0_; }
  inline uint32_t getV1() const { return v1_; }
  inline uint32_t getI0() const { return i0_; }
  inline uint32_t getI1() const { return i1_; }
  inline uint32_t getRampup() const { return rampup_; }
  inline uint32_t getRampdown() const { return rampdown_; }
  inline uint32_t getTrip() const { return trip_; }
  inline uint32_t getmaxV() const { return maxV_; }
  inline N470StatusWord getStatus() const { return status_; }
  
protected:
  void setStatus(std::vector<uint32_t>::const_iterator data);
  
private:
  CaenetBridge* bridge_;
  uint32_t address_;
  uint32_t id_;
  uint32_t vmon_;
  uint32_t imon_;
  uint32_t v0_;
  uint32_t v1_;
  uint32_t i0_;
  uint32_t i1_;
  uint32_t maxV_;
  uint32_t status_;
  uint32_t rampup_;
  uint32_t rampdown_;
  uint32_t trip_;
  
  friend class HVModule;
};

class HVModule{
public:
  explicit HVModule(uint32_t address, CaenetBridge* bridge);
  ~HVModule() {}
  
  inline std::string identification() const { return identification_; } 
  
  void updateStatus();
  
  void kill();
  void clearAlarm();
  void enableKeyboard(bool enable);
  void setTTL();
  void setNIM();
  
  inline HVChannel& channel(uint32_t id) { return channels_.at(id); }
  
  inline uint32_t getStatus() const { return status_; }
  
private:
  CaenetBridge* bridge_;
  uint32_t address_;
  std::vector<HVChannel> channels_;
  std::string identification_;
  uint32_t status_; //TODO this might not exist...
};

#endif
