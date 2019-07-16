#ifndef __VMECAENETBOARD
#define __VMECAENETBOARD

#include "VmeBoard.h"
#include <vector>

// V288 CAEN board
class CaenetBridge:public VmeBoard{
public:
  CaenetBridge(VmeController *cont, uint32_t bridgeAdd, uint8_t interrupt = 0);
  
  // Performs a reset
  void reset();
  
  // Check if the last operation was valid (returns the opposite of the status register LSB)
  bool validStatus();
  
  // Send data to caenet
  void write(const std::vector<uint32_t>& data);
  
  // Read back the response from the slave
  std::tuple<uint32_t, std::vector<uint32_t> > readResponse();

private:
  uint8_t interrupt_;
  
};

#endif //__VMECAENETBOARD
