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

#ifdef __PYTHONMODULECONFIGURATION

enum_<CVDataWidth>("CVDataWidth")
  .value("cvD8", cvD8)
  .value("cvD16", cvD16)
  .value("cvD32", cvD32)
  .value("cvD64", cvD64)
  .value("cvD16_swapped", cvD16_swapped)
  .value("cvD32_swapped", cvD32_swapped)
  .value("cvD64_swapped", cvD64_swapped)
  ;
  
enum_<CVAddressModifier>("CVAddressModifier")
  .value("cvA16_S", cvA16_S)
  .value("cvA16_U", cvA16_U)
  .value("cvA16_LCK", cvA16_LCK)
  .value("cvA24_S_BLT", cvA24_S_BLT)
  .value("cvA24_S_PGM", cvA24_S_PGM)
  .value("cvA24_S_DATA", cvA24_S_DATA)
  .value("cvA24_S_MBLT", cvA24_S_MBLT)
  .value("cvA24_U_BLT", cvA24_U_BLT)
  .value("cvA24_U_PGM", cvA24_U_PGM)
  .value("cvA24_U_DATA", cvA24_U_DATA)
  .value("cvA24_U_MBLT", cvA24_U_MBLT)
  .value("cvA24_LCK", cvA24_LCK)
  .value("cvA32_S_BLT", cvA32_S_BLT)
  .value("cvA32_S_PGM", cvA32_S_PGM)
  .value("cvA32_S_DATA", cvA32_S_DATA)
  .value("cvA32_S_MBLT", cvA32_S_MBLT)
  .value("cvA32_U_BLT", cvA32_U_BLT)
  .value("cvA32_U_PGM", cvA32_U_PGM)
  .value("cvA32_U_DATA", cvA32_U_DATA)
  .value("cvA32_U_MBLT", cvA32_U_MBLT)
  .value("cvA32_LCK", cvA32_LCK)
  .value("cvCR_CSR", cvCR_CSR)
  .value("cvA40_BLT", cvA40_BLT)
  .value("cvA40_LCK", cvA40_LCK)
  .value("cvA40", cvA40)
  .value("cvA64", cvA64)
  .value("cvA64_BLT", cvA64_BLT)
  .value("cvA64_MBLT", cvA64_MBLT)
  .value("cvA64_LCK", cvA64_LCK)
  .value("cvA3U_2eVME", cvA3U_2eVME)
  .value("cvA6U_2eVME", cvA6U_2eVME)
  ;

enum_<CVPulserSelect>("CVPulserSelect")
  .value("cvPulserA", cvPulserA)
  .value("cvPulserB", cvPulserB)
  ;

enum_<CVOutputSelect>("CVOutputSelect")
  .value("cvOutput0", cvOutput0)
  .value("cvOutput1", cvOutput1)
  .value("cvOutput2", cvOutput2)
  .value("cvOutput3", cvOutput3)
  .value("cvOutput4", cvOutput4)
  ;

enum_<CVInputSelect>("CVInputSelect")
  .value("cvInput0", cvInput0)
  .value("cvInput1", cvInput1)
  ;

enum_<CVIOSources>("CVIOSources")
  .value("cvManualSW", cvManualSW)
  .value("cvInputSrc0", cvInputSrc0)
  .value("cvInputSrc1", cvInputSrc1)
  .value("cvCoincidence", cvCoincidence)
  .value("cvVMESignals", cvVMESignals)
  .value("cvMiscSignals", cvMiscSignals)
  ;

enum_<CVTimeUnits>("CVTimeUnits")
  .value("cvUnit25ns", cvUnit25ns)
  .value("cvUnit1600ns", cvUnit1600ns)
  .value("cvUnit410us", cvUnit410us)
  .value("cvUnit104ms", cvUnit104ms)
  ;

enum_<CVLEDPolarity>("CVLEDPolarity")
  .value("cvActiveHigh", cvActiveHigh)
  .value("cvActiveLow", cvActiveLow)
  ;

enum_<CVIOPolarity>("CVIOPolarity")
  .value("cvDirect", cvDirect)
  .value("cvInverted", cvInverted)
  ;

enum_<CVRegisters>("CVRegisters")
  .value("cvStatusReg", cvStatusReg)
  .value("cvVMEControlReg", cvVMEControlReg)
  .value("cvFwRelReg", cvFwRelReg)
  .value("cvFwDldReg", cvFwDldReg)
  .value("cvFlenaReg", cvFlenaReg)
  .value("cvVMEIRQEnaReg", cvVMEIRQEnaReg)
  .value("cvInputReg", cvInputReg)
  .value("cvOutRegSet", cvOutRegSet)
  .value("cvInMuxRegSet", cvInMuxRegSet)
  .value("cvOutMuxRegSet", cvOutMuxRegSet)
  .value("cvLedPolRegSet", cvLedPolRegSet)
  .value("cvOutRegClear", cvOutRegClear)
  .value("cvInMuxRegClear", cvInMuxRegClear)
  .value("cvOutMuxRegClear", cvOutMuxRegClear)
  .value("cvLedPolRegClear", cvLedPolRegClear)
  .value("cvPulserA0", cvPulserA0)
  .value("cvPulserA1", cvPulserA1)
  .value("cvPulserB0", cvPulserB0)
  .value("cvPulserB1", cvPulserB1)
  .value("cvScaler0", cvScaler0)
  .value("cvScaler1", cvScaler1)
  .value("cvDispADL", cvDispADL)
  .value("cvDispADH", cvDispADH)
  .value("cvDispDTL", cvDispDTL)
  .value("cvDispDTH", cvDispDTH)
  .value("cvDispC1", cvDispC1)
  .value("cvDispC2", cvDispC2)
  .value("cvLMADL", cvLMADL)
  .value("cvLMADH", cvLMADH)
  .value("cvLMC", cvLMC)
  ;

