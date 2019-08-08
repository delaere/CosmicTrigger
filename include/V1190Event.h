#ifndef __V1190EVENT_H
#define __V1190EVENT_H

#include <iostream>
#include <vector>

class TDCHit
{
public:
  TDCHit(uint32_t data = 0) {
    type_ = (data>>26)&0x1;
    channel_ = (data>>19)&0x7F;
    measurement_ = data&0x7FFFF;
  }
  ~TDCHit() {}
  
  inline uint16_t getChannel() const { return channel_; }
  inline uint8_t  getType() const { return type_; }
  inline uint32_t getMeasurement() const { return measurement_; }
  
  inline void setChannel(uint16_t channel) { channel_ = channel&0x7F; }
  inline void setType(uint8_t type) { type_ = type&0x1; }
  inline void setMeasurement(uint32_t measurement) { measurement_ = measurement&0x7FFFF; }
  
  operator uint32_t() const { return (type_<<26) + (channel_<<19) + measurement_; }
  
  std::string toString() const;
  
private:
  uint32_t type_:1, channel_:7, measurement_:19; //TDC measurement
};

class TDCEvent
{
public:
  TDCEvent(uint32_t header = 0,uint16_t errorflags = 0):errorflags_(errorflags) {
    tdcid_ = (header>>24)&0x3;
    eventid_ = (header>>12)&0xFFF;
    bunchid_ = header&0xFFF;
  }
  ~TDCEvent() {}
  
  inline uint8_t getTDCId() const { return tdcid_; }
  inline uint16_t getEventId() const { return eventid_; }
  inline uint16_t getBunchId() const { return bunchid_; }
  inline uint16_t getErrorFlags() const { return errorflags_; }
  inline const std::vector<TDCHit>& getHits() const { return hits_; }
  
  inline void setTDCId(uint8_t id) { tdcid_ = id&0x3; }
  inline void setEventId(uint16_t id) { eventid_ = id&0xFFF; }
  inline void setBunchId(uint16_t id) { bunchid_ = id&0xFFF; }
  inline void setErrorFlags(uint16_t flags) { errorflags_ = flags; }
  
  inline void addHit(TDCHit hit) { hits_.push_back(hit); }
  
  std::string toString() const;
    
private:
  uint32_t tdcid_:2,eventid_:12, bunchid_:12; // TDC header word
  uint16_t errorflags_; // TDC error word
  std::vector<TDCHit> hits_;
  
};

class V1190Event
{
public:
  V1190Event(uint32_t header = 0, uint32_t trailer = 0, uint32_t extdTime = 0):extTriggerTime_(extdTime) {
    eventCount_ = (header>>5)&0x7FFFFF;
    geo_ = header&0x1F;
    status_ = (trailer>>24)&0x7;
  }
  ~V1190Event() {}
  
  inline uint32_t getEventCount() const { return eventCount_; }
  inline uint8_t getGeo() const { return geo_; }
  inline uint8_t getStatus() const { return status_; }
  inline uint32_t getExtdTriggerTime() const { return extTriggerTime_; }
  inline const std::vector<TDCHit>& getHits() const { return hits_; }
  inline const std::vector<TDCEvent>& getTDCEvents() const { return tdcevents_; }
  
  inline void setEventCount(uint32_t count) { eventCount_ = count&0x7FFFFF; }
  inline void setGeo(uint8_t geo) { geo_ = geo&0x1F; }
  inline void setStatus(uint8_t status) { status_ = status&0x7; }
  inline void setExtdTriggerTime(uint32_t time) { extTriggerTime_ = time&0x7FFFFFF; }
  
  inline void addHit(TDCHit hit) { hits_.push_back(hit); }
  inline void addTDCEvent(TDCEvent &tdc) { tdcevents_.push_back(tdc); }
  
  std::string toString() const;
  
private:
  uint32_t eventCount_:22, geo_:5, status_:3; // header & trailer
  uint32_t extTriggerTime_; // extended time (if enabled)
  std::vector<TDCHit> hits_; // hits (if TDC headers&trailers disabled)
  std::vector<TDCEvent> tdcevents_; // tdc events (if enabled)
};

std::ostream & operator << (std::ostream &out, const TDCHit &c);

std::ostream & operator << (std::ostream &out, const TDCEvent &c);

std::ostream & operator << (std::ostream &out, const V1190Event &c);

#endif // __V1190EVENT_H
