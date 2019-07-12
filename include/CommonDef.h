#ifndef __COMMONTYPEDEF
#define __COMMONTYPEDEF

#include <exception>

#include "CAENVMElib.h"
#include "CAENVMEoslib.h"
#include "CAENVMEtypes.h"

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
    explicit CAENVMEexception(const CAENVME_API& errorcode) : errorcode_(errorcode) {}
    ~CAENVMEexception() {}

    virtual const char* what() const throw()
    {
      return CAENVME_DecodeError(errorcode_);
    }
  private:
    CAENVME_API errorcode_;
};

#define checkCAENVMEexception(x) {CAENVME_API status = (x); if (status) throw CAENVMEexception(status);}

#endif
