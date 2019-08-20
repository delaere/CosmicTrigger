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

#include "PythonModule.h"
#include "PythonCaenVmeTypes.h"
#include "VmeController.h"
#include "VmeUsbBridge.h"
#include "VmeBoard.h"
#include "CaenetBridge.h"
#include "HVmodule.h"
#include "N470HVmodule.h"
#include "SY527PowerSystem.h"
#include "Scaler.h"
#include "Discri.h"
#include "TDC.h"
#include "TTCvi.h"


BOOST_PYTHON_MODULE(VeheMencE)
{
  // expose the CAENVMEtypes enums
  exposeCaenVmeTypes();
  
  // expose VmeController
  exposeToPython<VmeController>();

  // expose VmeUsbBridge
  exposeToPython<VmeUsbBridge>();
  exposeToPython<V1718Pulser>();
  exposeToPython<V1718Scaler>();
  
  // expose VmeBoard
  exposeToPython<VmeBoard>();
  
  // expose CaenetBridge
  exposeToPython<CaenetBridge>();
  
  // expose HVModule
  exposeToPython<HVModule>();
  exposeToPython<HVChannel>();
  exposeToPython<HVBoard>();
  
  // expose N470HVModule
  exposeToPython<N470HVModule>();
  exposeToPython<N470HVChannel>();
  exposeToPython<N470StatusWord>();
  
  // expose SY527PowerSystem
  exposeToPython<SY527StatusWord>();
  exposeToPython<SY527HVChannel>();
  exposeToPython<SY527PowerSystem>();
  
  // expose 1151N scaler
  exposeToPython<Scaler>();
  
  // expose V812 discri
  exposeToPython<Discri>();
  
  // expose TDC classes
  exposeToPython<TDCHit>();
  exposeToPython<TDCEvent>();
  exposeToPython<V1190Event>();
  exposeToPython<V1190ControlRegister>();
  exposeToPython<V1190StatusRegister>();
  exposeToPython<Tdc>();
  
  // expose TtcVi
  exposeToPython<TtcVi>();
}
