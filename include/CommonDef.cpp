#include "CommonDef.h"

// TODO rewrite this: it looks like an awkward way to apply a bit mask
unsigned int digit(unsigned int data, int begin, int end)
{
    if (begin<end){return -1;}
    data=data%(int)pow(2.0,begin+1);
    data=data-data%(int)pow(2.0,end);
    return (data/pow(2.0,end));
}

unsigned int digit(unsigned int data, int position)
{
    return digit(data, position, position);
}
