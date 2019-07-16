#ifndef __VMECONTROLLER
#define __VMECONTROLLER

#include "CommonDef.h"
#include "CAENVMEtypes.h"
#include <tuple>

// Main controller virtual class.
class VmeController{
  public:
    VmeController();
    virtual ~VmeController() {}
    
    // Address modifier and Data width
    virtual void setMode(const CVAddressModifier AM, const CVDataWidth DW);///<Sets default modes.
    virtual const VmeController* mode(const CVAddressModifier AM, const CVDataWidth DW) const;///<Sets temp mode
    virtual CVAddressModifier getAM() const;///<Gets default mode
    virtual CVDataWidth getDW() const;///<Gets default mode
    virtual void setAM(CVAddressModifier AM);///<Sets default modes.
    virtual void setDW(CVDataWidth DW);///<Sets default modes.
  
    // VME BUS operations
    virtual void writeData(long unsigned int address,void* data) const = 0;///<Short write data function using default modes.
    virtual void readData (long unsigned int address,void* data) const = 0;///<Short read data function using default modes.
    virtual void readWriteData(const long unsigned int address,void* data) const = 0;
    virtual void blockReadData(const long unsigned int address,unsigned char *buffer, int size, int *count, bool multiplex=false) const = 0;
    virtual void blockWriteData(const long unsigned int address,unsigned char *buffer, int size, int *count, bool multiplex=false) const = 0;
    virtual void ADOCycle(const long unsigned int address) const = 0;
    
    // IRQ operations
    virtual void IRQEnable(uint32_t mask) const = 0;
    virtual void IRQDisable(uint32_t mask) const = 0;
    virtual void IRQWait(uint32_t mask, uint32_t timeout_ms) const = 0;
    virtual unsigned char IRQCheck() const = 0;
    virtual uint16_t IACK(CVIRQLevels Level) const = 0;

  protected:
    virtual std::tuple<CVAddressModifier,CVDataWidth> useMode() const;///< more than a getter: it "consumes" the tmp mode.
    
  private:
    CVAddressModifier AM_;
    CVDataWidth DW_;
    mutable CVAddressModifier AMtmp_;
    mutable CVDataWidth DWtmp_;
};

#endif
