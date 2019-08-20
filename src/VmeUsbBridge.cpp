/*
 *  VeheMencE: a simple library for VME access
 *  Copyright (C) 2019 Universite catholique de Louvain (UCLouvain), Belgium
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "VmeUsbBridge.h"
#include "PythonModule.h"
#include <iostream>

VmeUsbBridge::VmeUsbBridge():VmeController() {
  this->board_ = cvV1718;
  char FWRel[128];
  
  LOG_DEBUG("calling CAENVME INIT");
  checkCAENVMEexception(CAENVME_Init((CVBoardTypes)(int)board_, 0, 0, &this->BHandle_));
  this->pulserA_ = new V1718Pulser(this->BHandle_,cvPulserA);
  this->pulserB_ = new V1718Pulser(this->BHandle_,cvPulserB);
  this->scaler_  = new V1718Scaler(this->BHandle_);

  LOG_DEBUG("Reading CAENVME BOARD RELEASE");
  checkCAENVMEexception(CAENVME_BoardFWRelease(this->BHandle_,FWRel));
  this->firmwareVersion_ = std::string(FWRel);
  
  LOG_INFO("VME USB Init... ok!");
  LOG_INFO("Firmware version: " + this->firmwareVersion_ )
}

VmeUsbBridge::~VmeUsbBridge(){
  LOG_TRACE("about to delete pulser A");
  delete this->pulserA_;
  LOG_TRACE("about to delete pulser B");
  delete this->pulserB_;
  LOG_TRACE("about to delete scaler");
  delete this->scaler_;
  LOG_TRACE("calling CAENVME_End");
  CAENVME_End(this->BHandle_);
  LOG_INFO("Disconnected from USB controler.");
}

VmeUsbBridge::VmeUsbBridge(const VmeUsbBridge& other):VmeController(other),firmwareVersion_(other.firmwareVersion_),BHandle_(other.BHandle_) {
  board_ = cvV1718;
  pulserA_ = new V1718Pulser(*other.pulserA_);
  pulserB_ = new V1718Pulser(*other.pulserB_);
  scaler_  = new V1718Scaler(*other.scaler_);
}

VmeUsbBridge& VmeUsbBridge::operator=(const VmeUsbBridge& other){
  board_ = cvV1718;
  firmwareVersion_ = other.firmwareVersion_;
  BHandle_ = other.BHandle_;
  pulserA_->operator=(*other.pulserA_);
  pulserB_->operator=(*other.pulserB_);
  scaler_->operator=(*other.scaler_);
  VmeController::operator=(other);
  return *this;
}

void VmeUsbBridge::writeData(long unsigned int address,void* data) const{
  auto [AM, DW] = useMode();
  checkCAENVMEexception(CAENVME_WriteCycle(this->BHandle_,address,data,AM,DW));
}

void VmeUsbBridge::readData(long unsigned int address,void* data) const {
  auto [AM, DW] = useMode();
  checkCAENVMEexception(CAENVME_ReadCycle(this->BHandle_,address,data,AM,DW));
}

void VmeUsbBridge::readWriteData(const long unsigned int address,void* data) const {
  auto [AM, DW] = useMode();
  checkCAENVMEexception(CAENVME_RMWCycle(this->BHandle_,address,data,AM,DW));
}

void VmeUsbBridge::blockReadData(const long unsigned int address,unsigned char *buffer, int size, int *count, bool multiplex) const {
  auto [AM, DW] = useMode();
  if (multiplex) {
    checkCAENVMEexception(CAENVME_MBLTReadCycle(this->BHandle_, address, buffer, size, AM, count));
  } else {
    checkCAENVMEexception(CAENVME_BLTReadCycle(this->BHandle_, address, buffer, size, AM, DW, count));
  }
}

void VmeUsbBridge::blockWriteData(const long unsigned int address,unsigned char *buffer, int size, int *count, bool multiplex) const {
  auto [AM, DW] = useMode();
  if (multiplex) {
    checkCAENVMEexception(CAENVME_MBLTWriteCycle(this->BHandle_, address, buffer, size, AM, count));
  } else {
    checkCAENVMEexception(CAENVME_BLTWriteCycle(this->BHandle_, address, buffer, size, AM, DW, count));
  }
}

void VmeUsbBridge::ADOCycle(const long unsigned int address) const {
  auto [AM, DW] = useMode();
  checkCAENVMEexception(CAENVME_ADOCycle(this->BHandle_, address, AM));
}

V1718Pulser& VmeUsbBridge::getPulser(CVPulserSelect pulser){
  if(pulser==cvPulserA) 
    return *this->pulserA_;
  else 
    return *this->pulserB_;
}

V1718Scaler& VmeUsbBridge::getScaler(){
  return *this->scaler_;
}

void VmeUsbBridge::configureOutputLine(CVOutputSelect line, CVIOPolarity polarity, CVLEDPolarity LEDpolarity, CVIOSources source) const {
  checkCAENVMEexception(CAENVME_SetOutputConf(this->BHandle_, line, polarity, LEDpolarity, source));
}

void VmeUsbBridge::configureInputLine(CVInputSelect line, CVIOPolarity polarity, CVLEDPolarity LEDpolarity) const {
  checkCAENVMEexception(CAENVME_SetInputConf(this->BHandle_, line, polarity, LEDpolarity));
}

std::tuple<CVIOPolarity, CVLEDPolarity, CVIOSources> VmeUsbBridge::outputLineConfiguration(CVOutputSelect line) const {
  CVIOPolarity OutPol;
  CVLEDPolarity LEDPol; 
  CVIOSources Source;
  checkCAENVMEexception(CAENVME_GetOutputConf(this->BHandle_, line,&OutPol,&LEDPol, &Source));
  return std::make_tuple(OutPol,LEDPol,Source);
}

std::tuple<CVIOPolarity, CVLEDPolarity> VmeUsbBridge::inputLineConfiguration(CVInputSelect line) const {
  CVIOPolarity InPol;
  CVLEDPolarity LEDPol;  
  checkCAENVMEexception(CAENVME_GetInputConf(this->BHandle_, line, &InPol, &LEDPol));
  return std::tuple(InPol,LEDPol);
}

uint32_t VmeUsbBridge::readRegister(CVRegisters reg) const {
  uint32_t data;
  checkCAENVMEexception(CAENVME_ReadRegister(this->BHandle_, reg, &data));
  return data;
}

void VmeUsbBridge::setOutputRegister(unsigned short mask) const {
  checkCAENVMEexception(CAENVME_SetOutputRegister(this->BHandle_, mask));
}

void VmeUsbBridge::clearOutputRegister(unsigned short mask) const {
  checkCAENVMEexception(CAENVME_ClearOutputRegister(this->BHandle_, mask));
}

void VmeUsbBridge::pulseOutputRegister(unsigned short mask) const {
  checkCAENVMEexception(CAENVME_PulseOutputRegister(this->BHandle_, mask));
}

CVDisplay VmeUsbBridge::readDisplay() const {
  CVDisplay value;
  checkCAENVMEexception(CAENVME_ReadDisplay(this->BHandle_, &value));
  return value;
}

void VmeUsbBridge::setArbiterType(CVArbiterTypes type) const {
  checkCAENVMEexception(CAENVME_SetArbiterType(this->BHandle_, type));
}

void VmeUsbBridge::setRequesterType(CVRequesterTypes type) const {
  checkCAENVMEexception(CAENVME_SetRequesterType(this->BHandle_, type));
}

void VmeUsbBridge::setReleaseType(CVReleaseTypes type) const {
  checkCAENVMEexception(CAENVME_SetReleaseType(this->BHandle_, type));
}

void VmeUsbBridge::setBusReqLevel(CVBusReqLevels level) const {
  checkCAENVMEexception(CAENVME_SetBusReqLevel(this->BHandle_,level));
}

void VmeUsbBridge::setTimeout(CVVMETimeouts timeout) const {
  checkCAENVMEexception(CAENVME_SetTimeout(this->BHandle_,timeout));
}

void VmeUsbBridge::setFIFOMode(bool mode) const {
  checkCAENVMEexception(CAENVME_SetFIFOMode(this->BHandle_,(short)mode));
}

CVArbiterTypes VmeUsbBridge::getArbiterType() const {
  CVArbiterTypes value;
  checkCAENVMEexception(CAENVME_GetArbiterType(this->BHandle_, &value));
  return value;
}

CVRequesterTypes VmeUsbBridge::getRequesterType() const {
  CVRequesterTypes value;
  checkCAENVMEexception(CAENVME_GetRequesterType(this->BHandle_, &value));
  return value;
}

CVReleaseTypes VmeUsbBridge::getReleaseType() const {
  CVReleaseTypes value;
  checkCAENVMEexception(CAENVME_GetReleaseType(this->BHandle_, &value));
  return value;
}

CVBusReqLevels VmeUsbBridge::getBusReqLevel() const {
  CVBusReqLevels value;
  checkCAENVMEexception(CAENVME_GetBusReqLevel(this->BHandle_, &value));
  return value;
}

CVVMETimeouts VmeUsbBridge::getTimeout() const {
  CVVMETimeouts value;
  checkCAENVMEexception(CAENVME_GetTimeout(this->BHandle_, &value));
  return value;
}

bool VmeUsbBridge::getFIFOMode() const {
  short value;
  checkCAENVMEexception(CAENVME_GetFIFOMode(this->BHandle_, &value));
  return value;
}

void VmeUsbBridge::systemReset() const {
  checkCAENVMEexception(CAENVME_SystemReset(this->BHandle_));
  /* note: we don't update the pulsers and scaler so a simple configure() on the restores the previous settings
  pulserA_.update();
  pulserB_.update();
  scaler.update();
  */
}

