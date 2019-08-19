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

#ifndef __VMEBOARD
#define __VMEBOARD

#include "CommonDef.h"
#include "CAENVMEtypes.h"
#include "VmeController.h"

// a generic VME board
class VmeBoard{
  
public:
  VmeBoard(VmeController* cont, uint32_t baseAddress, CVAddressModifier AM=cvA32_U_DATA, CVDataWidth DW=cvD16, bool enforceAMDW=false);
  virtual ~VmeBoard() {}
  
  // Enforce the AM and DW. 
  // If set to true, the mode will be enforced for all VME operations, disregarding the VmeController configuration.
  inline void enforceAMDW(bool enforce) { enforceAMDW_ = enforce; }
  inline bool isAMDWenforced() { return enforceAMDW_; }
  
protected:
  // access to the VME controller. If AMDW is enforced, the returned object is ready.
  inline const VmeController* controller() const { return (enforceAMDW_ ? cont_->mode(AM_,DW_) : cont_); }

  // get/set the module base address
  inline void setbaseAddress(uint32_t add) { baseAddress_ = add; }
  inline uint32_t baseAddress() const { return baseAddress_; }
  
  // proxy for the controller operations.
  // it automatically uses the proper AM/DW mode if enforced
  
  template<typename T> void writeData(long unsigned int address,T data) const { 
    controller()->writeData(address,data); 
  }
  template<typename T> T readData (long unsigned int address) const { 
    return controller()->readData<T>(address); 
  }
  template<typename T> T readWriteData(const long unsigned int address,T data) const { 
    return controller()->readWriteData(address,data); 
  }
  template<typename T> std::vector<T> blockReadData(const long unsigned int address, int size, bool multiplex=false) const { 
    return controller()->blockReadData<T>(address, size, multiplex); 
  }
  template<typename T> int blockWriteData(const long unsigned int address, std::vector<T> data, bool multiplex=false) const {
    return controller()->blockWriteData(address, data, multiplex); 
  }
  inline void ADOCycle(const long unsigned int address) const {
    controller()->ADOCycle(address);
  }

private:
  VmeController* cont_;  ///< Pointer to the controller
  CVAddressModifier AM_; ///< Stored AM value
  CVDataWidth DW_;       ///< Stored DW value
  bool enforceAMDW_;     ///< Should the mode be enforced ?
  uint32_t baseAddress_; ///< The base address of the board
};

#endif
