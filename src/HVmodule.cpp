#include "HVmodule.h"

#include "CaenetBridge.h"

#include "N470HVmodule.h"
#include "SY527PowerSystem.h"

HVBoard::HVBoard(uint32_t slot, std::string name, uint8_t current_unit, uint32_t serial_number,
                 uint32_t software_version, uint32_t nChannels, uint32_t vmax, uint32_t imax,
                 uint32_t rampmin, uint32_t rampmax, uint32_t vres, uint32_t ires, uint32_t vdec, uint32_t idec):
                 slot_(slot),name_(name),current_unit_(current_unit),serial_number_(serial_number),
                 software_version_(software_version),nChannels_(nChannels),vmax_(vmax),imax_(imax),
                 rampmin_(rampmin),rampmax_(rampmax),vres_(vres),ires_(ires),vdec_(vdec),idec_(idec) {}
            
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
  bridge_->write({0x0,address_,0});
  auto [ status, data ] = bridge_->readResponse(); checkCAENETexception(status);
  identification_=std::string(data.begin(),data.end());

  // the derived class should do the following
  /*
  // check that the idendity is as expected in the derived class
  assertIdentity();

  // discover boards
  discoverBoards();

  // instantiate the channels
  for( auto const& [slot, board] : boards_ ) {
    for(int i=0;i<board.getNChannels();i++) {
      channels_[std::pair(board.getSlot(),i)] = HVChannel(address,board,i,bridge); 
    }
  }
  */
}

HVModule* HVModule::HVModuleFactory(uint32_t address, CaenetBridge* bridge) {
  // get the identification string
  bridge->write({0x0,address,0});
  auto [ status, data ] = bridge->readResponse(); checkCAENETexception(status);
  auto id = std::string(data.begin(),data.end());
  
  // instantiate the proper class
  if(id.find("N470") != std::string::npos) {
    return new N470HVModule(address,bridge);
  } else if(id.find("SY527") != std::string::npos) {
    return new SY527PowerSystem(address,bridge);
  }
  
  // fall back
  return nullptr;
}