void VmeUsbBridge::IRQEnable(uint32_t mask) const {
  checkCAENVMEexception(CAENVME_IRQEnable(this->BHandle_,mask));
}

void VmeUsbBridge::IRQDisable(uint32_t mask) const {
  checkCAENVMEexception(CAENVME_IRQDisable(this->BHandle_,mask));
}

void VmeUsbBridge::IRQWait(uint32_t mask, uint32_t timeout_ms) const {
  checkCAENVMEexception(CAENVME_IRQWait(this->BHandle_,mask,timeout_ms));
}

unsigned char VmeUsbBridge::IRQCheck() const {
  unsigned char output;
  checkCAENVMEexception(CAENVME_IRQCheck(this->BHandle_, &output));
  return output;
}

uint16_t VmeUsbBridge::IACK(CVIRQLevels level) const {
  auto [AM, DW] = useMode();
  uint16_t vector;
  checkCAENVMEexception(CAENVME_IACKCycle(this->BHandle_, level, &vector, DW));
  return vector;
}

V1718Pulser::V1718Pulser(uint32_t handle, CVPulserSelect id):BHandle_(handle),pulserId_(id) {
  this->update();
}

void V1718Pulser::configure() const {
  checkCAENVMEexception(CAENVME_SetPulserConf(this->BHandle_, this->pulserId_,
                                              this->period_,this->width_,
                                              this->units_,this->pulseNo_,
                                              this->start_,this->reset_));
  this->configured_ = true;
}

