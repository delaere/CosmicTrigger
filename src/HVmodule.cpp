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

#include "HVmodule.h"
#include "PythonModule.h"
#include "CommonDef.h"
#include "CaenetBridge.h"

#include "N470HVmodule.h"
#include "SY527PowerSystem.h"

using namespace std;

HVBoard::HVBoard(uint32_t slot, std::string name, uint8_t current_unit, uint16_t serial_number,
                 uint16_t software_version, uint8_t nChannels, uint32_t vmax, uint16_t imax,
                 uint16_t rampmin, uint16_t rampmax, uint16_t vres, uint16_t ires, uint16_t vdec, uint16_t idec):
                 slot_(slot),name_(name),current_unit_(current_unit),serial_number_(serial_number),
                 software_version_(software_version),nChannels_(nChannels),vmax_(vmax),imax_(imax),
                 rampmin_(rampmin),rampmax_(rampmax),vres_(vres),ires_(ires),vdec_(vdec),idec_(idec) {
  LOG_TRACE("Board in slot " + to_string(slot) + ":");
  LOG_TRACE("Name: " + name);
  LOG_TRACE("current_unit: " + to_string(current_unit));
  LOG_TRACE("serial_number: " + to_string(serial_number));
  LOG_TRACE("software_version: " + to_string(software_version));
  LOG_TRACE("nChannels: " + to_string(nChannels));
  LOG_TRACE("vmax: " + to_string(vmax));
  LOG_TRACE("imax: " + to_string(imax));
  LOG_TRACE("rampmin: " + to_string(rampmin));
  LOG_TRACE("rampmax: " + to_string(rampmax));
  LOG_TRACE("vres: " + to_string(vres));
  LOG_TRACE("ires: " + to_string(ires));
  LOG_TRACE("vdec: " + to_string(vdec));
  LOG_TRACE("idec: " + to_string(idec));
}

            
HVChannel::HVChannel(uint32_t address, HVBoard& board, uint32_t id, CaenetBridge* bridge):bridge_(bridge),address_(address),id_(id),
  vmon_(0.),imon_(0.),v0_(0),v1_(0),i0_(0),i1_(0),maxV_(3000),softmaxV_(3000),status_(0),rampup_(0),rampdown_(0),trip_(0) {
    // create a bidirectional link between the channel and the board
    attach(board);
  }

uint32_t HVChannel::board() const { return board_ ? board_->getSlot() : 0; }

void HVChannel::attach(HVBoard &board) {
    board_ = &board;
    board.attachWithoutReciprocating(*this);
}

void HVChannel::attachWithoutReciprocating(HVBoard &board) {
    board_ = &board;
}
  
HVModule::HVModule(uint32_t address, CaenetBridge* bridge):bridge_(bridge),address_(address) {
  // read identification string
  bridge_->write({0x1,address_,0});
  auto [ status, data ] = bridge_->readResponse(); checkCAENETexception(status);
  identification_=std::string(data.begin(),data.end());

  // the derived class should do the following
  /*
  // check that the idendity is as expected in the derived class
  assertIdentity();

  // discover boards
  discoverBoards();

  // instantiate the channels
  for( auto & [slot, board] : getBoards() ) {
    for(int i=0;i<board.getNChannels();i++) {
      getChannels()[std::pair(board.getSlot(),i)] = new HVChannel(address,board,i,bridge); 
    }
  }
  */
}

HVModule* HVModule::HVModuleFactory(uint32_t address, CaenetBridge* bridge) {
  // get the identification string
  bridge->write({0x1,address,0});
  auto [ status, data ] = bridge->readResponse(); checkCAENETexception(status);
  auto id = std::string(data.begin(),data.end());
  LOG_TRACE(id);
  
  // instantiate the proper class
  if(id.find("N470") != std::string::npos) {
    return new N470HVModule(address,bridge);
  } else if(id.find("SY527") != std::string::npos) {
    return new SY527PowerSystem(address,bridge);
  }
  
  // fall back
  return nullptr;
}

using namespace boost::python;

template<> void exposeToPython<HVChannel>() {
  struct HVChannelWrap : HVChannel, wrapper<HVChannel> {
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
  class_<HVChannelWrap, boost::noncopyable>("HVChannel", init<uint32_t, HVBoard&, uint32_t, CaenetBridge*>())
    .add_property("id",&HVChannel::id)
    .add_property("board",&HVChannel::board)
    .def("getBoard",&HVChannel::getBoard, return_value_policy<reference_existing_object>())
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
}

template<> void exposeToPython<HVBoard>() {
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
}

template<> void exposeToPython<HVModule>() {
  struct HVModuleWrap : HVModule, wrapper<HVModule> {
    HVModuleWrap(uint32_t address, CaenetBridge* bridge):HVModule(address, bridge), wrapper<HVModule>() {}
    void discoverBoards() override {
      this->get_override("discoverBoards")();
    }
    void assertIdentity() const override {
      this->get_override("assertIdentity")();
    }
  };

  class_<HVModuleWrap, boost::noncopyable>("HVModule", init<uint32_t, CaenetBridge*>())
    .add_property("identification",&HVModule::identification)
    .def("HVModuleFactory",&HVModule::HVModuleFactory,return_value_policy<manage_new_object>())
    .staticmethod("HVModuleFactory") 
    .def("channel",&HVModule::channel,return_value_policy<reference_existing_object>())
    .def("board",&HVModule::board,return_value_policy<copy_non_const_reference>())
    .def("getChannels",&HVModule::getChannels,return_value_policy<copy_non_const_reference>())
    .def("getBoards",&HVModule::getBoards,return_value_policy<copy_non_const_reference>())
  ;
}
