#include <boost/python.hpp>
#include "VmeController.h"
#include "VmeUsbBridge.h"
#include "VmeBoard.h"
#include "CaenetBridge.h"
#include "HVmodule.h"
#include "N470HVmodule.h"
#include "SY527PowerSystem.h"

using namespace boost::python;

struct VmeControllerWrap : VmeController, wrapper<VmeController>
{
  VmeControllerWrap():VmeController(),wrapper<VmeController>() {}
  
  void writeData(long unsigned int address,void* data) const override {
    this->get_override("writeData")(address,data);
  }
  void readData(long unsigned int address,void* data) const override {
    this->get_override("readData")(address,data);
  }
  void readWriteData(const long unsigned int address,void* data) const override {
    this->get_override("readWriteData")(address,data);
  }
  void blockReadData(const long unsigned int address,unsigned char *buffer, int size, int *count, bool multiplex=false) const override {
    this->get_override("blockReadData")(address,buffer,size,count,multiplex);
  }
  void blockWriteData(const long unsigned int address,unsigned char *buffer, int size, int *count, bool multiplex=false) const override {
    this->get_override("blockWriteData")(address,buffer,size,count,multiplex);
  }
  void ADOCycle(const long unsigned int address) const override {
    this->get_override("ADOCycle")(address);
  }
  void IRQEnable(uint32_t mask) const override {
    this->get_override("IRQEnable")(mask);
  }
  void IRQDisable(uint32_t mask) const override {
    this->get_override("IRQDisable")(mask);
  }
  void IRQWait(uint32_t mask, uint32_t timeout_ms) const override {
    this->get_override("IRQWait")(mask,timeout_ms);
  }
  unsigned char IRQCheck() const override {
    return this->get_override("IRQCheck")();
  }
  uint16_t IACK(CVIRQLevels Level) const override {
    return this->get_override("IACK")(Level);
  }
};

struct HVChannelWrap : HVChannel, wrapper<HVChannel>
{
  HVChannelWrap(uint32_t address, HVBoard& board, uint32_t id, CaenetBridge* bridge):HVChannel(address, board, id, bridge), wrapper<HVChannel>() {} 
  
  void on() override {
    this->get_override("on")();
  }
  
  void off() override {
    this->get_override("off")();
  }
  
  void setV0(uint32_t v0) override {
    this->get_override("setV0")(v0);
  }
  
  void setV1(uint32_t v1) override {
    this->get_override("setV1")(v1);
  }
  
  void setI0(uint32_t i0) override {
    this->get_override("setI0")(i0);
  }
  
  void setI1(uint32_t i1) override {
    this->get_override("setI1")(i1);
  }
  
  void setRampup(uint32_t rampup) override {
    this->get_override("setRampup")(rampup);
  }
  
  void setRampdown(uint32_t rampdown) override {
    this->get_override("setRampdown")(rampdown);
  }
  
  void setTrip(uint32_t trip) override {
    this->get_override("setTrip")(trip);
  }
  
  void setSoftMaxV(uint32_t maxv) override {
    this->get_override("setSoftMaxV")(maxv);
  }
  
  void readOperationalParameters() override {
    this->get_override("readOperationalParameters")();
  }

};

struct HVModuleWrap : HVModule, wrapper<HVModule>
{
  HVModuleWrap(uint32_t address, CaenetBridge* bridge):HVModule(address, bridge), wrapper<HVModule>() {}
  
  void discoverBoards() override {
    this->get_override("discoverBoards")();
  }
  
  void assertIdentity() const override {
    this->get_override("assertIdentity")();
  }
};

