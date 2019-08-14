#include "VmeUsbBridge.h"
#include "TTCvi.h"
#include "Discri.h"
#include "TDC.h"

using namespace std;

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
    // some configuration...
    Tdc::WindowConfiguration conf;
    conf.width = 50;
    conf.offset = conf.computeOffset(-25);
    conf.extraMargin = 1;
    conf.rejectMargin = 1;
    conf.triggerTimeSubstraction = false;
    myTdc.setTriggerWindow(conf);
    // enable FIFO and TDC header
    myTdc.enableFIFO(true);
    myTdc.enableTDCHeader(true);
    sleep(1);
    // generate trigger and read data one event at a time
    for(int i=0;i<10;i++) {
      myTdc.trigger();
      LOG_DATA_INFO(myTdc.getEvent(true).toString());
    }
    // now with block transfer and FIFO
    // generate 10 triggers
    for(int i=0;i<10;i++) {
      myTdc.trigger();
    }  
    // read all events and print
    auto events = myTdc.getEvents(true);
    LOG_DATA_INFO("got " + to_string(events.size()) + " events");
    for(auto e : events) {
      LOG_DATA_INFO(e.toString());
    }
    // now with block transfer and without FIFO
    myTdc.enableFIFO(false);
    // we also disable the TDC header but allow empty events
    // so that we are sure to get something after the software triggers.
    myTdc.enableTDCHeader(false);
    V1190ControlRegister reg = myTdc.getControlRegister();
    reg.setBit(V1190ControlRegister::cvEMPTYEVT,true);
    myTdc.setControlRegister(reg);
    sleep(1);
    // generate 10 triggers
    for(int i=0;i<10;i++) {
      myTdc.trigger();
    }  
    // read all events and print
    events = myTdc.getEvents(false);
    LOG_DATA_INFO("got " + to_string(events.size()) + " events");
    for(auto e : events) {
      LOG_DATA_INFO(e.toString());
    }
    // DONE
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
