#include "VmeUsbBridge.h"
#include <iostream>

UsbController::UsbController(int verbose):vmeController(verbose) {
  this->board_ = cvV1718;
  char FWRel[128];
  
  checkCAENVMEexception(CAENVME_Init((CVBoardTypes)(int)board_, 0, 0, &this->BHandle_));
  this->pulserA_ = new V1718Pulser(this->BHandle_,cvPulserA);
  this->pulserB_ = new V1718Pulser(this->BHandle_,cvPulserB);
  this->scaler_  = new V1718Scaler(this->BHandle_);

  checkCAENVMEexception(CAENVME_BoardFWRelease(this->BHandle_,FWRel));
  this->firmwareVersion_ = std::string(FWRel);
  
  if(verbosity()>=3) {
    std::cout << "VME USB Init... ok!"<< std::endl;
    std::cout << "Firmware version: " << this->firmwareVersion_ << std::endl;
  }
}

UsbController::~UsbController(){
  delete this->pulserA_;
  delete this->pulserB_;
  delete this->scaler_;
  CAENVME_End(this->BHandle_);
  if(verbosity()>=3) {
    std::cout << "Disconnected from USB controler." << std::endl; 
  }
}

UsbController::UsbController(const UsbController& other):vmeController(other),firmwareVersion_(other.firmwareVersion_),BHandle_(other.BHandle_) {
  board_ = cvV1718;
  pulserA_ = new V1718Pulser(*other.pulserA_);
  pulserB_ = new V1718Pulser(*other.pulserB_);
  scaler_  = new V1718Scaler(*other.scaler_);
}

UsbController& UsbController::operator=(const UsbController& other){
  board_ = cvV1718;
  firmwareVersion_ = other.firmwareVersion_;
  BHandle_ = other.BHandle_;
  pulserA_->operator=(*other.pulserA_);
  pulserB_->operator=(*other.pulserB_);
  scaler_->operator=(*other.scaler_);
  vmeController::operator=(other);
  return *this;
}

void UsbController::writeData(long unsigned int address,void* data) const{
  auto [AM, DW] = useMode();
  checkCAENVMEexception(CAENVME_WriteCycle(this->BHandle_,address,data,AM,DW));
}

void UsbController::readData(long unsigned int address,void* data) const {
  auto [AM, DW] = useMode();
  checkCAENVMEexception(CAENVME_ReadCycle(this->BHandle_,address,data,AM,DW));
}

void UsbController::readWriteData(const long unsigned int address,void* data) const {
  auto [AM, DW] = useMode();
  checkCAENVMEexception(CAENVME_ReadCycle(this->BHandle_,address,data,AM,DW));
}

void UsbController::blockReadData(const long unsigned int address,unsigned char *buffer, int size, int *count, bool multiplex) const {
  auto [AM, DW] = useMode();
  if (multiplex) {
    checkCAENVMEexception(CAENVME_MBLTReadCycle(this->BHandle_, address, buffer, size, AM, count));
  } else {
    checkCAENVMEexception(CAENVME_BLTReadCycle(this->BHandle_, address, buffer, size, AM, DW, count));
  }
}

void UsbController::blockWriteData(const long unsigned int address,unsigned char *buffer, int size, int *count, bool multiplex) const {
  auto [AM, DW] = useMode();
  if (multiplex) {
    checkCAENVMEexception(CAENVME_MBLTWriteCycle(this->BHandle_, address, buffer, size, AM, count));
  } else {
    checkCAENVMEexception(CAENVME_BLTWriteCycle(this->BHandle_, address, buffer, size, AM, DW, count));
  }
}

void UsbController::ADOCycle(const long unsigned int address) const {
  auto [AM, DW] = useMode();
  checkCAENVMEexception(CAENVME_ADOCycle(this->BHandle_, address, AM));
}

V1718Pulser& UsbController::getPulser(CVPulserSelect pulser){
  if(pulser==cvPulserA) 
    return *this->pulserA_;
  else 
    return *this->pulserB_;
}

V1718Scaler& UsbController::getScaler(){
  return *this->scaler_;
}

void UsbController::configureOutputLine(CVOutputSelect line, CVIOPolarity polarity, CVLEDPolarity LEDpolarity, CVIOSources source) const {
  checkCAENVMEexception(CAENVME_SetOutputConf(this->BHandle_, line, polarity, LEDpolarity, source));
}

void UsbController::configureInputLine(CVInputSelect line, CVIOPolarity polarity, CVLEDPolarity LEDpolarity) const {
  checkCAENVMEexception(CAENVME_SetInputConf(this->BHandle_, line, polarity, LEDpolarity));
}