BOOST_PYTHON_MODULE(VeheMencE)
{
  //expose the enums (TODO if confirmed that we need it. We can live with integers)
  
  //expose VmeController
    class_<VmeControllerWrap, boost::noncopyable>("VmeController")
        .def("setMode", &VmeController::setMode)
        .def("mode", &VmeController::mode,return_value_policy<manage_new_object>())
        .add_property("AM",&VmeController::getAM,&VmeController::setAM)
        .add_property("DW",&VmeController::getDW,&VmeController::setDW)
        .def("writeData",pure_virtual(&VmeController::writeData)) 
        .def("readData",pure_virtual(&VmeController::readData))
        .def("readWriteData",pure_virtual(&VmeController::readWriteData))
        .def("blockReadData",pure_virtual(&VmeController::blockReadData))
        .def("blockWriteData",pure_virtual(&VmeController::blockWriteData))
        .def("ADOCycle",pure_virtual(&VmeController::ADOCycle))
        .def("IRQEnable",pure_virtual(&VmeController::IRQEnable))
        .def("IRQDisable",pure_virtual(&VmeController::IRQDisable))
        .def("IRQWait",pure_virtual(&VmeController::IRQWait))
        .def("IRQCheck",pure_virtual(&VmeController::IRQCheck))
        .def("IACK",pure_virtual(&VmeController::IACK))
    ;
  //expose VmeUsbBridge
    class_<VmeUsbBridge, bases<VmeController> >("VmeUsbBridge")
         .def("configureOutputLine",&VmeUsbBridge::configureOutputLine)
         .def("configureInputLine",&VmeUsbBridge::configureInputLine)
         .def("outputLineConfiguration",&VmeUsbBridge::outputLineConfiguration)
         .def("inputLineConfiguration",&VmeUsbBridge::inputLineConfiguration)
         .def("readRegister",&VmeUsbBridge::readRegister)
         .def("setOutputRegister",&VmeUsbBridge::setOutputRegister)
         .def("clearOutputRegister",&VmeUsbBridge::clearOutputRegister)
         .def("pulseOutputRegister",&VmeUsbBridge::pulseOutputRegister)
         .def("readDisplay",&VmeUsbBridge::readDisplay)
         .add_property("arbiterType",&VmeUsbBridge::getArbiterType,&VmeUsbBridge::setArbiterType)
         .add_property("requesterType",&VmeUsbBridge::getRequesterType,&VmeUsbBridge::setRequesterType)
         .add_property("releaseType",&VmeUsbBridge::getReleaseType,&VmeUsbBridge::setReleaseType)
         .add_property("busReqLevel",&VmeUsbBridge::getBusReqLevel,&VmeUsbBridge::setBusReqLevel)
         .add_property("timeout",&VmeUsbBridge::getTimeout,&VmeUsbBridge::setTimeout)
         .add_property("FIFOmode",&VmeUsbBridge::getFIFOMode,&VmeUsbBridge::setFIFOMode)
         .def("getPulser",&VmeUsbBridge::getPulser,return_value_policy<copy_non_const_reference>())
         .def("getScaler",&VmeUsbBridge::getScaler,return_value_policy<copy_non_const_reference>())
    ;
    class_<V1718Pulser>("V1718Pulser",init<uint32_t,CVPulserSelect>())
         .def("configure",&V1718Pulser::configure)
         .def("update",&V1718Pulser::configure)
         .def("start",&V1718Pulser::start)
         .def("stop",&V1718Pulser::stop)
         .add_property("period",&V1718Pulser::getPeriod,&V1718Pulser::setPeriod)
         .add_property("width",&V1718Pulser::getWidth,&V1718Pulser::setWidth)
         .add_property("units",&V1718Pulser::getUnits,&V1718Pulser::setUnits)
         .add_property("pulseNo",&V1718Pulser::getPulseNo,&V1718Pulser::setPulseNo)
         .add_property("startSource",&V1718Pulser::getStartSource,&V1718Pulser::setStartSource)
         .add_property("stopSource",&V1718Pulser::getStopSource,&V1718Pulser::setStopSource)
    ;
    class_<V1718Scaler>("V1718Scaler",init<uint32_t>())
         .def("configure",&V1718Scaler::configure)
         .def("update",&V1718Scaler::update)
         .def("resetCount",&V1718Scaler::resetCount)
         .def("enableGate",&V1718Scaler::enableGate)
         .def("disableGate",&V1718Scaler::disableGate)
         .def("count",&V1718Scaler::count)
         .add_property("limit",&V1718Scaler::getLimit,&V1718Scaler::setLimit)
         .add_property("autoReset",&V1718Scaler::getAutoReset,&V1718Scaler::setAutoReset)
         .add_property("hitSource",&V1718Scaler::getHitSource,&V1718Scaler::setHitSource)
         .add_property("gateSource",&V1718Scaler::getGateSource,&V1718Scaler::setGateSource)
         .add_property("resetSource",&V1718Scaler::getResetSource,&V1718Scaler::setResetSource)
    ;
    // expose VmeBoard
    class_<VmeBoard>("VmeBoard",init<VmeController*, uint32_t, CVAddressModifier, CVDataWidth, bool>())
         .add_property("enforce",&VmeBoard::isAMDWenforced,&VmeBoard::enforceAMDW)
    ;
    // expose CaenetBridge
    class_<CaenetBridge, bases<VmeBoard> >("CaenetBridge",init<VmeController*, uint32_t, uint8_t>())
         .def("reset",&CaenetBridge::reset)
         .def("validStatus",&CaenetBridge::validStatus)
         .def("write",&CaenetBridge::write)
         .def("readResponse",&CaenetBridge::readResponse)
    ;
    // expose HVmodule
    class_<HVChannelWrap, boost::noncopyable>("HVChannel", init<uint32_t, HVBoard&, uint32_t, CaenetBridge*>())
         .add_property("id",&HVChannel::id)
         .add_property("V0",&HVChannel::getV0,&HVChannel::setV0)
         .add_property("V1",&HVChannel::getV1,&HVChannel::setV1)
         .add_property("I0",&HVChannel::getI0,&HVChannel::setI0)
         .add_property("I1",&HVChannel::getI1,&HVChannel::setI1)
         .add_property("rampup",&HVChannel::getRampup,&HVChannel::setRampup)
         .add_property("rampdown",&HVChannel::getRampdown,&HVChannel::setRampdown)
         .add_property("trip",&HVChannel::getTrip,&HVChannel::setTrip)
         .add_property("Vmon",&HVChannel::getVmon)
         .add_property("Imon",&HVChannel::getImon)
         .add_property("maxV",&HVChannel::getmaxV)
         .add_property("status",&HVChannel::getStatus)
         .def("on",&HVChannel::on)
         .def("off",&HVChannel::off)
         .def("readOperationalParameters",&HVChannel::readOperationalParameters)
    ;
    class_<HVBoard>("HVBoard",init<uint32_t, std::string, uint8_t,uint16_t, uint16_t, uint8_t, uint32_t, uint16_t, uint16_t, uint16_t, uint16_t, uint16_t, uint16_t, uint16_t>())
         .def("getChannels",&HVBoard::getChannels)
         .def("getSlot",&HVBoard::getSlot)
         .def("getName",&HVBoard::getName)
         .def("getCurrentUnit",&HVBoard::getCurrentUnit)
         .def("getSerialNumber",&HVBoard::getSerialNumber)
         .def("getSoftwareVersion",&HVBoard::getSoftwareVersion)
         .def("getNChannels",&HVBoard::getNChannels)
         .def("getVMax",&HVBoard::getVMax)
         .def("getIMax",&HVBoard::getIMax)
         .def("getRampMin",&HVBoard::getRampMin)
         .def("getRampMax",&HVBoard::getRampMax)
         .def("getVResolution",&HVBoard::getVResolution)
         .def("getIResolution",&HVBoard::getIResolution)
         .def("getVDecimals",&HVBoard::getVDecimals)
         .def("getIDecimals",&HVBoard::getIDecimals)
    ;
    class_<HVModuleWrap, boost::noncopyable>("HVModule", init<uint32_t, CaenetBridge*>())
         .add_property("identification",&HVModule::identification)
         .def("HVModuleFactory",&HVModule::HVModuleFactory,return_value_policy<manage_new_object>())
         .def("channel",&HVModule::channel,return_value_policy<copy_non_const_reference>())
         .def("board",&HVModule::board,return_value_policy<copy_non_const_reference>())
         .def("getChannels",&HVModule::getChannels,return_value_policy<copy_non_const_reference>())
         .def("getBoards",&HVModule::getBoards,return_value_policy<copy_non_const_reference>())
    ;
    // expose N470HVmodule
    class_<N470StatusWord>("N470StatusWord",init<uint16_t>())
         .def("status",&N470StatusWord::status)
         .def("bit",&N470StatusWord::bit)
    ;
    class_<N470HVChannel>("N470HVChannel",init<uint32_t,HVBoard&,uint32_t,CaenetBridge*>())
         .def("getStatus",&N470HVChannel::getStatus)
    ;
    class_<N470HVModule>("N470HVModule",init<uint32_t,CaenetBridge*>())
         .add_property("status",&N470HVModule::getStatus)
         .def("updateStatus",&N470HVModule::updateStatus)
         .def("kill",&N470HVModule::kill)
         .def("clearAlarm",&N470HVModule::clearAlarm)
         .def("enableKeyboard",&N470HVModule::enableKeyboard)
         .def("setTTL",&N470HVModule::setTTL)
         .def("setNIM",&N470HVModule::setNIM)
    ;
    // expose SY527PowerSystem
    class_<SY527StatusWord>("SY527StatusWord",init<uint16_t>())
         .def("status",&SY527StatusWord::status)
         .def("bit",&SY527StatusWord::bit)
    ;
    class_<SY527HVChannel, bases<HVChannel> >("SY527HVChannel",init<uint32_t, HVBoard&, uint32_t, CaenetBridge*>())
         .def("setPasswordFlag",&SY527HVChannel::setPasswordFlag)
         .def("setOnOffFlag",&SY527HVChannel::setOnOffFlag)
         .def("setPoweronFlag",&SY527HVChannel::setPoweronFlag)
         .def("getName",&SY527HVChannel::getName)
         .def("getStatus",&SY527HVChannel::getStatus)
    ;
    class_<SY527PowerSystem, bases<HVModule> >("SY527PowerSystem",init<uint32_t,CaenetBridge*>())
         .def("updateStatus",&SY527PowerSystem::updateStatus)
    ;
}
