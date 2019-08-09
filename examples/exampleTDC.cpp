#include "VmeUsbBridge.h"
#include "TTCvi.h"
#include "Discri.h"
#include "TDC.h"

int main(int argc, char* argv[]){
  if (argc > 1) {
    Logger::initFromConfig(argv[1]);
    Logger::addDataFileLog("test.log");
  } else {
    Logger::init();
  }
  try {
    VmeUsbBridge myCont;
    Tdc myTdc(&myCont,0xAA0000);
    Tdc::WindowConfiguration conf;
    conf.width = 50;
    conf.offset = conf.computeOffset(-25);
    conf.extraMargin = 1;
    conf.rejectMargin = 1;
    conf.triggerTimeSubstraction = false;
    myTdc.setTriggerWindow(conf);
    myTdc.enableFIFO(true);
    myTdc.enableTDCHeader(true);
    // configure some fake addData
    uint16_t opcode = opcode = 0xC500;
    myTdc.writeOpcode(opcode);
    opcode = 123; myTdc.writeOpcode(opcode);
    opcode = 0; myTdc.writeOpcode(opcode);
    // generate trigger and read data
    for(int i=0;i<10;i++) {
      myTdc.trigger();
      LOG_DATA_INFO(myTdc.getEvent(true).toString());
    }
    // now with block transfer
    for(int i=0;i<10;i++) {
      myTdc.trigger();
    }  
    auto events = myTdc.getEvents(true);
    LOG_DATA_INFO("got " + to_string(events.size()) + " events");
    for(auto e : events) {
      LOG_DATA_INFO(e.toString());
    }
    // same without FIFO and without TDC headers
    myTdc.enableTDCHeader(false);
    myTdc.enableFIFO(false);
    for(int i=0;i<10;i++) {
      myTdc.trigger();
    }  
    events = myTdc.getEvents(false);
    LOG_DATA_INFO("got " + to_string(events.size()) + " events");
    for(auto e : events) {
      LOG_DATA_INFO(e.toString());
    }
    
  } catch (const CAENVMEexception& e) {
    std::cerr << e.what() << '\n';
    const boost::stacktrace::stacktrace* st = boost::get_error_info<traced>(e);
    if (st) {
      std::cerr << *st << '\n'; /*<-*/ return 0; /*->*/
    } /*<-*/ return 3; /*->*/
  } catch (const CAENETexception& e) {
    std::cerr << e.what() << '\n';
    const boost::stacktrace::stacktrace* st = boost::get_error_info<traced>(e);
    if (st) {
      std::cerr << *st << '\n'; /*<-*/ return 0; /*->*/
    } /*<-*/ return 3; /*->*/
  }
return 5;
}
