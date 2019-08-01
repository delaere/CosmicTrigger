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

#include "SY527PowerSystem.h"
#include "CaenetBridge.h"

using namespace std;

SY527HVChannel::SY527HVChannel(uint32_t address, HVBoard& board, uint32_t id, CaenetBridge* bridge):HVChannel(address,board,id,bridge),name_("") {}

void SY527HVChannel::on() {
  // send command
  bridge_->write({0x1,address_,0x18,chAddress(),0x0808});
  // read response
  auto [ status, data ] = bridge_->readResponse(); checkCAENETexception(status);
  LOG_DEBUG("Channel " + to_string(board()) + "." + to_string(id()) + " turned ON.");
}

void SY527HVChannel::off() {
  // send command
  bridge_->write({0x1,address_,0x18,chAddress(),0x0800});
  // read response
  auto [ status, data ] = bridge_->readResponse(); checkCAENETexception(status);
  LOG_DEBUG("Channel " + to_string(board()) + "." + to_string(id()) + " turned OFF.");
}

void SY527HVChannel::setV0(uint32_t v0) {
  v0_ = v0;
  // send command
  bridge_->write({0x1,address_,0x10,chAddress(),v0});
  // read response
  auto [ status, data ] = bridge_->readResponse(); checkCAENETexception(status);
  LOG_DEBUG("Channel " + to_string(board()) + "." + to_string(id()) + ": V0 set to " + to_string(v0));
}

void SY527HVChannel::setV1(uint32_t v1) {
  v1_ = v1;
  // send command
  bridge_->write({0x1,address_,0x11,chAddress(),v1});
  // read response
  auto [ status, data ] = bridge_->readResponse(); checkCAENETexception(status);
  LOG_DEBUG("Channel " + to_string(board()) + "." + to_string(id()) + ": V1 set to " + to_string(v1));
}

void SY527HVChannel::setI0(uint32_t i0) {
  i0_ = i0;
  // send command
  bridge_->write({0x1,address_,0x12,chAddress(),i0});
  // read response
  auto [ status, data ] = bridge_->readResponse(); checkCAENETexception(status);
  LOG_DEBUG("Channel " + to_string(board()) + "." + to_string(id()) + ": I0 set to " + to_string(i0));
}

void SY527HVChannel::setI1(uint32_t i1) {
  i1_ = i1;
  // send command
  bridge_->write({0x1,address_,0x13,chAddress(),i1});
  // read response
  auto [ status, data ] = bridge_->readResponse(); checkCAENETexception(status);
  LOG_DEBUG("Channel " + to_string(board()) + "." + to_string(id()) + ": I1 set to " + to_string(i1));
}

void SY527HVChannel::setRampup(uint32_t rampup) {
  rampup_ = rampup;
  // send command
  bridge_->write({0x1,address_,0x15,chAddress(),rampup});
  // read response
  auto [ status, data ] = bridge_->readResponse(); checkCAENETexception(status);
  LOG_DEBUG("Channel " + to_string(board()) + "." + to_string(id()) + ": Ramp UP set to " + to_string(rampup));
}

void SY527HVChannel::setRampdown(uint32_t rampdown) {
  rampdown_ = rampdown;
  // send command
  bridge_->write({0x1,address_,0x16,chAddress(),rampdown});
  // read response
  auto [ status, data ] = bridge_->readResponse(); checkCAENETexception(status);
  LOG_DEBUG("Channel " + to_string(board()) + "." + to_string(id()) + ": Ramp DOWN set to " + to_string(rampdown));
}

void SY527HVChannel::setTrip(uint32_t trip) {
  trip_ = trip;
  // send command
  bridge_->write({0x1,address_,0x17,chAddress(),trip});
  // read response
  auto [ status, data ] = bridge_->readResponse(); checkCAENETexception(status);
  LOG_DEBUG("Channel " + to_string(board()) + "." + to_string(id()) + ": TRIP set to " + to_string(trip));
}

void SY527HVChannel::setSoftMaxV(uint32_t softmaxV) {
  softmaxV_ = softmaxV;
  // send command
  bridge_->write({0x1,address_,0x14,chAddress(),softmaxV});
  // read response
  auto [ status, data ] = bridge_->readResponse(); checkCAENETexception(status);
  LOG_DEBUG("Channel " + to_string(board()) + "." + to_string(id()) + ": Soft MAXV set to " + to_string(softmaxV));
}

void SY527HVChannel::setPasswordFlag(bool flag) {
  // send command
  bridge_->write({0x1,address_,0x18,chAddress(),(uint32_t)(flag? 0x1010 : 0x1000)});
  // read response
  auto [ status, data ] = bridge_->readResponse(); checkCAENETexception(status);
  LOG_DEBUG("Channel " + to_string(board()) + "." + to_string(id()) + ": password flag set to " + to_string(flag));
}

void SY527HVChannel::setOnOffFlag(bool flag) {
  // send command
  bridge_->write({0x1,address_,0x18,chAddress(),(uint32_t)(flag? 0x4040 : 0x4000)});
  // read response
  auto [ status, data ] = bridge_->readResponse(); checkCAENETexception(status);
  LOG_DEBUG("Channel " + to_string(board()) + "." + to_string(id()) + ": On/Off flag set to " + to_string(flag));
}

