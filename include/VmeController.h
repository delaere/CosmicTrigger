#ifndef __VMECONTROLLER
#define __VMECONTROLLER

#include "CommonDef.h"
#include "CAENVMEtypes.h"
#include <utility>

/**
 * \brief Main controller virtual class.
 * 
 * This virtual class contains the definitions of the basic i/o functions for any VME system.
 * 
 * It has been created in order to be able to control any VME based system and therefore, the functions are pure virtual.
 * 
 */
//TODO rewrite this by transfering some of the (new) functions from the USB bridge class.
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
    //TODO more data methods

    inline void setVerbosity(int verbose){this->verbose_=verbose;}///< Sets verbosity level
    inline int verbosity() { return verbose_; }

  protected:
    virtual std::pair<CVAddressModifier,CVDataWidth> useMode() const;///< more than a getter: it "consumes" the tmp mode.
    
  private:
    int verbose_; ///<Defines verbosity level of any board using this controller
    CVAddressModifier AM_;
    CVDataWidth DW_;
    mutable CVAddressModifier AMtmp_;
    mutable CVDataWidth DWtmp_;
};

#endif
