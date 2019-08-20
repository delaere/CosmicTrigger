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

uint32_t vmeRead(VmeController* cont, long unsigned int address) {
  uint32_t data;
  cont->readData(address,&data);
  return data;
}

void vmeWrite(VmeController* cont, long unsigned int address,uint64_t data) {
  cont->writeData(address,&data);
}
  
template<> void exposeToPython<VmeController>() {
  struct VmeControllerWrap : VmeController, wrapper<VmeController> {
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
  def("vmeRead",&vmeRead);
  def("vmeWrite",&vmeWrite);
}

