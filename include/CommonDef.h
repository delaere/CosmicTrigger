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

// logger

#include "logger.h"

// exception classes

#include <boost/stacktrace.hpp>
#include <boost/exception/all.hpp>

typedef boost::error_info<struct tag_stacktrace, boost::stacktrace::stacktrace> traced;

template <class E>
void throw_with_trace(const E& e) {
    throw boost::enable_error_info(e)
        << traced(boost::stacktrace::stacktrace());
}

class CAENVMEexception: public std::exception
{
  public:
    explicit CAENVMEexception(const CAENVME_API& errorcode) : errorcode_(errorcode) { }
    
    ~CAENVMEexception() {}

    virtual const char* what() const throw()
    {
      return CAENVME_DecodeError(errorcode_);
    }
    
  private:
    CAENVME_API errorcode_;
};

#define checkCAENVMEexception(x) {CAENVME_API status = (x); if (status) throw_with_trace(CAENVMEexception(status));}

class CAENETexception: public std::exception
{
  public:
    explicit CAENETexception(const uint32_t& errorcode) : errorcode_(errorcode) { }

    ~CAENETexception() {}

    virtual const char* what() const throw()
    {
      if(errorcode_==0x0000) return "Successful operation.";
      if(errorcode_==0xFF00) return "BUSY module (it has tried to effect an operation while the module is still busy registering previous data inside the EEPROM).";
      if(errorcode_==0xFF01) return "Code not recognised or message incorrect.";
      if(errorcode_==0xFF02) return "Incorrect set value.";
      if(errorcode_==0xFF03) return "Channel or Board not present";
      if(errorcode_==0xFFFD) return "No data to be transmitted.";
      if(errorcode_==0xFFFE) return "The H.C. Controller identifier is incorrect.";
      if(errorcode_==0xFFFF) return "The addressed module does not exist.";
      return "Unknown exception!";
    }
    
  private:
    uint32_t errorcode_;
};

#define checkCAENETexception(x) {uint32_t status = (x); if (status) throw_with_trace(CAENETexception(status)); }

//utilities

template <typename V, size_t... Is>
auto gen_tuple_impl(V vec, std::index_sequence<Is...> ) {
    return std::make_tuple(vec.at(Is)...);
}

template <size_t N, typename V>
auto gen_tuple(V vec) {
    return gen_tuple_impl(vec, std::make_index_sequence<N>{} );
}

template< typename T >
std::string int_to_hex( T i )
{
  std::stringstream stream;
  stream << "0x" 
         << std::setfill ('0') << std::setw(sizeof(T)*2) 
         << std::hex << i;
  return stream.str();
}

#endif
