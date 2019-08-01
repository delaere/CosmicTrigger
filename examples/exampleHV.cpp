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

bool isRamping(bool isSY527, bool isN470, uint32_t status) {
  bool ramping = false;
  if(isN470) {
    ramping = N470StatusWord(status).bit(N470StatusWord::cvRUP);
  } else if(isSY527) {
    ramping = SY527StatusWord(status).bit(SY527StatusWord::cvRUP);
  }
  return ramping;
}

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
    caenet.reset();
    caenet.validStatus();
    LOG_TRACE("Instantiating the HV module");
    auto powersupply = HVModule::HVModuleFactory(0x02, &caenet);
    LOG_INFO(powersupply->identification() + " READY.");
    bool isN470 = (powersupply->identification().find("N470") != std::string::npos);
    bool isSY527 = (powersupply->identification().find("SY527") != std::string::npos);
    
    for (auto& [id,channel] : powersupply->getChannels()) {
      channel->readOperationalParameters();
      int vmax = channel->getBoard()->getVMax(); // in volts
      int softvmax = channel->getSoftMaxV(); // in volts
      vmax = softvmax<vmax ? softvmax : vmax;
      int vdec = channel->getBoard()->getVDecimals(); // number of decimals
      int vset = vmax>200 ? 200*quick_pow10(vdec) : vmax*quick_pow10(vdec); // with some digits
      uint16_t ramp = 10; // in V/s
      ramp = ramp>channel->getBoard()->getRampMax() ? channel->getBoard()->getRampMax() : ramp;
      ramp = ramp<channel->getBoard()->getRampMin() ? channel->getBoard()->getRampMin() : ramp;
      LOG_INFO("set V0 to " + to_string(vset/float(quick_pow10(vdec))) + " V, rup " + to_string(ramp));
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
	doneramping &= ! isRamping(isSY527,isN470,channel->getStatus());
	if (isRamping(isSY527,isN470,channel->getStatus())) {
          LOG_DATA_INFO("channel " + to_string(id.first) + "." + to_string(id.second) + " : " + to_string(channel->getVmon()) + " V. RAMPING." );
	} else {
          LOG_DATA_INFO("channel " + to_string(id.first) + "." + to_string(id.second) + " : " + to_string(channel->getVmon()) + " V." );
	}
      }
      sleep(1);
    }
    LOG_INFO("Reached the nominal voltage, going back off.");
    for (auto& [id,channel] : powersupply->getChannels()) {
      channel->off();
    }
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
