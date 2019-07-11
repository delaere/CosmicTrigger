#include "VmeBoard.h"


vmeBoard::vmeBoard(vmeController* cont,CVAddressModifier AM,CVDataWidth DW){
  this->cont=cont;
  this->AM=AM;
  this->DW=DW;
}

bool vmeBoard::vLevel(coutLevel level){
    return(this->cont->verbosity()>=(int)level);
}

void vmeBoard::writeData(long unsigned int add, void *DATA){
  cont->mode(AM,DW)->writeData(add,DATA);
}
void vmeBoard::readData(long unsigned int add, void *DATA){
  cont->mode(AM,DW)->readData(add,DATA);
}
void vmeBoard::writeData(long unsigned int add, void *DATA,CVAddressModifier tAM, CVDataWidth tDW){
  cont->mode(tAM,tDW)->writeData(add,DATA);
}
void vmeBoard::readData(long unsigned int add, void *DATA,CVAddressModifier tAM, CVDataWidth tDW){
  cont->mode(tAM,tDW)->readData(add,DATA);
}

void vmeBoard::setAM(CVAddressModifier AM){
  this->AM=AM;
}
void vmeBoard::setDW(CVDataWidth DW){
  this->DW=DW;
}
