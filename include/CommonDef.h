#ifndef __COMMONTYPEDEF
#define __COMMONTYPEDEF

#include <exception>
#include <utility>
#include <tuple>
#include <execinfo.h>
#include <vector>

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
    explicit CAENVMEexception(const CAENVME_API& errorcode) : errorcode_(errorcode) {
        void *array[10];
        size_t size;
        char **strings;
        size_t i;

        size = backtrace (array, 10);
        strings = backtrace_symbols (array, size);

        for (i = 0; i < size; i++)
          trace_.push_back(strings[i]);

        free (strings);
    }
    
    ~CAENVMEexception() {}

    virtual const char* what() const throw()
    {
      return CAENVME_DecodeError(errorcode_);
    }
    
    const std::vector<std::string>& trace() const 
    {
      return trace_;
    }
    
  private:
    CAENVME_API errorcode_;
    std::vector<std::string> trace_;
};

#define checkCAENVMEexception(x) {CAENVME_API status = (x); if (status) throw CAENVMEexception(status);}

class CAENETexception: public std::exception
{
  public:
    explicit CAENETexception(const uint32_t& errorcode) : errorcode_(errorcode) {
        void *array[10];
        size_t size;
        char **strings;
        size_t i;

        size = backtrace (array, 10);
        strings = backtrace_symbols (array, size);

        for (i = 0; i < size; i++)
          trace_.push_back(strings[i]);

        free (strings);
    }

    ~CAENETexception() {}

    virtual const char* what() const throw()
    {
      if(errorcode_==0x0000) return "Successful operation.";
      if(errorcode_==0xFF00) return "BUSY module (it has tried to effect an operation while the module is still busy registering previous data inside the EEPROM).";
      if(errorcode_==0xFF01) return "Code not recognised or message incorrect.";
      if(errorcode_==0xFF02) return "Incorrect set value.";
      if(errorcode_==0xFFFD) return "No data to be transmitted.";
      if(errorcode_==0xFFFE) return "The H.C. Controller identifier is incorrect.";
      if(errorcode_==0xFFFF) return "The addressed module does not exist.";
      return "Unknown exception!";
    }
    
    const std::vector<std::string>& trace() const 
    {
      return trace_;
    }
    
  private:
    uint32_t errorcode_;
    std::vector<std::string> trace_;
};

#define checkCAENETexception(x) {uint32_t status = (x); if (status) throw CAENETexception(status); }

//utilities

template <typename V, size_t... Is>
auto gen_tuple_impl(V vec, std::index_sequence<Is...> ) {
    return std::make_tuple(vec.at(Is)...);
}

template <size_t N, typename V>
auto gen_tuple(V vec) {
    return gen_tuple_impl(vec, std::make_index_sequence<N>{} );
}

#endif
