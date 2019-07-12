#ifndef __VMECONTROLLER
#define __VMECONTROLLER

#include "CommonDef.h"
#include "CAENVMEtypes.h"
#include <tuple>

/**
 * \brief Main controller virtual class.
 * 
 * This virtual class contains the definitions of the basic i/o functions for any VME system.
 * 
 * It has been created in order to be able to control any VME based system and therefore, the functions are pure virtual.
 * 
 */

class vmeController{
  public:
    vmeController(int verbose);
    virtual ~vmeController() {}
    
    virtual void setMode(const CVAddressModifier AM, const CVDataWidth DW);///<Sets default modes.
    virtual const vmeController* mode(const CVAddressModifier AM, const CVDataWidth DW) const;///<Sets temp mode
    virtual CVAddressModifier getAM() const;///<Gets default mode
    virtual CVDataWidth getDW() const;///<Gets default mode
    virtual void setAM(CVAddressModifier AM);///<Sets default modes.
    virtual void setDW(CVDataWidth DW);///<Sets default modes.
  
    virtual void writeData(long unsigned int address,void* data) const = 0;///<Short write data function using default modes.
    virtual void readData (long unsigned int address,void* data) const = 0;///<Short read data function using default modes.
    virtual void readWriteData(const long unsigned int address,void* data) const = 0;
    virtual void blockReadData(const long unsigned int address,unsigned char *buffer, int size, int *count, bool multiplex=false) const = 0;
    virtual void blockWriteData(const long unsigned int address,unsigned char *buffer, int size, int *count, bool multiplex=false) const = 0;
    virtual void ADOCycle(const long unsigned int address) const = 0;

    inline void setVerbosity(int verbose){this->verbose_=verbose;}///< Sets verbosity level
    inline int  verbosity() const { return verbose_; }///<Gets verbosity level
    inline bool verbosity(coutLevel level) const { return (verbose_ >= (int)level) ; }

  protected:
    virtual std::tuple<CVAddressModifier,CVDataWidth> useMode() const;///< more than a getter: it "consumes" the tmp mode.
    
  private:
    int verbose_; ///<Defines verbosity level of any board using this controller
    CVAddressModifier AM_;
    CVDataWidth DW_;
    mutable CVAddressModifier AMtmp_;
    mutable CVDataWidth DWtmp_;
};

#endif
