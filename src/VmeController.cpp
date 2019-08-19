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
  return this;
}

std::tuple<CVAddressModifier,CVDataWidth> VmeController::useMode() const {
  CVAddressModifier AM = this->AMtmp_;
  CVDataWidth DW = this->DWtmp_;
  this->AMtmp_ = this->AM_;
  this->DWtmp_ = this->DW_;
  return std::make_tuple(AM,DW);
}

template<typename T> void VmeController::writeData(long unsigned int address,T data) const {
  writeDataImpl(&data);
}

template<typename T> T    VmeController::readData (long unsigned int address) const {
  T data;
  readDataImpl(address,&data);
  return data;
}

template<typename T> T    VmeController::readWriteData(const long unsigned int address,T data) const {
  readWriteDataImpl(address,&data);
  return data;
}

template<typename T> std::vector<T> VmeController::blockReadData(const long unsigned int address, int size, bool multiplex) const {
  size_t sizeoft = sizeof(T);
  std::vector<T> output(size);
  void* buffer = output.data();
  int count = 0;
  blockReadDataImpl(address, (unsigned char*)buffer, size*sizeoft , &count, multiplex);
  output.resize(count);
  return output;
}

template<typename T> int VmeController::blockWriteData(const long unsigned int address, std::vector<T> data, bool multiplex) const {
  size_t sizeoft = sizeof(T);
  void* buffer = data.data();
  int count = 0;
  blockWriteDataImpl(address,(unsigned char *)buffer, data.size()*sizeoft, &count, multiplex);
  return count;
}

void VmeController::ADOCycle(const long unsigned int address) const {
  ADOCycleImpl(address);
}
