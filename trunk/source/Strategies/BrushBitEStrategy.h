#ifndef BRUSHBITESTRATEGY
#define BRUSHBITESTRATEGY

#include "BrushStrategy.h"

class BrushBitEStrategy : public BrushStrategy
{
  public:
    unsigned int getType(unsigned int para, unsigned int parab);
    unsigned int getFlags(unsigned int para, unsigned int parab);
};

#endif