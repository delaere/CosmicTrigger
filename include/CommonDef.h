#ifndef __TYPEDEF
#define __TYPEDEF

#include <iostream>
#include <cmath>
#include <sstream>
#include <stdlib.h>

using namespace std;

//TODO only used by TDC... check and move there.

unsigned int digit(unsigned int data, int begin, int end);

unsigned int digit(unsigned int data, int digit);


/// Defines coutLevels to manage library output.
typedef enum coutLevel{
      SILENT = 0,	///<Shows nearly nothing, except fatal errors
      ERROR  =  1,	///<Shows errors
      WARNING = 2,	///<Shows warning (usually bad parameters)
      NORMAL = 3,	///<Normal level, explains what it's doing
      DEBUG = 4		///<Debug, it talks... A LOT
  
} coutLevel;

#endif
