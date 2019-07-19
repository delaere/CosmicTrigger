#ifdef __PYTHONMODULECONFIGURATION

struct VmeControllerWrap : VmeController, wrapper<VmeController>
{
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

struct HVChannelWrap : HVChannel, wrapper<HVChannel>
{
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

struct HVModuleWrap : HVModule, wrapper<HVModule>
{
  HVModuleWrap(uint32_t address, CaenetBridge* bridge):HVModule(address, bridge), wrapper<HVModule>() {}
  
  void discoverBoards() override {
    this->get_override("discoverBoards")();
  }
  
  void assertIdentity() const override {
    this->get_override("assertIdentity")();
  }
};

#endif