enum_<CVStatusRegisterBits>("CVStatusRegisterBits")
  .value("cvSYSRES", cvSYSRES)
  .value("cvSYSCTRL", cvSYSCTRL)
  .value("cvDTACK", cvDTACK)
  .value("cvBERR", cvBERR)
  .value("cvDIP0", cvDIP0)
  .value("cvDIP1", cvDIP1)
  .value("cvDIP2", cvDIP2)
  .value("cvDIP3", cvDIP3)
  .value("cvDIP4", cvDIP4)
  .value("cvUSBTYPE", cvUSBTYPE)
  ;

enum_<CVInputRegisterBits>("CVInputRegisterBits")
  .value("cvIn0Bit", cvIn0Bit)
  .value("cvIn1Bit", cvIn1Bit)
  .value("cvCoincBit", cvCoincBit)
  .value("cvPulsAOutBit", cvPulsAOutBit)
  .value("cvPulsBOutBit", cvPulsBOutBit)
  .value("cvScalEndCntBit", cvScalEndCntBit)
  .value("cvLocMonBit", cvLocMonBit)
  ;

enum_<CVOutputRegisterBits>("CVOutputRegisterBits")
  .value("cvPulsAStartBit", cvPulsAStartBit)
  .value("cvPulsAResetBit", cvPulsAResetBit)
  .value("cvPulsBStartBit", cvPulsBStartBit)
  .value("cvPulsBResetBit", cvPulsBResetBit)
  .value("cvScalGateBit", cvScalGateBit)
  .value("cvScalResetBit", cvScalResetBit)
  .value("cvOut0Bit", cvOut0Bit)
  .value("cvOut1Bit", cvOut1Bit)
  .value("cvOut2Bit", cvOut2Bit)
  .value("cvOut3Bit", cvOut3Bit)
  .value("cvOut4Bit", cvOut4Bit)
  ;

enum_<CVArbiterTypes>("CVArbiterTypes")
  .value("cvPriorized", cvPriorized)
  .value("cvRoundRobin", cvRoundRobin)
  ;

enum_<CVRequesterTypes>("CVRequesterTypes")
  .value("cvFair", cvFair)
  .value("cvDemand", cvDemand)
  ;

enum_<CVReleaseTypes>("CVReleaseTypes")
  .value("cvRWD", cvRWD)
  .value("cvROR", cvROR)
  ;

enum_<CVBusReqLevels>("CVBusReqLevels")
  .value("cvBR0", cvBR0)
  .value("cvBR1", cvBR1)
  .value("cvBR2", cvBR2)
  .value("cvBR3", cvBR3)
  ;

enum_<CVIRQLevels>("CVIRQLevels")
  .value("cvIRQ1", cvIRQ1)
  .value("cvIRQ2", cvIRQ2)
  .value("cvIRQ3", cvIRQ3)
  .value("cvIRQ4", cvIRQ4)
  .value("cvIRQ5", cvIRQ5)
  .value("cvIRQ6", cvIRQ6)
  .value("cvIRQ7", cvIRQ7)
  ;

enum_<CVVMETimeouts>("CVVMETimeouts")
  .value("cvTimeout50us", cvTimeout50us)
  .value("cvTimeout400us", cvTimeout400us)
  ;

class_<CVDisplay>("CVDisplay")
  .def_readwrite("cvAddress",&CVDisplay::cvAddress)
  .def_readwrite("cvData",&CVDisplay::cvData)
  .def_readwrite("cvAM",&CVDisplay::cvAM)
  .def_readwrite("cvIRQ",&CVDisplay::cvIRQ)
  .def_readwrite("cvDS0",&CVDisplay::cvDS0)
  .def_readwrite("cvDS1",&CVDisplay::cvDS1)
  .def_readwrite("cvAS",&CVDisplay::cvAS)
  .def_readwrite("cvIACK",&CVDisplay::cvIACK)
  .def_readwrite("cvWRITE",&CVDisplay::cvWRITE)
  .def_readwrite("cvLWORD",&CVDisplay::cvLWORD)
  .def_readwrite("cvDTACK",&CVDisplay::cvDTACK)
  .def_readwrite("cvBERR",&CVDisplay::cvBERR)
  .def_readwrite("cvSYSRES",&CVDisplay::cvSYSRES)
  .def_readwrite("cvBR",&CVDisplay::cvBR)
  .def_readwrite("cvBG",&CVDisplay::cvBG)
  ;

#endif
