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
#include "PythonModule.h"

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

using namespace boost::python;

template<> void exposeToPython<VmeController>() {
  struct VmeControllerWrap : VmeController, wrapper<VmeController> {
    VmeControllerWrap():VmeController(),wrapper<VmeController>() {}
    void writeDataImpl(long unsigned int address,void* data) const override {
      this->get_override("writeDataImpl")(address,data);
    }
    void readDataImpl(long unsigned int address,void* data) const override {
      this->get_override("readDataImpl")(address,data);
    }
    void readWriteDataImpl(const long unsigned int address,void* data) const override {
      this->get_override("readWriteDataImpl")(address,data);
    }
    void blockReadDataImpl(const long unsigned int address,unsigned char *buffer, int size, int *count, bool multiplex=false) const override {
      this->get_override("blockReadDataImpl")(address,buffer,size,count,multiplex);
    }
    void blockWriteDataImpl(const long unsigned int address,unsigned char *buffer, int size, int *count, bool multiplex=false) const override {
      this->get_override("blockWriteDataImpl")(address,buffer,size,count,multiplex);
    }
    void ADOCycleImpl(const long unsigned int address) const override {
      this->get_override("ADOCycleImpl")(address);
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
  
  class_<VmeControllerWrap, boost::noncopyable>("VmeController")
    .def("setMode", &VmeController::setMode)
    .def("mode", &VmeController::mode,return_value_policy<manage_new_object>())
    .add_property("AM",&VmeController::getAM,&VmeController::setAM)
    .add_property("DW",&VmeController::getDW,&VmeController::setDW)
    .def("writeData16",&VmeController::writeData<uint16_t>)
    .def("readData16",&VmeController::readData<uint16_t>)
    .def("readWriteData16",&VmeController::readWriteData<uint16_t>)
    .def("blockReadData16",&VmeController::blockReadData<uint16_t>)
    .def("blockWriteData16",&VmeController::blockWriteData<uint16_t>)
    .def("writeData32",&VmeController::writeData<uint32_t>)
    .def("readData32",&VmeController::readData<uint32_t>)
    .def("readWriteData32",&VmeController::readWriteData<uint32_t>)
    .def("blockReadData32",&VmeController::blockReadData<uint32_t>)
    .def("blockWriteData32",&VmeController::blockWriteData<uint32_t>)
    .def("writeData64",&VmeController::writeData<uint64_t>)
    .def("readData64",&VmeController::readData<uint64_t>)
    .def("readWriteData64",&VmeController::readWriteData<uint64_t>)
    .def("blockReadData64",&VmeController::blockReadData<uint64_t>)
    .def("blockWriteData64",&VmeController::blockWriteData<uint64_t>)
    .def("ADOCycle",&VmeController::ADOCycle)
    .def("IRQEnable",pure_virtual(&VmeController::IRQEnable))
    .def("IRQDisable",pure_virtual(&VmeController::IRQDisable))
    .def("IRQWait",pure_virtual(&VmeController::IRQWait))
    .def("IRQCheck",pure_virtual(&VmeController::IRQCheck))
    .def("IACK",pure_virtual(&VmeController::IACK))
  ;
}

