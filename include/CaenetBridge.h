#ifndef __VMECAENETBOARD
#define __VMECAENETBOARD

#include "VmeBoard.h"
#include <vector>

/// V288 CAEN board
class caenetBridge:public vmeBoard{
public:
  caenetBridge(vmeController *cont, uint32_t bridgeAdd, uint8_t interrupt = 0);
  
  void reset(); ///< Performs a reset
  
  bool validStatus(); ///< Valid operation? (returns the opposite of the status register LSB)
  
  void write(std::vector<uint32_t>& data); ///< send data to caenet
  
  uint32_t readResponse(std::vector<uint32_t>& data); ///< read back the response from the slave

private:
  uint8_t interrupt_;
  
};

#endif //__VMECAENETBOARD
