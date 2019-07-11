#ifndef __VMECONTROLLER
#define __VMECONTROLLER

#include "CommonDef.h"
#include "CAENVMEtypes.h"

//#include <iostream>
//#include <cmath>
//#include <sstream>
//#include <stdlib.h>
//#include <unistd.h>

/**
 * \brief Main controller virtual class.
 * 
 * This virtual class contains the definitions of the basic i/o functions for any VME system.
 * 
 * It has been created in order to be able to control any VME based system and therefore, the functions are pure virtual.
 * 
 */
//TODO rewrite this by transfering some of the (new) functions from the USB bridge class.
//TODO also review the conventions
class vmeController{
public:
  
  virtual void setMode(const CVAddressModifier AM, const CVDataWidth DW);///<Sets default modes.
  virtual const vmeController* mode(const CVAddressModifier AM, const CVDataWidth DW) const = 0;
  virtual CVAddressModifier getAM() const = 0;///<Gets default mode
  virtual CVDataWidth getDW() const = 0;///<Gets default mode
  virtual void setAM(CVAddressModifier AM) = 0;
  virtual void setDW(CVDataWidth DW) = 0;
  
  virtual void writeData(long unsigned int address,void* data) const = 0;///<Short write data function using default modes.
  virtual void readData (long unsigned int address,void* data) const = 0;///<Short read data function using default modes.

  void setVerbosity(int verbose){this->verbose_=verbose;}///< Sets verbosity level
  int verbosity() { return verbose_; }

protected:
  int verbose_; ///<Defines verbosity level of any board using this controller
};

#endif
