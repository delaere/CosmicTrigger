#ifndef __N470HVMODULE
#define __N470HVMODULE

#include "HVmodule.h"

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

class N470HVModule;

// One single HV channel.
class N470HVChannel: public HVChannel
{
public:
  explicit N470HVChannel(uint32_t address, HVBoard& board, uint32_t id, CaenetBridge* bridge);
  virtual ~N470HVChannel() {}
  
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
  
  // read all parameters from hardware
  void readOperationalParameters() override;
  
  // readOperationalParameters should be called first.
  inline N470StatusWord getStatus() const { return status_; }
  
protected:
  void setStatus(std::vector<uint32_t>::const_iterator data);
  
private:
  uint32_t status_;
  
  friend class N470HVModule;
};

class N470HVModule: public HVModule 
{
public:
  explicit N470HVModule(uint32_t address, CaenetBridge* bridge);
  ~N470HVModule() {}
  
  void updateStatus();
  inline uint32_t getStatus() const { return status_; }
  
  void kill();
  void clearAlarm();
  void enableKeyboard(bool enable);
  void setTTL();
  void setNIM();
  
protected:
  // method to populate the boards map
  virtual void discoverBoards() override;
  
  // method to check that the returned identity is ok for this class
  virtual void assertIdentity() const override;
  
private:
  uint32_t status_; //TODO this might not exist...
};

#endif //N470HVModule
