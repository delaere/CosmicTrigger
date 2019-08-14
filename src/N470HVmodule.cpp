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

#include "N470HVmodule.h"
#include "CaenetBridge.h"

using namespace std;

N470HVChannel::N470HVChannel(uint32_t address, HVBoard& board, uint32_t id, CaenetBridge* bridge):HVChannel(address,board,id,bridge){}

void N470HVChannel::on() {
  // send command
  bridge_->write({0x1,address_,(id_<<8)+0xA});
  // read response
  auto [ status, data ] = bridge_->readResponse(); checkCAENETexception(status);
  status_ = data[0];
  LOG_DEBUG("Channel " + to_string(board()) + "." + to_string(id()) + " turned ON.");
}

void N470HVChannel::off() {
  // send command
  bridge_->write( {0x1,address_,(id_<<8)+0xB});
  // read response
  auto [ status, data ] = bridge_->readResponse(); checkCAENETexception(status);
  status_ = data[0];
  LOG_DEBUG("Channel " + to_string(board()) + "." + to_string(id()) + " turned OFF.");
}

void N470HVChannel::setV0(uint32_t v0) {
  v0_ = v0;
  // send command
  bridge_->write({0x1,address_,(id_<<8)+0x3,v0});
  // read response
  auto [ status, data ] = bridge_->readResponse(); checkCAENETexception(status);
  status_ = data[0];
  LOG_DEBUG("Channel " + to_string(board()) + "." + to_string(id()) + ": V0 set to " + to_string(v0));
}

void N470HVChannel::setV1(uint32_t v1) {
  v1_ = v1;
  // send command
  bridge_->write({0x1,address_,(id_<<8)+0x5,v1});
  // read response
  auto [ status, data ] = bridge_->readResponse(); checkCAENETexception(status);
  status_ = data[0];
  LOG_DEBUG("Channel " + to_string(board()) + "." + to_string(id()) + ": V1 set to " + to_string(v1));
}

void N470HVChannel::setI0(uint32_t i0) {
  i0_ = i0;
  // send command
  bridge_->write({0x1,address_,(id_<<8)+0x4,i0});
  // read response
  auto [ status, data ] = bridge_->readResponse(); checkCAENETexception(status);
  status_ = data[0];
  LOG_DEBUG("Channel " + to_string(board()) + "." + to_string(id()) + ": I0 set to " + to_string(i0));
}

void N470HVChannel::setI1(uint32_t i1) {
  i1_ = i1;
  // send command
  bridge_->write({0x1,address_,(id_<<8)+0x6,i1});
  // read response
  auto [ status, data ] = bridge_->readResponse(); checkCAENETexception(status);
  status_ = data[0];
  LOG_DEBUG("Channel " + to_string(board()) + "." + to_string(id()) + ": I1 set to " + to_string(i1));
}

void N470HVChannel::setRampup(uint32_t rampup) {
  rampup_ = rampup;
  // send command
  bridge_->write({0x1,address_,(id_<<8)+0x8,rampup});
  // read response
  auto [ status, data ] = bridge_->readResponse(); checkCAENETexception(status);
  status_ = data[0];
  LOG_DEBUG("Channel " + to_string(board()) + "." + to_string(id()) + ": Ramp UP set to " + to_string(rampup));
}

void N470HVChannel::setRampdown(uint32_t rampdown) {
  rampdown_ = rampdown;
  // send command
  bridge_->write({0x1,address_,(id_<<8)+0x9,rampdown});
  // read response
  auto [ status, data ] = bridge_->readResponse(); checkCAENETexception(status);
  status_ = data[0];
  LOG_DEBUG("Channel " + to_string(board()) + "." + to_string(id()) + ": Ramp DOWN set to " + to_string(rampdown));
}

void N470HVChannel::setTrip(uint32_t trip) {
  trip_ = trip;
  // send command
  bridge_->write({0x1,address_,(id_<<8)+0x7,trip});
  // read response
  auto [ status, data ] = bridge_->readResponse(); checkCAENETexception(status);
  status_ = data[0];
  LOG_DEBUG("Channel " + to_string(board()) + "." + to_string(id()) + ": TRIP set to " + to_string(trip));
}