std::tuple<CVIOPolarity, CVLEDPolarity, CVIOSources> UsbController::outputLineConfiguration(CVOutputSelect line) const {
  CVIOPolarity OutPol;
  CVLEDPolarity LEDPol; 
  CVIOSources Source;
  checkCAENVMEexception(CAENVME_GetOutputConf(this->BHandle_, line,&OutPol,&LEDPol, &Source));
  return std::make_tuple(OutPol,LEDPol,Source);
}

std::tuple<CVIOPolarity, CVLEDPolarity> UsbController::inputLineConfiguration(CVInputSelect line) const {
  CVIOPolarity InPol;
  CVLEDPolarity LEDPol;  
  checkCAENVMEexception(CAENVME_GetInputConf(this->BHandle_, line, &InPol, &LEDPol));
  return std::tuple(InPol,LEDPol);
}

uint32_t UsbController::readRegister(CVRegisters reg) const {
  uint32_t data;
  checkCAENVMEexception(CAENVME_ReadRegister(this->BHandle_, reg, &data));
  return data;
}

void UsbController::setOutputRegister(unsigned short mask) const {
  checkCAENVMEexception(CAENVME_SetOutputRegister(this->BHandle_, mask));
}

void UsbController::clearOutputRegister(unsigned short mask) const {
  checkCAENVMEexception(CAENVME_ClearOutputRegister(this->BHandle_, mask));
}

void UsbController::pulseOutputRegister(unsigned short mask) const {
  checkCAENVMEexception(CAENVME_PulseOutputRegister(this->BHandle_, mask));
}

CVDisplay UsbController::readDisplay() const {
  CVDisplay value;
  checkCAENVMEexception(CAENVME_ReadDisplay(this->BHandle_, &value));
  return value;
}

void UsbController::setArbiterType(CVArbiterTypes type) const {
  checkCAENVMEexception(CAENVME_SetArbiterType(this->BHandle_, type));
}

void UsbController::setRequesterType(CVRequesterTypes type) const {
  checkCAENVMEexception(CAENVME_SetRequesterType(this->BHandle_, type));
}

void UsbController::setReleaseType(CVReleaseTypes type) const {
  checkCAENVMEexception(CAENVME_SetReleaseType(this->BHandle_, type));
}

void UsbController::setBusReqLevel(CVBusReqLevels level) const {
  checkCAENVMEexception(CAENVME_SetBusReqLevel(this->BHandle_,level));
}

void UsbController::setTimeout(CVVMETimeouts timeout) const {
  checkCAENVMEexception(CAENVME_SetTimeout(this->BHandle_,timeout));
}

void UsbController::setFIFOMode(bool mode) const {
  checkCAENVMEexception(CAENVME_SetFIFOMode(this->BHandle_,(short)mode));
}

CVArbiterTypes UsbController::getArbiterType() const {
  CVArbiterTypes value;
  checkCAENVMEexception(CAENVME_GetArbiterType(this->BHandle_, &value));
  return value;
}

CVRequesterTypes UsbController::getRequesterType() const {
  CVRequesterTypes value;
  checkCAENVMEexception(CAENVME_GetRequesterType(this->BHandle_, &value));
  return value;
}

CVReleaseTypes UsbController::getReleaseType() const {
  CVReleaseTypes value;
  checkCAENVMEexception(CAENVME_GetReleaseType(this->BHandle_, &value));
  return value;
}

CVBusReqLevels UsbController::getBusReqLevel() const {
  CVBusReqLevels value;
  checkCAENVMEexception(CAENVME_GetBusReqLevel(this->BHandle_, &value));
  return value;
}

CVVMETimeouts UsbController::getTimeout() const {
  CVVMETimeouts value;
  checkCAENVMEexception(CAENVME_GetTimeout(this->BHandle_, &value));
  return value;
}

bool UsbController::getFIFOMode() const {
  short value;
  checkCAENVMEexception(CAENVME_GetFIFOMode(this->BHandle_, &value));
  return value;
}

void UsbController::systemReset() const {
  checkCAENVMEexception(CAENVME_SystemReset(this->BHandle_));
  /* note: we don't update the pulsers and scaler so a simple configure() on the restores the previous settings
  pulserA_.update();
  pulserB_.update();
  scaler.update();
  */
}

void UsbController::IRQEnable(uint32_t mask) const {
  checkCAENVMEexception(CAENVME_IRQEnable(this->BHandle_,mask));
}

void UsbController::IRQDisable(uint32_t mask) const {
  checkCAENVMEexception(CAENVME_IRQDisable(this->BHandle_,mask));
}

void UsbController::IRQWait(uint32_t mask, uint32_t timeout_ms) const {
  checkCAENVMEexception(CAENVME_IRQWait(this->BHandle_,mask,timeout_ms));
}

unsigned char UsbController::IRQCheck() const {
  unsigned char output;
  checkCAENVMEexception(CAENVME_IRQCheck(this->BHandle_, &output));
  return output;
}

uint16_t UsbController::IACK(CVIRQLevels level) const {
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
