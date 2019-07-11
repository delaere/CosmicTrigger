#include "VmeController.h"

using namespace std;

vmeController::vmeController(int verbose) {
  this->verbose_ = verbose;
  this->AM_=cvA32_S_DATA;
  this->DW_=cvD16;
  this->AMtmp_=cvA32_S_DATA;
  this->DWtmp_=cvD16;
}

void vmeController::setMode(CVAddressModifier AM, CVDataWidth DW){
  this->AM_=AM;
  this->DW_=DW;
  this->AMtmp_=AM;
  this->DWtmp_=DW;
}

void vmeController::setAM(CVAddressModifier AM){
  this->AM_=AM;
  this->AMtmp_=AM;
}

void vmeController::setDW(CVDataWidth DW){
  this->DW_=DW;
  this->DWtmp_=DW;
}

CVAddressModifier vmeController::getAM(void) const {
  return(this->AM_);
}

CVDataWidth vmeController::getDW(void) const {
  return(this->DW_);
}

const vmeController* vmeController::mode(const CVAddressModifier AM, const CVDataWidth DW) const {
  this->AMtmp_=AM;
  this->DWtmp_=DW;
  return this;
}

std::pair<CVAddressModifier,CVDataWidth> vmeController::useMode() const {
  CVAddressModifier AM = this->AMtmp_;
  CVDataWidth DW = this->DWtmp_;
  this->AMtmp_ = this->AM_;
  this->DWtmp_ = this->DW_;
  return std::make_pair(AM,DW);
}
