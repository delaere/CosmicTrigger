#include "CaenetBridge.h"

CaenetBridge::CaenetBridge(vmeController *cont, uint32_t bridgeAdd, uint8_t interrupt):vmeBoard(cont,bridgeAdd,cvA24_U_DATA,cvD16,true),interrupt_(interrupt) {}

void CaenetBridge::reset() {
  uint16_t data = 0x0000;
  writeData(baseAddress()+0x6,&data);
}

bool CaenetBridge::validStatus() {
  uint16_t data = 0x0000;
  readData(baseAddress()+0x2,&data);
  return !(data&0x1);
}

void CaenetBridge::write(const std::vector<uint32_t>& data) {
  // buffers data
  for(auto d : data) {
    writeData(baseAddress(),&d);
    if (!validStatus()) throw CAENETexception(0XFFF0);
  }
  // start transmission
  uint32_t tmp = 0x0000;
  writeData(baseAddress()+0x4,&tmp);
  if (!validStatus()) throw CAENETexception(0XFFF0);
}

/*
uint32_t CaenetBridge::readResponse(std::vector<uint32_t>& data) {
  uint32_t errorCode = 0;
  uint32_t tmp = 0;
  // first wait for the response
  if (interrupt_) { // wait for interrupt if possible
    controller()->IRQWait(1<<(interrupt_-1),500);
    controller()->IRQDisable(1<<(interrupt_-1));
  }
  // polling. First word read is the error code (0 for success)
  do {
    readData(baseAddress(),&errorCode);
  } while (!validStatus());
  // then read data
  while (validStatus()) {
    readData(baseAddress(),&tmp);
    data.push_back(tmp);
  }
  // last word was garbage
  data.pop_back();
  return errorCode;
}
*/
std::tuple<uint32_t, std::vector<uint32_t> > CaenetBridge::readResponse() {
  uint32_t errorCode = 0;
  std::vector<uint32_t> data;
  uint32_t tmp = 0;
  // first wait for the response
  if (interrupt_) { // wait for interrupt if possible
    controller()->IRQWait(1<<(interrupt_-1),500);
    controller()->IRQDisable(1<<(interrupt_-1));
  }
  // polling. First word read is the error code (0 for success)
  do {
    readData(baseAddress(),&errorCode);
  } while (!validStatus());
  // then read data
  while (validStatus()) {
    readData(baseAddress(),&tmp);
    data.push_back(tmp);
  }
  // last word was garbage
  data.pop_back();
  return make_tuple(errorCode,data);
}
