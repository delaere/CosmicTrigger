#ifndef __VMEBOARD
#define __VMEBOARD

#include "CommonDef.h"
#include "CAENVMEtypes.h"
#include "VmeController.h"

// a generic VME board
class VmeBoard{
  
public:
  VmeBoard(VmeController* cont, uint32_t baseAddress, CVAddressModifier AM=cvA32_U_DATA, CVDataWidth DW=cvD16, bool enforceAMDW=false);
  virtual ~VmeBoard() {}
  
  // Enforce the AM and DW. 
  // If set to true, the mode will be enforced for all VME operations, disregarding the VmeController configuration.
  inline void enforceAMDW(bool enforce) { enforceAMDW_ = enforce; }
  inline bool isAMDWenforced() { return enforceAMDW_; }
  
protected:
  // access to the VME controller. If AMDW is enforced, the returned object is ready.
  inline const VmeController* controller() const { return (enforceAMDW_ ? cont_->mode(AM_,DW_) : cont_); }

  // get/set the module base address
  inline void setbaseAddress(uint32_t add) { baseAddress_ = add; }
  inline uint32_t baseAddress() const { return baseAddress_; }
  
  // proxy for the controller operations.
  // it automatically uses the proper AM/DW mode if enforced
  
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
  VmeController* cont_;  ///< Pointer to the controller
  CVAddressModifier AM_; ///< Stored AM value
  CVDataWidth DW_;       ///< Stored DW value
  bool enforceAMDW_;     ///< Should the mode be enforced ?
  uint32_t baseAddress_; ///< The base address of the board
};

#endif
