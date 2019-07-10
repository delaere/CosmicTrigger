#include "VmeUsbBridge.h"
#include <iostream>

UsbController::UsbController(int verbose){
  this->verbose_=verbose;
  this->board_=V1718;
  char SwRel[128];
  CAENVME_API status = 0;

  status = CAENVME_Init((CVBoardTypes)(int)board, 0, 0, &this->BHandle);
  if (status) {
    throw CAENVMEexception(status);
  }
  this->AM_=A32_S_DATA;
  this->DW_=D16;
  this->AMtmp_=A32_S_DATA;
  this->DWtmp_=D16;
  
  this->pulserA_ = V1718Pulser(this->BHandle_,cvPulserA);
  this->pulserB_ = V1718Pulser(this->BHandle_,cvPulserB);
  this->scaler_  = V1718Scaler(this->BHandle_);

  status = CAENVME_BoardFWRelease(handle,FWRel);
  if (status) {
    throw CAENVMEexception(status);
  }
  this->firmwareVersion_ = std::string(SwRel);
  
  if(verbose>=3) {
    std::cout << "VME USB Init... ok!"<< std::endl;
    std::cout << "Firmware version: " << this->firmwareVersion << std::endl;
  }
}

UsbController::~UsbController(){
  CAENVME_API status = CAENVME_End(this->BHandle_);
  if (status) {
    throw CAENVMEexception(status);
  }
  if(verbose>=3) {
    std::cout << "Disconnected from USB controler." << endl; 
  }
}

void UsbController::setMode(CVAddressModifier AM, CVDataWidth DW){
  this->AM_=AM;
  this->DW_=DW;
  this->AMtmp_=AM;
  this->DWtmp_=DW;
}

void UsbController::setAM(CVAddressModifier AM){
  this->AM_=AM;
  this->AMtmp_=AM;
}

void UsbController::setDW(CVDataWidth DW){
  this->DW_=DW;
  this->DWtmp_=DW;
}

CVAddressModifier UsbController::getAM(void){
  return(this->AM);
  
}

CVDataWidth UsbController::getDW(void){
  return(this->DW);
}

UsbController* UsbController::mode(const CVAddressModifier AM, const CVDataWidth DW){
  this->AMtmp_=AM;
  this->DWtmp_=DW;
}

