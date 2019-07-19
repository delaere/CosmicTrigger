#ifndef __SY527HVMODULE
#define __SY527HVMODULE

#include "HVmodule.h"

// Simple class to represent the status word.
class SY527StatusWord
{
public:
  
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
