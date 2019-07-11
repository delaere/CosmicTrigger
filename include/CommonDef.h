#ifndef __COMMONTYPEDEF
#define __COMMONTYPEDEF

//#include <iostream>
//#include <cmath>
//#include <sstream>
//#include <stdlib.h>
#include <exception>

#include "CAENVMElib.h"
#include "CAENVMEoslib.h"
#include "CAENVMEtypes.h"

//TODO only used by TDC... check and move there.
unsigned int digit(unsigned int data, int begin, int end);
unsigned int digit(unsigned int data, int digit);

/// Defines coutLevels to manage library output.
typedef enum coutLevel{
      SILENT = 0,	///<Shows nearly nothing, except fatal errors
      ERROR  =  1,	///<Shows errors
      WARNING = 2,	///<Shows warning (usually bad parameters)
      NORMAL = 3,	///<Normal level, explains what it's doing
      DEBUG = 4		///<Debug, it talks... A LOT
} coutLevel;

class CAENVMEexception: public std::exception
{
  public:
    CAENVMEexception(const CAENVME_API& errorcode) : errorcode_(errorcode) {}
    ~CAENVMEexception() {}

    virtual const char* what() const throw()
    {
      return CAENVME_DecodeError(errorcode_);
    }
  private:
    CAENVME_API errorcode_;
};

//TODO generalize the use of this macro
#define checkCAENVMEexception(x) {CAENVME_API status = (x); if (status) throw CAENVMEexception(status);}

#endif
