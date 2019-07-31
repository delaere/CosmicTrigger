#include "VmeUsbBridge.h"
#include "CaenetBridge.h"
#include "HVmodule.h"
#include "N470HVmodule.h"
#include "SY527PowerSystem.h"
#include "logger.h"
#include <iostream>
#include <exception>
#include <string>

using namespace std;

int main(int argc, char* argv[]){

  if (argc > 1) {
	  Logger::initFromConfig(argv[1]);
	  Logger::addDataFileLog("test.log");
  } else {
	   Logger::init();
  }

  try {
    LOG_DEBUG("Start of configuration");
    LOG_TRACE("Instantiating the controller");
    VmeUsbBridge vme;
    LOG_TRACE("Instantiating the CAENET bridge");
    CaenetBridge caenet(&vme, 0xF0000);
    LOG_TRACE("Instantiating the HV module");
    auto powersupply = HVModule::HVModuleFactory(0x02, &caenet);
    LOG_INFO(powersupply->identification() + " READY.");
    bool isN470 = (powersupply->identification().find("N470") != std::string::npos);
    bool isSY527 = (powersupply->identification().find("SY527") != std::string::npos);
    
    
    for (auto& [id,channel] : powersupply->getChannels()) {
      int vset = channel->getBoard()->getVMax()>200 ? 200 : channel->getBoard()->getVMax();
      uint16_t ramp = 10;
      ramp = ramp>channel->getBoard()->getRampMax() ? channel->getBoard()->getRampMax() : ramp;
      ramp = ramp<channel->getBoard()->getRampMin() ? channel->getBoard()->getRampMin() : ramp;
      channel->setV0(vset);
      channel->setRampup(ramp);
      channel->on();
    }
    
    LOG_INFO("Turned all channels on at 200V (if possible, otherwise maxV).");
    
    LOG_INFO("Monitoring ramp up...");
    bool doneramping = false;
    while (!doneramping) {
      doneramping = true;
      for (auto& [id,channel] : powersupply->getChannels()) {
        channel->readOperationalParameters();
        LOG_DATA_INFO("channel " + to_string(id.first) + "." + to_string(id.second) + " : " + to_string(channel->getVmon()) + " V");
        if(isN470) {
          doneramping &= ! N470StatusWord(channel->getStatus()).bit(N470StatusWord::cvRUP);
        } else if(isSY527) {
          doneramping &= ! SY527StatusWord(channel->getStatus()).bit(SY527StatusWord::cvRUP);
        }
        sleep(1);
      }
    }
    LOG_INFO("Reached the nominal voltage");
    delete powersupply;
    LOG_TRACE("Done");
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
