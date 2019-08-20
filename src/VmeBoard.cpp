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

#include "VmeBoard.h"
#include "PythonModule.h"

VmeBoard::VmeBoard(VmeController* cont, uint32_t baseAddress, 
                   CVAddressModifier AM, CVDataWidth DW, 
                   bool enforceAMDW):cont_(cont),AM_(AM),DW_(DW),enforceAMDW_(enforceAMDW),baseAddress_(baseAddress) {}
                   
using namespace boost::python;

template<> void exposeToPython<VmeBoard>() {
  class_<VmeBoard>("VmeBoard",init<VmeController*, uint32_t, CVAddressModifier, CVDataWidth, bool>())
    .add_property("enforce",&VmeBoard::isAMDWenforced,&VmeBoard::enforceAMDW)
  ;
}


