#ifndef __VMECONTROLLER
#define __VMECONTROLLER

#include "CommonDef.h"
#include "CAENVMEtypes.h"

#include <iostream>
#include <cmath>
#include <sstream>
#include <stdlib.h>
#include <unistd.h>

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
  virtual   void setMode(CVAddressModifier AM, CVDataWidth DW)=0;///<Sets default modes.
  virtual   int writeData(long unsigned int address,void* data) const = 0;///<Short write data function using default modes.
  virtual   int readData (long unsigned int address,void* data) const = 0;///<Short read data function using default modes.

  int verbose; ///<Defines verbosity level of any board using this controller
  
  void setVerbose(int verbose){this->verbose=verbose;}///< Sets verbosity level
  
  virtual CVAddressModifier getAM(void) const = 0;///<Gets default mode
  virtual CVDataWidth getDW(void) const = 0;///<Gets default mode
  
};

#endif
