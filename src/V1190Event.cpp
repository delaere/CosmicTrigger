#include "V1190Event.h"
#include <sstream>
using namespace std;

ostream & operator << (ostream &out, const TDCHit &c)
{
    out << c.toString();
    return out;
}

ostream & operator << (ostream &out, const TDCEvent &c)
{
    out << c.toString();
    return out;
}

ostream & operator << (ostream &out, const V1190Event &c)
{
    out << c.toString();
    return out;
}

bool operator==(const TDCEvent& lhs, const TDCEvent& rhs)
{
  return (lhs.tdcid_ == rhs.tdcid_) && (lhs.eventid_==rhs.eventid_) && (lhs.bunchid_ == rhs.bunchid_) && (lhs.errorflags_ == rhs.errorflags_) && (lhs.hits_ == rhs.hits_);
}

bool operator==(const V1190Event& lhs, const V1190Event& rhs)
{
  return (lhs.eventCount_ == rhs.eventCount_) && (lhs.geo_==rhs.geo_) && (lhs.status_ == rhs.status_) && (lhs.extTriggerTime_ == rhs.extTriggerTime_) && (lhs.hits_ == rhs.hits_) && (lhs.tdcevents_ == rhs.tdcevents_);
}

std::string TDCHit::toString() const {
  std::stringstream  output;
  if(getType()) {
    output << "Leading edge measurement on channel " << std::dec << getChannel();
    output << " : " << getMeasurement();
    output << " ( width = " << (getMeasurement()>>12);
    output << ", leading time = " << (getMeasurement()&0xFFF) << " for pair measurement )" << std::endl;
  } else {
    output << "Trailing edge measurement on channel " << std::dec << getChannel();
    output << " : " << getMeasurement() << std::endl;
  }
  return output.str();
}

std::string TDCEvent::toString() const {
  std::stringstream output;
  output << "  TDC " << std::dec << tdcid_ << " Event number: " << eventid_ ;
  output << ", bunch id: " << bunchid_ << " :" << std::endl;
  output << "  Errors: " << std::showbase << std::hex << errorflags_ << std::endl;
  if(hits_.size()) {
    output << "  Recorded measurements :" << std::endl; 
    for (auto measurement : hits_) {
      output << "    " << measurement << std::endl;
    }
  }
  return output.str();
}

std::string V1190Event::toString() const {
  std::stringstream output;
  output << "Event number : " << eventCount_ ;
  output << " (GEO " << std::showbase << std::hex << geo_ << ", status " << status_ << ")" << std::endl;
  if(extTriggerTime_) output << "Extended Trigger Time: " << std::dec << extTriggerTime_ << std::endl;
  if(hits_.size()) {
    output << "Recorded measurements :" << std::endl; 
    for (auto measurement : hits_) {
      output << "  " << measurement << std::endl;
    }
  } else if (tdcevents_.size()) {
    output << "Recorded TDC events :" << std::endl; 
    for (auto measurement : tdcevents_) {
      output << measurement << std::endl;
    }
  }
  return output.str();
}
