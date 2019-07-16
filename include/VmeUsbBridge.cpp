#include "VmeUsbBridge.h"
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
  delete this->pulserA_;
  delete this->pulserB_;
  delete this->scaler_;
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
  checkCAENVMEexception(CAENVME_ReadCycle(this->BHandle_,address,data,AM,DW));
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
