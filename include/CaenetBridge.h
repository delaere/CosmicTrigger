#ifndef __VMECAENETBOARD
#define __VMECAENETBOARD

#include "VmeBoard.h"
#include <vector>

/// V288 CAEN board
class CaenetBridge:public VmeBoard{
public:
  CaenetBridge(VmeController *cont, uint32_t bridgeAdd, uint8_t interrupt = 0);
  
  void reset(); ///< Performs a reset
  
  bool validStatus(); ///< Valid operation? (returns the opposite of the status register LSB)
  
  void write(const std::vector<uint32_t>& data); ///< send data to caenet
  
  std::tuple<uint32_t, std::vector<uint32_t> > readResponse(); ///< read back the response from the slave

private:
  uint8_t interrupt_;
  
};

#endif //__VMECAENETBOARD