int UsbController::writeData(long unsigned int address,void* data){
  CVAddressModifier AM = this->AMtmp_;
  CVDataWidth DW = this->DWtmp_;
  this->AMtmp_ = this->AM_;
  this->DWtmp_ = this->DW_;
  return(CAENVME_WriteCycle(this->BHandle,address,data,AM,DW);
}

int UsbController::readData(long unsigned int address,void* data){
  CVAddressModifier AM = this->AMtmp_;
  CVDataWidth DW = this->DWtmp_;
  this->AMtmp_ = this->AM_;
  this->DWtmp_ = this->DW_;
  return(CAENVME_ReadCycle(this->BHandle,address,data,AM,DW);
}

V1718Pulser& UsbController::getPulser(CVPulserSelect pulser){
  if(pulser==cvPulserA) 
    return this->pulserA_;
  else 
    return this->pulserB_;
}

V1718Scaler& UsbController::getScaler(){
  return this->scaler_;
}

CVDisplay UsbController::readDisplay() const {
  CVDisplay value;
  CAENVME_API status = CAENVME_ReadDisplay(this->BHandle_, &value);
  if (status) {
    throw CAENVMEexception(status);
  }
  return value;
}

void UsbController::systemReset(){
  CAENVME_API status = CAENVME_SystemReset(this->BHandle_);
  if (status) {
    throw CAENVMEexception(status);
  }
  /* note: we don't update the pulsers and scaler so a simple configure() on the restores the previous settings
  pulserA_.update();
  pulserB_.update();
  scaler.update();
  */
}

void UsbController::IRQEnable(uint32_t mask) const {
  CAENVME_API status = CAENVME_IRQEnable(this->BHandle_,mask);
  if (status) {
    throw CAENVMEexception(status);
  }
}

void UsbController::IRQDisable(uint32_t mask) const {
  CAENVME_API status = CAENVME_IRQDisable(this->BHandle_,mask);
  if (status) {
    throw CAENVMEexception(status);
  }
}

void UsbController::IRQWait(uint32_t mask, uint32_t timeout_ms) const {
  CAENVME_API status = CAENVME_IRQEnable(this->BHandle_,mask,timeout_ms);
  if (status) {
    throw CAENVMEexception(status);
  }
}

unsigned char IRQCheck() const {
  unsigned char output;
  CAENVME_API status = CAENVME_IRQCheck(this->BHandle_, &output);
  if (status) {
    throw CAENVMEexception(status);
  }
  return output;
}

uint16_t IACK(CVIRQLevels level) const {
  CVDataWidth DW = this->DWtmp_;
  this->DWtmp_ = this->DW_;
  uint16_t vector;
  CAENVME_API status = CAENVME_IACKCycle(this->BHandle_, level, &vector, DW);
  if (status) {
    throw CAENVMEexception(status);
  }
  return vector;
}

V1718Pulser::V1718Pulser(int32_t* handle, CVPulserSelect id){
  this->BHandle_ = handle;
  this->pulserId_ = id;
  this->update();
}

void V1718Pulser::configure(){
  CAENVME_API status = 0;
  status = CAENVME_SetPulserConf(this->BHandle_, this->pulserId_,
                                 this->period_,this->width_,
                                 this->units_,this->pulseNo_,
                                 this->start_,this->reset_);
  if (status) {
    throw CAENVMEexception(status);
  }
  this->configured_ = true;
}

void V1718Pulser::start(){
  if (!this->configured_ ) configure();
  if (this->start_ != cvManualSW) throw CAENVMEexception(cvInvalidParam);
  CAENVME_API status = CAENVME_StartPulser(this->BHandle_, this->pulserId_);
  if (status) {
    throw CAENVMEexception(status);
  }
}

void V1718Pulser::stop(){
  if (!this->configured_ ) configure();
  if (this->stop_ != cvManualSW) throw CAENVMEexception(cvInvalidParam);
  CAENVME_API status = CAENVME_StopPulser(this->BHandle_, this->pulserId_);
  if (status) {
    throw CAENVMEexception(status);
  }
}

void V1718Pulser::update(){
  CAENVME_API status = 0;
  status = CAENVME_GetPulserConf(this->BHandle_, this->pulserId_, 
                                 &(this->period_),&(this->width_),
                                 &(this->units_),&(this->pulseNo_),
                                 &(this->start_),&(this->reset_));
  if (status) {
    throw CAENVMEexception(status);
  }
  this->configured_ = true;
}

V1718Scaler::V1718Scaler(int32_t* handle){
  this->BHandle_ = handle;
  this->update();
}

void V1718Scaler::configure(){
  CAENVME_API status = 0;
  status = CAENVME_SetScalerConf(this->BHandle_, 
                                 this->limit_,this->autoReset_,
                                 this->hit_,this->gate_,
                                 this->reset_);
  if (status) {
    throw CAENVMEexception(status);
  }
  this->configured_ = true;
}

void V1718Scaler::update(){
  CAENVME_API status = 0;
  status = CAENVME_GetScalerConf(this->BHandle_,
                                 &(this->limit_),&(this->autoReset_),
                                 &(this->hit_),&(this->gate_),
                                 &(this->reset_));
  if (status) {
    throw CAENVMEexception(status);
  }
  this->configured_ = true;
}

void V1718Scaler::resetCount(){
  if (!this->configured_ ) configure();
  if (this->reset_ != cvManualSW) throw CAENVMEexception(cvInvalidParam);
  CAENVME_API status = 0;
  status = CAENVME_ResetScalerCount(this->BHandle_);
  if (status) {
    throw CAENVMEexception(status);
  }
}

void V1718Scaler::enableGate(){
  if (!this->configured_ ) configure();
  if (this->gate_ != cvManualSW) throw CAENVMEexception(cvInvalidParam);
  CAENVME_API status = 0;
  status = CAENVME_EnableScalerGate(this->BHandle_);
  if (status) {
    throw CAENVMEexception(status);
  }
}

void V1718Scaler::disableGate(){
  if (!this->configured_ ) configure();
  if (this->gate_ != cvManualSW) throw CAENVMEexception(cvInvalidParam);
  CAENVME_API status = 0;
  status = CAENVME_DisableScalerGate(this->BHandle_);
  if (status) {
    throw CAENVMEexception(status);
  }
}

unsigned short V1718Scaler::count() const {
  CAENVME_API status = 0;
  unsigned short data = 0;
  status = CAENVME_ReadRegister(this->BHandle_, cvScaler1, &data);
  if (status) {
    throw CAENVMEexception(status);
  }
  return (data & 1023);
}
