#ifndef __VMEBOARD
#define __VMEBOARD

#include "CommonDef.h"
#include "CAENVMEtypes.h"
#include "VmeController.h"

  /**
   * \brief Mother class for any board type class.
   * 
   * This class should be used to create any daughter board class.
   * It is used to store basic communication functions with a virtual vmeController and to store associated default values.
   * 
   * In enforced mode, the controller will always operate in the designated AM/DW mode if the proxy read/write functions are used.
   *
   */

class vmeBoard{
  
public:
  vmeBoard(vmeController* cont, uint32_t baseAddress, CVAddressModifier AM=cvA32_U_DATA, CVDataWidth DW=cvD16, bool enforceAMDW=false);
  virtual ~vmeBoard() {}
  
  inline void enforceAMDW(bool enforce) { enforceAMDW_ = enforce; }
  inline bool isAMDWenforced() { return enforceAMDW_; }
  
protected:
  inline const vmeController* controller() const { return (enforceAMDW_ ? cont_->mode(AM_,DW_) : cont_); }
  
  inline bool verbosity(coutLevel level) const { return cont_->verbosity(level); }

  inline void setAdd(uint32_t add) { baseAddress_=add; }
  inline uint32_t baseAddress() const { return baseAddress_; }
  
  inline void writeData(long unsigned int address,void* data) const { 
    controller()->writeData(address,data); 
  }
  inline void readData (long unsigned int address,void* data) const { 
    controller()->readData(address,data); 
  }
  inline void readWriteData(const long unsigned int address,void* data) const { 
    controller()->readWriteData(address,data); 
  }
  inline void blockReadData(const long unsigned int address,unsigned char *buffer, int size, int *count, bool multiplex=false) const { 
    controller()->blockReadData(address, buffer, size, count, multiplex); 
  }
  inline void blockWriteData(const long unsigned int address,unsigned char *buffer, int size, int *count, bool multiplex=false) const {
    controller()->blockWriteData(address, buffer, size, count, multiplex); 
  }
  inline void ADOCycle(const long unsigned int address) const {
    controller()->ADOCycle(address);
  }

private:
  vmeController* cont_;  ///< Pointer to the controller
  CVAddressModifier AM_; ///< Stored AM value
  CVDataWidth DW_;       ///< Stored DW value
  bool enforceAMDW_;     ///< Should the mode be enforced ?
  uint32_t baseAddress_; ///< The base address of the board
};

#endif
