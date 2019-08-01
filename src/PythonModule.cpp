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

#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include "VmeController.h"
#include "VmeUsbBridge.h"
#include "VmeBoard.h"
#include "CaenetBridge.h"
#include "HVmodule.h"
#include "N470HVmodule.h"
#include "SY527PowerSystem.h"
#define __PYTHONMODULECONFIGURATION

using namespace boost::python;

#include "PythonModule.h"

BOOST_PYTHON_MODULE(VeheMencE)
{
    //expose the CAENVMEtypes enums
#include "PythonCaenVmeTypes.h"

    //expose VmeController
    class_<VmeControllerWrap, boost::noncopyable>("VmeController")
        .def("setMode", &VmeController::setMode)
        .def("mode", &VmeController::mode,return_value_policy<manage_new_object>())
        .add_property("AM",&VmeController::getAM,&VmeController::setAM)
        .add_property("DW",&VmeController::getDW,&VmeController::setDW)
        .def("writeData",pure_virtual(&VmeController::writeData)) 
        .def("readData",pure_virtual(&VmeController::readData))
        .def("readWriteData",pure_virtual(&VmeController::readWriteData))
        .def("blockReadData",pure_virtual(&VmeController::blockReadData))
        .def("blockWriteData",pure_virtual(&VmeController::blockWriteData))
        .def("ADOCycle",pure_virtual(&VmeController::ADOCycle))
        .def("IRQEnable",pure_virtual(&VmeController::IRQEnable))
        .def("IRQDisable",pure_virtual(&VmeController::IRQDisable))
        .def("IRQWait",pure_virtual(&VmeController::IRQWait))
        .def("IRQCheck",pure_virtual(&VmeController::IRQCheck))
        .def("IACK",pure_virtual(&VmeController::IACK))
    ;
    
    def("vmeRead",&vmeRead);
    def("vmeWrite",&vmeWrite);
    //expose VmeUsbBridge
    class_<VmeUsbBridge, bases<VmeController> >("VmeUsbBridge")
         .def("configureOutputLine",&VmeUsbBridge::configureOutputLine)
         .def("configureInputLine",&VmeUsbBridge::configureInputLine)
         .def("outputLineConfiguration",&VmeUsbBridge::outputLineConfiguration)
         .def("inputLineConfiguration",&VmeUsbBridge::inputLineConfiguration)
         .def("readRegister",&VmeUsbBridge::readRegister)
         .def("setOutputRegister",&VmeUsbBridge::setOutputRegister)
         .def("clearOutputRegister",&VmeUsbBridge::clearOutputRegister)
         .def("pulseOutputRegister",&VmeUsbBridge::pulseOutputRegister)
         .def("readDisplay",&VmeUsbBridge::readDisplay)
         .add_property("arbiterType",&VmeUsbBridge::getArbiterType,&VmeUsbBridge::setArbiterType)
         .add_property("requesterType",&VmeUsbBridge::getRequesterType,&VmeUsbBridge::setRequesterType)
         .add_property("releaseType",&VmeUsbBridge::getReleaseType,&VmeUsbBridge::setReleaseType)
         .add_property("busReqLevel",&VmeUsbBridge::getBusReqLevel,&VmeUsbBridge::setBusReqLevel)
         .add_property("timeout",&VmeUsbBridge::getTimeout,&VmeUsbBridge::setTimeout)
         .add_property("FIFOmode",&VmeUsbBridge::getFIFOMode,&VmeUsbBridge::setFIFOMode)
         .def("systemReset",&VmeUsbBridge::systemReset)
         .def("getPulser",&VmeUsbBridge::getPulser,return_value_policy<copy_non_const_reference>())
         .def("getScaler",&VmeUsbBridge::getScaler,return_value_policy<copy_non_const_reference>())
    ;
    class_<V1718Pulser>("V1718Pulser",init<uint32_t,CVPulserSelect>())
         .def("configure",&V1718Pulser::configure)
         .def("update",&V1718Pulser::configure)
         .def("start",&V1718Pulser::start)
         .def("stop",&V1718Pulser::stop)
         .add_property("period",&V1718Pulser::getPeriod,&V1718Pulser::setPeriod)
         .add_property("width",&V1718Pulser::getWidth,&V1718Pulser::setWidth)
         .add_property("units",&V1718Pulser::getUnits,&V1718Pulser::setUnits)
         .add_property("pulseNo",&V1718Pulser::getPulseNo,&V1718Pulser::setPulseNo)
         .add_property("startSource",&V1718Pulser::getStartSource,&V1718Pulser::setStartSource)
         .add_property("stopSource",&V1718Pulser::getStopSource,&V1718Pulser::setStopSource)
    ;
    class_<V1718Scaler>("V1718Scaler",init<uint32_t>())
         .def("configure",&V1718Scaler::configure)
         .def("update",&V1718Scaler::update)
         .def("resetCount",&V1718Scaler::resetCount)
         .def("enableGate",&V1718Scaler::enableGate)
         .def("disableGate",&V1718Scaler::disableGate)
         .def("count",&V1718Scaler::count)
         .add_property("limit",&V1718Scaler::getLimit,&V1718Scaler::setLimit)
         .add_property("autoReset",&V1718Scaler::getAutoReset,&V1718Scaler::setAutoReset)
         .add_property("hitSource",&V1718Scaler::getHitSource,&V1718Scaler::setHitSource)
         .add_property("gateSource",&V1718Scaler::getGateSource,&V1718Scaler::setGateSource)
         .add_property("resetSource",&V1718Scaler::getResetSource,&V1718Scaler::setResetSource)
    ;
    // expose VmeBoard
    class_<VmeBoard>("VmeBoard",init<VmeController*, uint32_t, CVAddressModifier, CVDataWidth, bool>())
         .add_property("enforce",&VmeBoard::isAMDWenforced,&VmeBoard::enforceAMDW)
    ;
    // expose CaenetBridge
    class_<CaenetBridge, bases<VmeBoard> >("CaenetBridge",init<VmeController*, uint32_t, uint8_t>())
         .def("reset",&CaenetBridge::reset)
         .def("validStatus",&CaenetBridge::validStatus)
         .def("write",&CaenetBridge::write)
         .def("readResponse",&CaenetBridge::readResponse)
    ;
    class_<std::vector<uint32_t> >("vec_int")
         .def(vector_indexing_suite<std::vector<uint32_t> >())
         .def("__iter__", iterator<std::vector<uint32_t> >())
    ;
    class_<std::pair<uint32_t, std::vector<uint32_t> > >("CaenetResponse")
         .def_readwrite("errorCode", &std::pair<uint32_t, std::vector<uint32_t> >::first)
         .def_readwrite("data",&std::pair<uint32_t, std::vector<uint32_t> >::second)
    ;
    // expose HVmodule
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
    class_<HVModuleWrap, boost::noncopyable>("HVModule", init<uint32_t, CaenetBridge*>())
         .add_property("identification",&HVModule::identification)
         .def("HVModuleFactory",&HVModule::HVModuleFactory,return_value_policy<manage_new_object>())
         .staticmethod("HVModuleFactory") 
         .def("channel",&HVModule::channel,return_value_policy<reference_existing_object>())
         .def("board",&HVModule::board,return_value_policy<copy_non_const_reference>())
         .def("getChannels",&HVModule::getChannels,return_value_policy<copy_non_const_reference>())
         .def("getBoards",&HVModule::getBoards,return_value_policy<copy_non_const_reference>())
    ;
    // expose N470HVmodule
    {
      scope in_N470StatusWord = class_<N470StatusWord>("N470StatusWord",init<uint16_t>())
           .def("status",&N470StatusWord::status)
           .def("bit",&N470StatusWord::bit)
      ;

      enum_<N470StatusWord::CVStatusWordBit>("CVStatusWordBit")
          .value("cvONOFF", N470StatusWord::cvONOFF)
          .value("cvOVC", N470StatusWord::cvOVC)
          .value("cvOVV", N470StatusWord::cvOVV)
          .value("cvUNV", N470StatusWord::cvUNV)
          .value("cvTRIP", N470StatusWord::cvTRIP)
          .value("cvRUP", N470StatusWord::cvRUP)
          .value("cvRDW", N470StatusWord::cvRDW)
          .value("cvMAXV", N470StatusWord::cvMAXV)
          .value("cvPOL", N470StatusWord::cvPOL)
          .value("cvVSEL", N470StatusWord::cvVSEL)
          .value("cvISEL", N470StatusWord::cvISEL)
          .value("cvKILL", N470StatusWord::cvKILL)
          .value("cvHVEN", N470StatusWord::cvHVEN)
          .value("cvNIMTTL", N470StatusWord::cvNIMTTL)
          .value("cvOUTCAL", N470StatusWord::cvOUTCAL)
          .value("cvALARM", N470StatusWord::cvALARM)
      ;
    }
       
    class_<N470HVChannel>("N470HVChannel",init<uint32_t,HVBoard&,uint32_t,CaenetBridge*>())
         .def("getStatus",&N470HVChannel::getStatus)
    ;
    class_<N470HVModule>("N470HVModule",init<uint32_t,CaenetBridge*>())
         .add_property("status",&N470HVModule::getStatus)
         .def("updateStatus",&N470HVModule::updateStatus)
         .def("kill",&N470HVModule::kill)
         .def("clearAlarm",&N470HVModule::clearAlarm)
         .def("enableKeyboard",&N470HVModule::enableKeyboard)
         .def("setTTL",&N470HVModule::setTTL)
         .def("setNIM",&N470HVModule::setNIM)
    ;
    // expose SY527PowerSystem
    {
      scope in_SY527StatusWord = class_<SY527StatusWord>("SY527StatusWord",init<uint16_t>())
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
      ;
    }
    class_<SY527HVChannel, bases<HVChannel> >("SY527HVChannel",init<uint32_t, HVBoard&, uint32_t, CaenetBridge*>())
         .def("setPasswordFlag",&SY527HVChannel::setPasswordFlag)
         .def("setOnOffFlag",&SY527HVChannel::setOnOffFlag)
         .def("setPoweronFlag",&SY527HVChannel::setPoweronFlag)
         .def("getName",&SY527HVChannel::getName)
         .def("getStatus",&SY527HVChannel::getStatus)
    ;
    class_<SY527PowerSystem, bases<HVModule> >("SY527PowerSystem",init<uint32_t,CaenetBridge*>())
         .def("updateStatus",&SY527PowerSystem::updateStatus)
    ;
}
