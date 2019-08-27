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
#include "PythonModule.h"

using namespace std;

SY527HVChannel::SY527HVChannel(uint32_t address, const HVBoard& board, uint32_t id, uint8_t type, CaenetBridge* bridge):HVChannel(address,board,id,type,bridge),name_(""),priorityOn_(0),priorityOff_(0) {}

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
  vmon_ = ((chStatus[0]<<16) + chStatus[1])/float(quick_pow10(getProperties().getVDecimals()));
  maxV_ = chStatus[2];
  imon_ = chStatus[3]/float(quick_pow10(getProperties().getIDecimals()));
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

void SY527HVChannel::setName(std::string name) {
  std::vector<uint32_t> command = {0x1,address_,0x19,chAddress()};
  for(uint c = 0; c < name.size(); c+=2) {
    command.push_back(name[c]<<8|name[c+1]);
  }
  if(!(name.size()%2)) command.push_back(0);
  bridge_->write(command);
  auto [ status, data ] = bridge_->readResponse(); checkCAENETexception(status);
  name_=name;
}

SY527PowerSystem::SY527PowerSystem(uint32_t address, CaenetBridge* bridge):HVModule(address,bridge) { 
  // check that the idendity is as expected in the derived class
  assertIdentity();
  LOG_INFO("New SY527PowerSystem with identification string: "+ identification());

  // discover boards
  discoverBoards();
  LOG_INFO("Has "+to_string(getBoards().size())+ " boards.");

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
      uint16_t sernum  = boardDesc[3];
      uint16_t ver     = boardDesc[4];
      uint8_t nchan    = (boardDesc[15]>>8);
      HVBoard board(i,name, sernum, ver, nchan);
      LOG_INFO("Board " + to_string(board.getSlot()) + " has " + to_string(board.getNChannels()) + " channels.");
      
      // type of board
      bool homogeneous  = !((boardDesc[16]>>9)&0x1);
      if(homogeneous) {
        // data for uniform boards
        uint8_t curruni  =  boardDesc[2]&0xFF;
        uint32_t vmax    = (boardDesc[19]>>8)+(boardDesc[18]<<8)+((boardDesc[17]&0xFF)<<24);
        uint16_t imax    = (boardDesc[20]>>8)+((boardDesc[19]&0xFF)<<8);
        uint16_t rpmin   = (boardDesc[21]>>8)+((boardDesc[20]&0xFF)<<8);
        uint16_t rpmax   = (boardDesc[22]>>8)+((boardDesc[21]&0xFF)<<8);
        uint16_t vres    = (boardDesc[23]>>8)+((boardDesc[22]&0xFF)<<8);
        uint16_t ires    = (boardDesc[24]>>8)+((boardDesc[23]&0xFF)<<8);
        uint16_t vdec    = (boardDesc[25]>>8)+((boardDesc[24]&0xFF)<<8);
        uint16_t idec    = (boardDesc[26]>>8)+((boardDesc[25]&0xFF)<<8);
        ChannelProperties props(curruni,vmax,imax,rpmin,rpmax,vres,ires,vdec,idec);
        board.add(props);
        boards_.insert({i,board});
        // instantiate the channels
        for(int ch=0;ch<nchan;ch++) {
          channels_[std::pair(i,ch)] = new SY527HVChannel(address_,boards_[i],ch,0,bridge_);
        }
      } else {
        // look at additional words to build various channel types
        uint16_t numtypes = boardDesc[27];        
        // build the various channel types and add them to the board
        for(uint8_t tpe = 0; tpe<numtypes; ++tpe) {
          unsigned int startword = 31+(nchan-1)/2+14*tpe;
          uint8_t curruni  =  boardDesc[startword]>>8;
          uint32_t vmax    = (boardDesc[startword+3]<<16)+boardDesc[startword+4];
          uint16_t imax    =  boardDesc[startword+5];
          uint16_t rpmin   =  boardDesc[startword+6];
          uint16_t rpmax   =  boardDesc[startword+7];
          uint16_t vres    =  boardDesc[startword+8];
          uint16_t ires    =  boardDesc[startword+9];
          uint16_t vdec    =  boardDesc[startword+10];
          uint16_t idec    =  boardDesc[startword+11];
          ChannelProperties props(curruni,vmax,imax,rpmin,rpmax,vres,ires,vdec,idec);
          board.add(props);
        }
        boards_.insert({i,board});
        LOG_INFO("Board " + to_string(board.getSlot()) + " has " + to_string(numtypes) + " channels types.");
        // instantiate the channels
        for(int ch=0;ch<nchan;ch++) {
          uint8_t chtype = (ch%2) ? ((boardDesc[28+ch/2])&0xFF) : ((boardDesc[28+ch/2])>>8);
          channels_[std::pair(i,ch)] = new SY527HVChannel(address_,boards_[i],ch,chtype,bridge_);
        }
      }
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

uint16_t SY527PowerSystem::getHWStatus() {
  bridge_->write({0x1,address_,0x7});
  auto [ status, hwstatus ] = bridge_->readResponse(); checkCAENETexception(status);
  return hwstatus[0];
}
  
void SY527PowerSystem::selfTest(bool alwaysRestart) {
  bridge_->write({0x1,address_,uint16_t(alwaysRestart ? 0x8 : 0x9)});
  LOG_INFO("Triggered a self-test. Wait few seconds before checking the HW status.");
}

void SY527PowerSystem::formatEEPROM() {
  bridge_->write({0x1,address_,0x30});
  // read response
  auto [ status, data ] = bridge_->readResponse(); checkCAENETexception(status);
  // confirm
  bridge_->write({0x1,address_,0x31});
  std::tie(status, data) = bridge_->readResponse(); checkCAENETexception(status);
}

void SY527PowerSystem::clearAlarm() {
  bridge_->write({0x1,address_,0x32});
  auto [ status, data ] = bridge_->readResponse(); checkCAENETexception(status);
}

void  SY527PowerSystem::lockKeyboard(bool lock) {
  bridge_->write({0x1,address_,(uint16_t)(lock ? 0x33 : 0x34)});
  auto [ status, data ] = bridge_->readResponse(); checkCAENETexception(status);
}
  
void  SY527PowerSystem::killAll() {
  bridge_->write({0x1,address_,0x35});
  auto [ status, data ] = bridge_->readResponse(); checkCAENETexception(status);
  bridge_->write({0x1,address_,0x36});
  std::tie(status,data) = bridge_->readResponse(); checkCAENETexception(status);
}

ChannelGroup SY527PowerSystem::getGroup(uint n) {
  bridge_->write({0x1,address_,0x40, n});
  auto [ status, groupDefinition ] = bridge_->readResponse(); checkCAENETexception(status);
  // group name
  std::string name = { char(groupDefinition[0]>>8),char(groupDefinition[0]&0xFF),
                       char(groupDefinition[1]>>8),char(groupDefinition[1]&0xFF),
                       char(groupDefinition[2]>>8),char(groupDefinition[2]&0xFF),
                       char(groupDefinition[3]>>8),char(groupDefinition[3]&0xFF),
                       char(groupDefinition[4]>>8),char(groupDefinition[4]&0xFF),
                       char(groupDefinition[5]>>8)};
  ChannelGroup group(n,name,bridge_,address_);
  // channels
  for(uint i = 6;i<groupDefinition.size();i+=2) {
    uint8_t board = groupDefinition[i]>>8;
    uint8_t chan = groupDefinition[i]&0xFF;
    uint8_t priorityOn = groupDefinition[i+1]>>8;
    uint8_t priorityOff = groupDefinition[i+1]&0xFF;
    group.insert((SY527HVChannel*)(channel(board,chan)));
    group.back()->setPriorityON(priorityOn);
    group.back()->setPriorityOFF(priorityOff);
  }
  // return the group
  return group;
}

uint32_t SY527PowerSystem::getGeneralStatus() {
  bridge_->write({0x1,address_,0x5});
  auto [ status, globalStatus ] = bridge_->readResponse(); checkCAENETexception(status);
  return (globalStatus[0]<<16)|globalStatus[1];
}

void SY527PowerSystem::programAlarms(bool levelHigh, bool pulsedAlarm, bool OVCalarm, bool OVValarm, bool UNValarm) {
  uint16_t status = levelHigh | (pulsedAlarm<<1) | (OVCalarm<<2) | (OVValarm<<3) | (UNValarm<<4);
}

std::vector<SY527PowerSystem::Checksum> SY527PowerSystem::checksum(bool current){
  bridge_->write({0x1,address_,(uint16_t)(current? 0x61 : 0x60)});
  auto [ status, response ] = bridge_->readResponse(); checkCAENETexception(status);
  // convert vector<uint16_t> to vector<checksum> (1 to 2)
  std::vector<SY527PowerSystem::Checksum> checksums;
  for (auto word : response) {
    checksums.push_back(SY527PowerSystem::Checksum(word>>8));
    checksums.push_back(SY527PowerSystem::Checksum(word&0xFF));
  }
  return checksums;
}


ChannelGroup::ChannelGroup(uint id, std::string name, CaenetBridge* bridge, uint32_t address):id_(id),name_(name),bridge_(bridge),address_(address) {
}

struct channelEqual {
  SY527HVChannel* m_value;
  channelEqual(SY527HVChannel* value) : m_value(value) {}
  bool operator()(const SY527HVChannel* cls) const {
    return ((cls->board()<<8)|cls->id()) == ((m_value->board()<<8)|m_value->id());
  }
};

struct channelIdx {
  std::pair<uint32_t,uint32_t> m_value;
  channelIdx(std::pair<uint32_t,uint32_t> value) : m_value(value) {}
  bool operator()(const SY527HVChannel* cls) const {
    return ((cls->board()<<8)|cls->id()) == ((m_value.first<<8)|m_value.second);
  }
};

ChannelGroup::iterator       ChannelGroup::find(const value_type& x) { 
  return std::find_if(begin(),end(),channelEqual(x)); 
}

ChannelGroup::const_iterator ChannelGroup::find(const value_type& x) const { 
  return std::find_if(begin(),end(),channelEqual(x)); 
}

ChannelGroup::iterator       ChannelGroup::find(const key_type& x) { 
  return std::find_if(begin(),end(),channelIdx(x)); 
}

ChannelGroup::const_iterator ChannelGroup::find(const key_type& x) const { 
  return std::find_if(begin(),end(),channelIdx(x)); 
}

std::pair<ChannelGroup::iterator,bool> ChannelGroup::insert(const value_type& x) {
  // check that the channel is not yet in the group
  ChannelGroup::iterator item = find(x);
  if(item != end()) {
    return make_pair(item,false);
  } else {
    // inserts the channel
    channels_.push_back(x);
    addChannel((x->board()<<8)|x->id());
    return std::make_pair(end()-1,true);
  }
}

ChannelGroup::iterator ChannelGroup::erase(ChannelGroup::const_iterator position) {
  // remove it from the hardware group
  removeChannel((*position)->board()<<8|(*position)->id());
  // remove from the set
  return channels_.erase(position);
}

ChannelGroup::size_type ChannelGroup::erase(const ChannelGroup::value_type& x){
  // find the element in the group
  ChannelGroup::const_iterator item = find(x);
  if(item != end()) {
    // remove it from the hardware group
    removeChannel(x->board()<<8|x->id());
    // remove from the set
    channels_.erase(item);
    return 1;
  } else {
    return 0;
  }
}

ChannelGroup::size_type ChannelGroup::erase(const ChannelGroup::key_type& x){
  // find the element in the group
  ChannelGroup::const_iterator item = find(x);
  if(item != end()) {
    // remove it from the hardware group
    removeChannel((*item)->board()<<8|(*item)->id());
    // remove from the set
    channels_.erase(item);
    return 1;
  } else {
    return 0;
  }
}

ChannelGroup::iterator ChannelGroup::erase(ChannelGroup::const_iterator first, ChannelGroup::const_iterator last){
  // remove all from hardware group
  for(ChannelGroup::const_iterator position = first; position<last ; ++position) {
    removeChannel((*position)->board()<<8|(*position)->id());
  }
  // remove from the set
  return channels_.erase(first,last); 
}

void ChannelGroup::addChannel(uint16_t num) {
  bridge_->write({0x1,address_,0x50, id_, num});
  auto [ status, response ] = bridge_->readResponse(); checkCAENETexception(status);
}

void ChannelGroup::removeChannel(uint16_t num) {
  bridge_->write({0x1,address_,0x51, id_, num});
  auto [ status, response ] = bridge_->readResponse(); checkCAENETexception(status);
}

void ChannelGroup::readParameters() {
  bridge_->write({0x1,address_,0x41,id_});
  auto [ status, grStatus ] = bridge_->readResponse(); checkCAENETexception(status);
  
  for(uint i = 0; i<channels_.size(); ++i) {
    channels_[i]->vmon_ = ((grStatus[0+i*5]<<16) + grStatus[1+i*5])/float(quick_pow10(channels_[i]->getProperties().getVDecimals()));
    channels_[i]->maxV_ = grStatus[2+i*5];
    channels_[i]->imon_ = grStatus[3+i*5]/float(quick_pow10(channels_[i]->getProperties().getIDecimals()));
    channels_[i]->status_ = grStatus[4+i*5]&0xFFFF | (channels_[i]->status_&0xFFFF0000); 
  }
}

void ChannelGroup::readSettings() {
  bridge_->write({0x1,address_,0x43,id_});
  auto [ status, grStatus ] = bridge_->readResponse(); checkCAENETexception(status);
  for(uint i = 0; i<channels_.size(); ++i) {
    channels_[i]->v0_ = (grStatus[0+i*3]<<16) + grStatus[1+i*3];
    channels_[i]->i0_ = grStatus[2+i*3];
  }
  bridge_->write({0x1,address_,0x44,id_});
  std::tie( status, grStatus ) = bridge_->readResponse(); checkCAENETexception(status);
  for(uint i = 0; i<channels_.size(); ++i) {
    channels_[i]->v1_ = (grStatus[0+i*3]<<16) + grStatus[1+i*3];
    channels_[i]->i1_ = grStatus[2+i*3];
  }
  bridge_->write({0x1,address_,0x45,id_});
  std::tie( status, grStatus ) = bridge_->readResponse(); checkCAENETexception(status);
  for(uint i = 0; i<channels_.size(); ++i) {
    channels_[i]->softmaxV_ = grStatus[0+i*3];
    channels_[i]->trip_ = grStatus[1+i*3];
    channels_[i]->status_ = (grStatus[2+i*3]<<16) | (channels_[i]->status_&0xFFFF);
  }
  bridge_->write({0x1,address_,0x46,id_});
  std::tie( status, grStatus ) = bridge_->readResponse(); checkCAENETexception(status);
  for(uint i = 0; i<channels_.size(); ++i) {
    channels_[i]->rampup_ = grStatus[0+i*2];
    channels_[i]->rampdown_ = grStatus[1+i*2];
  }
}

void ChannelGroup::setV0(uint32_t v0) {
  for(auto channel : channels_) { channel->v0_ = v0; }
  // send command
  bridge_->write({0x1,address_,0x52,id_,v0});
  // read response
  auto [ status, data ] = bridge_->readResponse(); checkCAENETexception(status);
  LOG_DEBUG("Group " + to_string(id_) + ": V0 set to " + to_string(v0));
}

void ChannelGroup::setV1(uint32_t v1) {
  for(auto channel : channels_) { channel->v1_ = v1; }
  // send command
  bridge_->write({0x1,address_,0x53,id_,v1});
  // read response
  auto [ status, data ] = bridge_->readResponse(); checkCAENETexception(status);
  LOG_DEBUG("Group " + to_string(id_) + ": V1 set to " + to_string(v1));
}

void ChannelGroup::setI0(uint32_t i0) {
  for(auto channel : channels_) { channel->i0_ = i0; }
  // send command
  bridge_->write({0x1,address_,0x55,id_,i0});
  // read response
  auto [ status, data ] = bridge_->readResponse(); checkCAENETexception(status);
  LOG_DEBUG("Group " + to_string(id_) + ": I1 set to " + to_string(i0));
}

void ChannelGroup::setI1(uint32_t i1) {
  for(auto channel : channels_) { channel->i1_ = i1; }
  // send command
  bridge_->write({0x1,address_,0x55,id_,i1});
  // read response
  auto [ status, data ] = bridge_->readResponse(); checkCAENETexception(status);
  LOG_DEBUG("Group " + to_string(id_) + ": I1 set to " + to_string(i1));
}

void ChannelGroup::setRampup(uint32_t rampup) {
  for(auto channel : channels_) { channel->rampup_ = rampup; }
  // send command
  bridge_->write({0x1,address_,0x57,id_,rampup});
  // read response
  auto [ status, data ] = bridge_->readResponse(); checkCAENETexception(status);
  LOG_DEBUG("Group " + to_string(id_) + ": Ramp UP set to " + to_string(rampup));
}

void ChannelGroup::setRampdown(uint32_t rampdown) {
  for(auto channel : channels_) { channel->rampdown_ = rampdown; }
  // send command
  bridge_->write({0x1,address_,0x58,id_,rampdown});
  // read response
  auto [ status, data ] = bridge_->readResponse(); checkCAENETexception(status);
  LOG_DEBUG("Group " + to_string(id_) + ": Ramp DOWN set to " + to_string(rampdown));
}

void ChannelGroup::setTrip(uint32_t trip) {
  for(auto channel : channels_) { channel->trip_ = trip; }
  // send command
  bridge_->write({0x1,address_,0x59,id_,trip});
  // read response
  auto [ status, data ] = bridge_->readResponse(); checkCAENETexception(status);
  LOG_DEBUG("Group " + to_string(id_) + ": TRIP set to " + to_string(trip));
}

void ChannelGroup::setSoftMaxV(uint32_t maxv) {
  for(auto channel : channels_) { channel->softmaxV_ = maxv; }
  // send command
  bridge_->write({0x1,address_,0x56,id_,maxv});
  // read response
  auto [ status, data ] = bridge_->readResponse(); checkCAENETexception(status);
  LOG_DEBUG("Group " + to_string(id_) + ": Soft MAXV set to " + to_string(maxv));
}

void ChannelGroup::on() {
  bridge_->write({0x1,address_,0x5A, id_});
  auto [ status, response ] = bridge_->readResponse(); checkCAENETexception(status);
  LOG_DEBUG("Group " + to_string(id_) + " turned ON.");
}

void ChannelGroup::off() {
  bridge_->write({0x1,address_,0x5B, id_});
  auto [ status, response ] = bridge_->readResponse(); checkCAENETexception(status);
  LOG_DEBUG("Group " + to_string(id_) + " turned OFF.");
}

void ChannelGroup::setGroupName() {
  std::vector<uint32_t> data = {0x1,address_,0x1B, id_};
  for(uint c = 0; c < name_.size(); c+=2) {
    data.push_back(name_[c]<<8|name_[c+1]);
  }
  if(!(name_.size()%2)) data.push_back(0);
  bridge_->write(data);
  auto [ status, response ] = bridge_->readResponse(); checkCAENETexception(status);
}

void ChannelGroup::setPriorityON(const value_type& channel, uint16_t priority) {
  assert(priority<=16);
  // find the element in the group
  ChannelGroup::const_iterator item = find(channel);
  if(item != end()) {
    // set the priority
    bridge_->write({0x1,address_,0x21, id_,(*item)->chAddress(),priority});
    auto [ status, response ] = bridge_->readResponse(); checkCAENETexception(status);
    (*item)->setPriorityON(priority);
  }
}

void ChannelGroup::setPriorityOFF(const value_type& channel, uint16_t priority) {
  assert(priority<=16);
  // find the element in the group
  ChannelGroup::const_iterator item = find(channel);
  if(item != end()) {
    // set the priority
    bridge_->write({0x1,address_,0x20, id_,(*item)->chAddress(),priority});
    auto [ status, response ] = bridge_->readResponse(); checkCAENETexception(status);
    (*item)->setPriorityOFF(priority);
  }
}

void ChannelGroup::setPriorityON(const key_type& channel, uint16_t priority) {
  assert(priority<=16);
  // find the element in the group
  ChannelGroup::const_iterator item = find(channel);
  if(item != end()) {
    // set the priority
    bridge_->write({0x1,address_,0x21, id_,(*item)->chAddress(),priority});
    auto [ status, response ] = bridge_->readResponse(); checkCAENETexception(status);
    (*item)->setPriorityON(priority);
  }
}

void ChannelGroup::setPriorityOFF(const key_type& channel, uint16_t priority) {
  assert(priority<=16);
  // find the element in the group
  ChannelGroup::const_iterator item = find(channel);
  if(item != end()) {
    // set the priority
    bridge_->write({0x1,address_,0x20, id_,(*item)->chAddress(),priority});
    auto [ status, response ] = bridge_->readResponse(); checkCAENETexception(status);
    (*item)->setPriorityOFF(priority);
  }
}


using namespace boost::python;

template<> void exposeToPython<SY527StatusWord>() {
  scope in_SY527StatusWord = class_<SY527StatusWord>("SY527StatusWord",init<uint32_t>())
   .def("status",&SY527StatusWord::status)
   .def("bit",&SY527StatusWord::bit)
  ;

  enum_<SY527StatusWord::CVStatusWordBit>("CVStatusWordBit")
    .value("cvPRESENT", SY527StatusWord::cvPRESENT)
    .value("cvINTRIP", SY527StatusWord::cvINTRIP)
    .value("cvKILL", SY527StatusWord::cvKILL)
    .value("cvMAXV", SY527StatusWord::cvMAXV)
    .value("cvEXTRIP", SY527StatusWord::cvEXTRIP)
    .value("cvOVV", SY527StatusWord::cvOVV)
    .value("cvUNV", SY527StatusWord::cvUNV)
    .value("cvOVC", SY527StatusWord::cvOVC)
    .value("cvRDW", SY527StatusWord::cvRDW)
    .value("cvRUP", SY527StatusWord::cvRUP)
    .value("cvONOFF", SY527StatusWord::cvONOFF)
    .value("cvEXTRIPEN", SY527StatusWord::cvEXTRIPEN)
    .value("cvPWDREQ", SY527StatusWord::cvPWDREQ)
    .value("cvPOWDOWN", SY527StatusWord::cvPOWDOWN)
    .value("cvOOEN", SY527StatusWord::cvOOEN)
    .value("cvPOWON", SY527StatusWord::cvPOWON)
    .value("cvABSORBS", SY527StatusWord::cvABSORBS)
    .value("cvEXDIS", SY527StatusWord::cvEXDIS)
    .value("cvPOWER", SY527StatusWord::cvPOWER)
  ;
}

template<> void exposeToPython<SY527HVChannel>() {
  class_<SY527HVChannel, bases<HVChannel> >("SY527HVChannel",init<uint32_t, HVBoard&, uint32_t, uint8_t, CaenetBridge*>())
    .def("setPasswordFlag",&SY527HVChannel::setPasswordFlag)
    .def("setOnOffFlag",&SY527HVChannel::setOnOffFlag)
    .def("setPoweronFlag",&SY527HVChannel::setPoweronFlag)
    .add_property("name",&ChannelGroup::getName, &ChannelGroup::setName)
    .def("getStatus",&SY527HVChannel::getStatus)
  ;
}

template<> void exposeToPython<SY527PowerSystem>() {
  scope in_SY527PowerSystem = class_<SY527PowerSystem, bases<HVModule> >("SY527PowerSystem",init<uint32_t,CaenetBridge*>())
    .def("updateStatus",&SY527PowerSystem::updateStatus)
    .def("getHWStatus",&SY527PowerSystem::getHWStatus)
    .def("selfTest",&SY527PowerSystem::selfTest)
    .def("getGroup",&SY527PowerSystem::getGroup)
    .def("getGeneralStatus",&SY527PowerSystem::getGeneralStatus)
    .def("programAlarms",&SY527PowerSystem::programAlarms)
    .def("formatEEPROM",&SY527PowerSystem::formatEEPROM)
    .def("clearAlarm",&SY527PowerSystem::clearAlarm)
    .def("lockKeyboard",&SY527PowerSystem::lockKeyboard)
    .def("killAll",&SY527PowerSystem::killAll)
    .def("checksum",&SY527PowerSystem::checksum)
  ;
  enum_<SY527PowerSystem::Checksum>("Checksum")
    .value("correctChecksum", SY527PowerSystem::correctChecksum)
    .value("wrongHdrChecksum", SY527PowerSystem::wrongHdrChecksum)
    .value("wrongFwChecksum", SY527PowerSystem::wrongFwChecksum)
    .value("wrongChecksum", SY527PowerSystem::wrongChecksum)
    .value("boardAbsent", SY527PowerSystem::boardAbsent)
  ;
  class_<std::vector<SY527PowerSystem::Checksum> >("vec_checksum")
    .def(vector_indexing_suite<std::vector<SY527PowerSystem::Checksum> >())
    .def("__iter__", boost::python::iterator<std::vector<SY527PowerSystem::Checksum> >())
  ;
 
}

template<> void exposeToPython<ChannelGroup>() {
   
  ChannelGroup::iterator (ChannelGroup::*find1)(const ChannelGroup::value_type&) = &ChannelGroup::find;
  ChannelGroup::const_iterator (ChannelGroup::*find2)(const ChannelGroup::value_type&) const = &ChannelGroup::find;
  ChannelGroup::iterator (ChannelGroup::*find3)(const ChannelGroup::key_type&) = &ChannelGroup::find;
  ChannelGroup::const_iterator (ChannelGroup::*find4)(const ChannelGroup::key_type&) const = &ChannelGroup::find;
  ChannelGroup::reference (ChannelGroup::*at1)(ChannelGroup::size_type) = &ChannelGroup::at;
  ChannelGroup::const_reference (ChannelGroup::*at2)(ChannelGroup::size_type) const = &ChannelGroup::at;
  ChannelGroup::size_type (ChannelGroup::*count1)(const ChannelGroup::value_type&) const = &ChannelGroup::count;
  ChannelGroup::size_type (ChannelGroup::*count2)(const ChannelGroup::key_type&) const = &ChannelGroup::count;
  ChannelGroup::iterator (ChannelGroup::*erase1)(ChannelGroup::const_iterator) = &ChannelGroup::erase;
  ChannelGroup::size_type (ChannelGroup::*erase2)(const ChannelGroup::value_type&) = &ChannelGroup::erase;
  ChannelGroup::size_type (ChannelGroup::*erase3)(const ChannelGroup::key_type&) = &ChannelGroup::erase;
  ChannelGroup::iterator (ChannelGroup::*erase4)(ChannelGroup::const_iterator,ChannelGroup::const_iterator) = &ChannelGroup::erase;
  void (ChannelGroup::*pron1)(const ChannelGroup::value_type& ,uint16_t) =  &ChannelGroup::setPriorityON;
  void (ChannelGroup::*pron2)(const ChannelGroup::key_type& ,uint16_t) =  &ChannelGroup::setPriorityON;
  void (ChannelGroup::*proff1)(const ChannelGroup::value_type& ,uint16_t) =  &ChannelGroup::setPriorityOFF;
  void (ChannelGroup::*proff2)(const ChannelGroup::key_type& ,uint16_t) =  &ChannelGroup::setPriorityOFF;
  
  class_<ChannelGroup>("ChannelGroup",no_init)
    .add_property("name",&ChannelGroup::getName, &ChannelGroup::setName)
    .def("readParameters",&ChannelGroup::readParameters)
    .def("readSettings",&ChannelGroup::readSettings)
    .def("setV0",&ChannelGroup::setV0)
    .def("setV1",&ChannelGroup::setV1)
    .def("setI0",&ChannelGroup::setI0)
    .def("setI1",&ChannelGroup::setI1)
    .def("setRampup",&ChannelGroup::setRampup)
    .def("setRampdown",&ChannelGroup::setRampdown)
    .def("setTrip",&ChannelGroup::setTrip)
    .def("setSoftMaxV",&ChannelGroup::setSoftMaxV)
    .def("on",&ChannelGroup::on)
    .def("off",&ChannelGroup::off)
    .def("__len__",&ChannelGroup::size)
    .def("find",find1)
    .def("find",find2)
    .def("find",find3)
    .def("find",find4)
    .def("count",count1)
    .def("count",count2)
    .def("__getitem__",at1,return_value_policy<copy_non_const_reference>())
    .def("__getitem__",at2,return_value_policy<copy_const_reference>())
    .def("insert",&ChannelGroup::insert)
    .def("insert",&ChannelGroup::insert)
    .def("erase",erase1)
    .def("erase",erase2)
    .def("erase",erase3)
    .def("erase",erase4)
    .def("setPriorityON",pron1)
    .def("setPriorityON",pron2)
    .def("setPriorityOFF",proff1)
    .def("setPriorityOFF",proff2)
  ;

}