void V1718Pulser::start() const {
  if (!this->configured_ ) configure();
  if (this->start_ != cvManualSW) throw CAENVMEexception(cvInvalidParam);
  checkCAENVMEexception(CAENVME_StartPulser(this->BHandle_, this->pulserId_));
}

void V1718Pulser::stop() const {
  if (!this->configured_ ) configure();
  if (this->reset_ != cvManualSW) throw CAENVMEexception(cvInvalidParam);
  checkCAENVMEexception(CAENVME_StopPulser(this->BHandle_, this->pulserId_));
}

void V1718Pulser::update(){
  checkCAENVMEexception(CAENVME_GetPulserConf(this->BHandle_, this->pulserId_, 
                                              &(this->period_),&(this->width_),
                                              &(this->units_), &(this->pulseNo_),
                                              &(this->start_), &(this->reset_)));
  this->configured_ = true;
}

V1718Scaler::V1718Scaler(uint32_t handle):BHandle_(handle) {
  this->update();
}

void V1718Scaler::configure(){
  checkCAENVMEexception(CAENVME_SetScalerConf(this->BHandle_, 
                                              this->limit_,this->autoReset_,
                                              this->hit_,this->gate_,
                                              this->reset_));
  this->configured_ = true;
}

void V1718Scaler::update(){
  checkCAENVMEexception(CAENVME_GetScalerConf(this->BHandle_,
                                              &(this->limit_),&(this->autoReset_),
                                              &(this->hit_),&(this->gate_),
                                              &(this->reset_)));
  this->configured_ = true;
}

void V1718Scaler::resetCount(){
  if (!this->configured_ ) configure();
  if (this->reset_ != cvManualSW) throw CAENVMEexception(cvInvalidParam);
  checkCAENVMEexception(CAENVME_ResetScalerCount(this->BHandle_));
}

void V1718Scaler::enableGate(){
  if (!this->configured_ ) configure();
  if (this->gate_ != cvManualSW) throw CAENVMEexception(cvInvalidParam);
  checkCAENVMEexception(CAENVME_EnableScalerGate(this->BHandle_));
}

void V1718Scaler::disableGate(){
  if (!this->configured_ ) configure();
  if (this->gate_ != cvManualSW) throw CAENVMEexception(cvInvalidParam);
  checkCAENVMEexception(CAENVME_DisableScalerGate(this->BHandle_));
}

unsigned short V1718Scaler::count() const {
  unsigned int data = 0;
  checkCAENVMEexception(CAENVME_ReadRegister(this->BHandle_, cvScaler1, &data));
  return (data & 1023);
}

using namespace boost::python;

template<> void exposeToPython<VmeUsbBridge>() {
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
    .def("systemReset",&VmeUsbBridge::systemReset)
    .def("getPulser",&VmeUsbBridge::getPulser,return_value_policy<copy_non_const_reference>())
    .def("getScaler",&VmeUsbBridge::getScaler,return_value_policy<copy_non_const_reference>())
  ;
}

template<> void exposeToPython<V1718Pulser>() {
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
}

template<> void exposeToPython<V1718Scaler>() {
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
}
