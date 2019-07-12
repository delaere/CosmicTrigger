#include "VmeBoard.h"


vmeBoard::vmeBoard(vmeController* cont, uint32_t baseAddress, 
                   CVAddressModifier AM, CVDataWidth DW, 
                   bool enforceAMDW):cont_(cont),AM_(AM),DW_(DW),enforceAMDW_(enforceAMDW),baseAddress_(baseAddress) {}