void SY527HVChannel::setPoweronFlag(bool flag) {
  // send command
  bridge_->write({0x1,address_,0x18,chAddress(),(uint32_t)(flag? 0x8080 : 0x8000)});
  // read response
  auto [ status, data ] = bridge_->readResponse(); checkCAENETexception(status);
  LOG_DEBUG("Channel " + to_string(board()) + "." + to_string(id()) + ": PowerOn flag set to " + to_string(flag));
}

void SY527HVChannel::readOperationalParameters() {
  // here we need to read both the status data packet and the parameters data packet
  // send command
  bridge_->write({0x1,address_,0x1,chAddress()});
  // read response
  auto [ status1, chStatus ] = bridge_->readResponse(); checkCAENETexception(status1);
  vmon_ = ((chStatus[0]<<16) + chStatus[1])/quick_pow10(board_->getVDecimals());
  maxV_ = chStatus[2];
  imon_ = chStatus[3]/quick_pow10(board_->getIDecimals());
  status_ = chStatus[4];
  // send command
  bridge_->write({0x1,address_,0x2,chAddress()});
  // read response
  auto [ status2, parameters ] = bridge_->readResponse(); checkCAENETexception(status2);
  std::vector<char> characters = { char(parameters[0]>>8), char(parameters[0]&0xFF),
                      char(parameters[1]>>8), char(parameters[1]&0xFF),
                      char(parameters[2]>>8), char(parameters[2]&0xFF),
                      char(parameters[3]>>8), char(parameters[3]&0xFF),
                      char(parameters[4]>>8), char(parameters[4]&0xFF),
                      char(parameters[5]>>8), char(parameters[5]&0xFF) };
  name_ = "";
  for(auto character = characters.begin(); character<characters.end() && (*character); character++) {
    name_ += *character;
  }
  v0_ = parameters[7] + (parameters[6]<<16);
  v1_ = parameters[9] + (parameters[8]<<16);
  i0_ = parameters[10];
  i1_ = parameters[11];
  softmaxV_ = parameters[12];
  rampup_ = parameters[13];
  rampdown_ = parameters[14];
  trip_ = parameters[15];
  status_ += parameters[16]<<16;
  LOG_DEBUG("Operation parameters of channel " + to_string(board()) + "." + to_string(id()) + " updated.");
}

SY527PowerSystem::SY527PowerSystem(uint32_t address, CaenetBridge* bridge):HVModule(address,bridge) { 
  // check that the idendity is as expected in the derived class
  assertIdentity();
  LOG_INFO("New SY527PowerSystem with identification string: "+ identification());

  // discover boards
  discoverBoards();
  LOG_INFO("Has "+to_string(getBoards().size())+ " boards.");

  // instantiate the channels
  for( auto & [slot, board] : getBoards() ) {
    LOG_INFO("Board " + to_string(board.getSlot()) + " has " + to_string(board.getNChannels()) + " channels.");
    for(int i=0;i<board.getNChannels();i++) {
      channels_[std::pair(board.getSlot(),i)] = new SY527HVChannel(address,board,i,bridge); 
    }
  }
}

void SY527PowerSystem::discoverBoards() {
  // read crate occupation
  bridge_->write({0x1,address_,0x4});
  auto [ status, cratemap ] = bridge_->readResponse(); checkCAENETexception(status);
  // cratemap is a bit field. 1-> board present
  for(unsigned int i=0;i<10;i++) {
    if (cratemap[0] & (1<<i)) {
      bridge_->write({0x1,address_,0x3,i});
      auto [ status, boardDesc ] = bridge_->readResponse(); checkCAENETexception(status);
      assert(boardDesc.size()>=27);
      // build the board from the boardDesc
      std::string name = { char(boardDesc[0]>>8),char(boardDesc[0]&0xFF),
                           char(boardDesc[1]>>8),char(boardDesc[1]&0xFF),
                           char(boardDesc[2]>>8)};
      uint8_t currlim  =  boardDesc[2]&0xFF;
      uint16_t sernum  = boardDesc[3];
      uint16_t ver     = boardDesc[4];
      uint8_t nchan    = (boardDesc[15]>>8);
      uint32_t vmax    = (boardDesc[19]>>8)+(boardDesc[18]<<8)+((boardDesc[17]&0xFF)<<24);
      uint16_t imax    = (boardDesc[20]>>8)+((boardDesc[19]&0xFF)<<8);
      uint16_t rpmin   = (boardDesc[21]>>8)+((boardDesc[20]&0xFF)<<8);
      uint16_t rpmax   = (boardDesc[22]>>8)+((boardDesc[21]&0xFF)<<8);
      uint16_t vres    = (boardDesc[23]>>8)+((boardDesc[22]&0xFF)<<8);
      uint16_t ires    = (boardDesc[24]>>8)+((boardDesc[23]&0xFF)<<8);
      uint16_t vdec    = (boardDesc[25]>>8)+((boardDesc[24]&0xFF)<<8);
      uint16_t idec    = (boardDesc[26]>>8)+((boardDesc[25]&0xFF)<<8);
      boards_[i]  = HVBoard(i,name,currlim,sernum,ver,nchan,vmax,imax,rpmin,rpmax,vres,ires,vdec,idec);
    }
  }
}
  
void SY527PowerSystem::assertIdentity() const {
  // this should be the start of the string.
  assert(identification().find("SY527") != std::string::npos);
}

void SY527PowerSystem::updateStatus() {
  // there is no generic command, so loop on channels and call readOperationalParameters
  for (auto [key,channel] : getChannels()) {
    channel->readOperationalParameters();
  }
}
