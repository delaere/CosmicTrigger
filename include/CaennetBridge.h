#ifndef __VMECAENETBOARD
#define __VMECAENETBOARD

#include "VmeBoard.h"

class caenetBridge:public vmeBoard{
public:
  caenetBridge(vmeController *cont, uint32_t bridgeAdd);
    
};

#endif //__VMECAENETBOARD
