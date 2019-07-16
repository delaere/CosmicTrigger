#ifndef __UsbVmeBridge
#define __UsbVmeBridge

#include <iostream>
#include <tuple>

#include "CommonDef.h"
#include "VmeController.h"

// the internal pulser of the V1718
class V1718Pulser
{
  public:
    V1718Pulser(uint32_t handle, CVPulserSelect id);
    ~V1718Pulser() {}

    void setPeriod(unsigned char period) { this->period_ = period; this->configured_ = false; }
    unsigned char getPeriod() const { return this->period_; }
    void setWidth(unsigned char width) {this->width_ = width; this->configured_ = false; }
    unsigned char  getWidth() const { return this->width_; }
    void setUnits(CVTimeUnits units) { this->units_ = units; this->configured_ = false; }
    CVTimeUnits getUnits() const { return this->units_; }
    void setPulseNo(unsigned char no) { this->pulseNo_ = no; this->configured_ = false; }
    unsigned char getPulseNo() const { return this->pulseNo_; }
    void setStartSource(CVIOSources source) { this->start_ = source; this->configured_ = false; }
    CVIOSources getStartSource() const { return this->start_; }
    void setStopSource(CVIOSources source) { this->reset_ = source; this->configured_ = false; }
    CVIOSources getStopSource() const { return this->reset_; }

    void configure() const;
    void update();
    
    void start() const;
    void stop() const;

  private:
    uint32_t BHandle_;
    CVPulserSelect pulserId_;
    unsigned char period_;
    unsigned char width_;
    CVTimeUnits units_;
    unsigned char pulseNo_;
    CVIOSources start_;
    CVIOSources reset_;
    mutable bool configured_;
};

// the internal scaler of the V1718
class V1718Scaler
{
  public:
    explicit V1718Scaler(uint32_t handle);
    ~V1718Scaler() {}
  
    void setLimit(short limit) { this->limit_ = limit; this->configured_=false; }
    short getLimit() const { return this->limit_; }
    void setAutoReset(short autoReset) { this->autoReset_ = autoReset; this->configured_=false; }
    short getAutoReset() const { return this->autoReset_; }
    void setHitSource(CVIOSources source) {this->hit_ = source; this->configured_=false; }
    CVIOSources getHitSource() const { return this->hit_; }
    void setGateSource(CVIOSources source) {this->gate_ = source; this->configured_=false; }
    CVIOSources getGateSource() const { return this->gate_; }
    void setResetSource(CVIOSources source) { this->reset_ = source; this->configured_=false; }
    CVIOSources getResetSource() const { return this->reset_; }
    
    void configure();
    void update();
    
    void resetCount();
    void enableGate();
    void disableGate();
    unsigned short count() const;
  
  private:
    uint32_t BHandle_;
    short limit_;
    short autoReset_;
    CVIOSources hit_;
    CVIOSources gate_;
    CVIOSources reset_;
    bool configured_;
};

// V1718 VME USB bridge.
class VmeUsbBridge: public VmeController{
public:
     explicit VmeUsbBridge();
     VmeUsbBridge(const VmeUsbBridge& other); ///< copy constructor
     VmeUsbBridge& operator=(const VmeUsbBridge& other); ///< copy operator
     ~VmeUsbBridge();///< Liberates the USB controller and "BHandle    

     /* VME data cycles */
     void writeData(const long unsigned int address,void* data) const override;
     void readData (const long unsigned int address,void* data) const override;
     void readWriteData(const long unsigned int address,void* data) const override;
     void blockReadData(const long unsigned int address,unsigned char *buffer, int size, int *count, bool multiplex=false) const override;
     void blockWriteData(const long unsigned int address,unsigned char *buffer, int size, int *count, bool multiplex=false) const override;
     void ADOCycle(const long unsigned int address) const override;
     
     /* Pulser */
     V1718Pulser& getPulser(CVPulserSelect); 
     
     /* Scaler */
     V1718Scaler& getScaler();
     
     /* Input and output registers */
     void configureOutputLine(CVOutputSelect line, CVIOPolarity polarity, CVLEDPolarity LEDpolarity, CVIOSources source) const;
     void configureInputLine(CVInputSelect line, CVIOPolarity polarity, CVLEDPolarity LEDpolarity) const;
     std::tuple<CVIOPolarity, CVLEDPolarity, CVIOSources> outputLineConfiguration(CVOutputSelect line) const;
     std::tuple<CVIOPolarity, CVLEDPolarity> inputLineConfiguration(CVInputSelect line) const;
     uint32_t readRegister(CVRegisters reg) const;
     void setOutputRegister(unsigned short mask) const;
     void clearOutputRegister(unsigned short mask) const;
     void pulseOutputRegister(unsigned short mask) const;

     /* Read display */
     CVDisplay readDisplay() const;

     /* Behavior */
     void setArbiterType(CVArbiterTypes type) const;
     void setRequesterType(CVRequesterTypes type) const;
     void setReleaseType(CVReleaseTypes type) const;
     void setBusReqLevel(CVBusReqLevels level) const;
     void setTimeout(CVVMETimeouts timeout) const;
     void setFIFOMode(bool mode) const;
     CVArbiterTypes getArbiterType() const;
     CVRequesterTypes getRequesterType() const;
     CVReleaseTypes getReleaseType() const;
     CVBusReqLevels getBusReqLevel() const;
     CVVMETimeouts getTimeout() const;
     bool getFIFOMode() const;
     
     /* System reset */
     void systemReset() const;
     
     /* Interupts */
     void IRQEnable(uint32_t mask) const override;
     void IRQDisable(uint32_t mask) const override;
     void IRQWait(uint32_t mask, uint32_t timeout_ms) const override;
     unsigned char IRQCheck() const override;
     uint16_t IACK(CVIRQLevels Level) const override;
     
private:
  std::string firmwareVersion_;
  int32_t BHandle_;
  CVBoardTypes board_;

  V1718Pulser* pulserA_;
  V1718Pulser* pulserB_;
  V1718Scaler* scaler_;
};

#endif