void N470HVChannel::setSoftMaxV(uint32_t) {
  throw_with_trace(CAENETexception(0xFF01));
}

void N470HVChannel::readOperationalParameters() {
  // send command
  bridge_->write({0x1,address_,(id_<<8)+0x2});
  // read response
  auto [ status, data ] = bridge_->readResponse(); checkCAENETexception(status);
  std::tie(status_, vmon_, imon_, v0_, i0_, v1_, i1_, trip_, rampup_, rampdown_, maxV_) = gen_tuple<11>(data);
  LOG_DEBUG("Operation parameters of channel " + to_string(board()) + "." + to_string(id()) + " updated.");
}

void N470HVChannel::setStatus(std::vector<uint32_t>::const_iterator data) {
  vmon_   = *data++;
  imon_   = *data++;
  maxV_   = *data++;
  status_ = *data++;
  LOG_DEBUG("vmon,imon,maxV,status = " + to_string(vmon_) + ", " + to_string(imon_) + ", " + to_string(maxV_) + ", " + int_to_hex(status_));
}

N470HVModule::N470HVModule(uint32_t address, CaenetBridge* bridge):HVModule(address,bridge) { 
  // check that the idendity is as expected in the derived class
  assertIdentity();
  LOG_INFO("New N470HVModule with identification string: "+ identification());

  // discover boards
  discoverBoards();
  LOG_INFO("Has "+to_string(getBoards().size())+ " boards.");

  // instantiate the channels
  for( auto & [slot, board] : getBoards() ) {
    LOG_INFO("Board " + to_string(board.getSlot()) + " has " + to_string(board.getNChannels()) + " channels.");
    for(int i=0;i<board.getNChannels();i++) {
      channels_[std::pair(board.getSlot(),i)] = new N470HVChannel(address,board,i,bridge); 
    }
  }
}

void N470HVModule::discoverBoards() {
  // only one board in that module. Cannot be dynamically discovered.
  boards_[0] = HVBoard(0,"N470", 2, 0, 0, 4, 8000, 3000, 0, 500, 500, 1000, 0, 0);
}
  
void N470HVModule::assertIdentity() const {
  // this should be the start of the string.
  assert(identification().find("N470") != std::string::npos);
}

void N470HVModule::updateStatus() {
  // send command
  bridge_->write({0x1,address_,1});
  // read response
  auto [ status, data ] = bridge_->readResponse(); checkCAENETexception(status);
  for(auto [key,channel] : getChannels()) {
    LOG_DEBUG("set status of channel " + to_string(key.first) + "." + to_string(key.second));
    static_cast<N470HVChannel*>(channel)->setStatus(data.begin()+(channel->id()*4));
  }
}

void N470HVModule::kill() {
  // send command
  bridge_->write({0x1,address_,12});
  // read response
  auto [ status, data ] = bridge_->readResponse(); checkCAENETexception(status);
  LOG_DEBUG("Module killed");
}

void N470HVModule::clearAlarm() {
  // send command
  bridge_->write({0x1,address_,13});
  // read response
  auto [ status, data ] = bridge_->readResponse(); checkCAENETexception(status);
  LOG_DEBUG("Alarm cleared");
}

void N470HVModule::enableKeyboard(bool enable) {
  uint8_t opcode = enable ? 14 : 15;
  // send command
  bridge_->write({0x1,address_,opcode});
  // read response
  auto [ status, data ] = bridge_->readResponse(); checkCAENETexception(status);
  LOG_DEBUG("Keyboard enabled:" + to_string(enable));
}

void N470HVModule::setTTL() {
  // send command
  bridge_->write({0x1,address_,16});
  // read response
  auto [ status, data ] = bridge_->readResponse(); checkCAENETexception(status);
  LOG_DEBUG("Input set to TTL");
}

void N470HVModule::setNIM() {
  // send command
  bridge_->write({0x1,address_,17});
  // read response
  auto [ status, data ] = bridge_->readResponse(); checkCAENETexception(status);
  LOG_DEBUG("Input set to NIM");
}
