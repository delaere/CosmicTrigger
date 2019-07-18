#include "N470HVmodule.h"
#include "CaenetBridge.h"

//TODO check what is returned by the methods "set". If this is the status, it can be saved in status_

N470HVChannel::N470HVChannel(uint32_t address, HVBoard& board, uint32_t id, CaenetBridge* bridge):HVChannel(address,board,id,bridge),status_(0) {}

void N470HVChannel::on() {
  // send command
  bridge_->write({0x0,address_,(id_<<8)+0xA});
  // read response
  auto [ status, data ] = bridge_->readResponse(); checkCAENETexception(status);
  // system status is ignored
}

void N470HVChannel::off() {
  // send command
  bridge_->write( {0x0,address_,(id_<<8)+0xB});
  // read response
  auto [ status, data ] = bridge_->readResponse(); checkCAENETexception(status);
  // system status is ignored
}

void N470HVChannel::setV0(uint32_t v0) {
  v0_ = v0;
  // send command
  bridge_->write({0x0,address_,(id_<<8)+0x3,v0});
  // read response
  auto [ status, data ] = bridge_->readResponse(); checkCAENETexception(status);
  // data is ignored
}

void N470HVChannel::setV1(uint32_t v1) {
  v1_ = v1;
  // send command
  bridge_->write({0x0,address_,(id_<<8)+0x5,v1});
  // read response
  auto [ status, data ] = bridge_->readResponse(); checkCAENETexception(status);
  // data is ignored
}

void N470HVChannel::setI0(uint32_t i0) {
  i0_ = i0;
  // send command
  bridge_->write({0x0,address_,(id_<<8)+0x4,i0});
  // read response
  auto [ status, data ] = bridge_->readResponse(); checkCAENETexception(status);
  // data is ignored
}

void N470HVChannel::setI1(uint32_t i1) {
  i1_ = i1;
  // send command
  bridge_->write({0x0,address_,(id_<<8)+0x6,i1});
  // read response
  auto [ status, data ] = bridge_->readResponse(); checkCAENETexception(status);
  // data is ignored
}

void N470HVChannel::setRampup(uint32_t rampup) {
  rampup_ = rampup;
  // send command
  bridge_->write({0x0,address_,(id_<<8)+0x8,rampup});
  // read response
  auto [ status, data ] = bridge_->readResponse(); checkCAENETexception(status);
  // data is ignored
}

void N470HVChannel::setRampdown(uint32_t rampdown) {
  rampdown_ = rampdown;
  // send command
  bridge_->write({0x0,address_,(id_<<8)+0x9,rampdown});
  // read response
  auto [ status, data ] = bridge_->readResponse(); checkCAENETexception(status);
  // data is ignored
}

void N470HVChannel::setTrip(uint32_t trip) {
  trip_ = trip;
  // send command
  bridge_->write({0x0,address_,(id_<<8)+0x7,trip});
  // read response
  auto [ status, data ] = bridge_->readResponse(); checkCAENETexception(status);
  // data is ignored
}

void N470HVChannel::setSoftMaxV(uint32_t) {
  throw_with_trace(CAENETexception(0xFF01));
}

void N470HVChannel::readOperationalParameters() {
  // send command
  bridge_->write({0x0,address_,(id_<<8)+0x2});
  // read response
  auto [ status, data ] = bridge_->readResponse(); checkCAENETexception(status);
  std::tie(status_, vmon_, imon_, v0_, i0_, v1_, i1_, trip_, rampup_, rampdown_, maxV_) = gen_tuple<11>(data);
}

void N470HVChannel::setStatus(std::vector<uint32_t>::const_iterator data) {
  vmon_   = *data++;
  imon_   = *data++;
  maxV_   = *data++;
  status_ = *data++;
}

N470HVModule::N470HVModule(uint32_t address, CaenetBridge* bridge):HVModule(address,bridge),status_(0) { 
  // check that the idendity is as expected in the derived class
  assertIdentity();

  // discover boards
  discoverBoards();

  // instantiate the channels
  for( auto & [slot, board] : getBoards() ) {
    for(int i=0;i<board.getNChannels();i++) {
      getChannels()[std::pair(board.getSlot(),i)] = new N470HVChannel(address,board,i,bridge); 
    }
  }
}

void N470HVModule::discoverBoards() {
  auto boards = getBoards();
  // only one board in that module. Cannot be dynamically discovered.
  boards[0] = HVBoard(0,"N470", 2, 0, 0, 4, 8000, 3000, 0, 500, 500, 1000, 0, 0);
}
  
void N470HVModule::assertIdentity() const {
  // this should be the start of the string.
  assert(identification().find("N 470") != std::string::npos);
}

void N470HVModule::updateStatus() {
  // send command
  bridge_->write({0x0,address_,1});
  // read response
  auto [ status, data ] = bridge_->readResponse(); checkCAENETexception(status);
  for(auto [key,channel] : getChannels()) {
    static_cast<N470HVChannel*>(channel)->setStatus(data.begin()+(channel->id()*4));
  }
}

void N470HVModule::kill() {
  // send command
  bridge_->write({0x0,address_,12});
  // read response
  auto [ status, data ] = bridge_->readResponse(); checkCAENETexception(status);
  //TODO handle data
}

void N470HVModule::clearAlarm() {
  // send command
  bridge_->write({0x0,address_,13});
  // read response
  auto [ status, data ] = bridge_->readResponse(); checkCAENETexception(status);
  //TODO handle data
}

void N470HVModule::enableKeyboard(bool enable) {
  uint8_t opcode = enable ? 14 : 15;
  // send command
  bridge_->write({0x0,address_,opcode});
  // read response
  auto [ status, data ] = bridge_->readResponse(); checkCAENETexception(status);
  //TODO handle data
}

void N470HVModule::setTTL() {
  // send command
  bridge_->write({0x0,address_,16});
  // read response
  auto [ status, data ] = bridge_->readResponse(); checkCAENETexception(status);
  //TODO handle data
}

void N470HVModule::setNIM() {
  // send command
  bridge_->write({0x0,address_,17});
  // read response
  auto [ status, data ] = bridge_->readResponse(); checkCAENETexception(status);
  //TODO handle data
}
