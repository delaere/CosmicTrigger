#include "VmeController.h"

using namespace std;

VmeController::VmeController() {
  this->AM_=cvA32_S_DATA;
  this->DW_=cvD16;
  this->AMtmp_=cvA32_S_DATA;
  this->DWtmp_=cvD16;
}

void VmeController::setMode(CVAddressModifier AM, CVDataWidth DW){
  this->AM_=AM;
  this->DW_=DW;
  this->AMtmp_=AM;
  this->DWtmp_=DW;
}

void VmeController::setAM(CVAddressModifier AM){
  this->AM_=AM;
  this->AMtmp_=AM;
}

void VmeController::setDW(CVDataWidth DW){
  this->DW_=DW;
  this->DWtmp_=DW;
}

CVAddressModifier VmeController::getAM(void) const {
  return(this->AM_);
}

CVDataWidth VmeController::getDW(void) const {
  return(this->DW_);
}

const VmeController* VmeController::mode(const CVAddressModifier AM, const CVDataWidth DW) const {
  this->AMtmp_=AM;
  this->DWtmp_=DW;
  LOG_DATA_TRACE("Using the controller in mode" + to_string(AM) + " " + to_string(DW));
  return this;
}

std::tuple<CVAddressModifier,CVDataWidth> VmeController::useMode() const {
  CVAddressModifier AM = this->AMtmp_;
  CVDataWidth DW = this->DWtmp_;
  this->AMtmp_ = this->AM_;
  this->DWtmp_ = this->DW_;
  return std::make_tuple(AM,DW);
}
