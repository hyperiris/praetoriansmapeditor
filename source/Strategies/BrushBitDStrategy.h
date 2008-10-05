#ifndef BRUSHBITDSTRATEGY
#define BRUSHBITDSTRATEGY

#include "BrushStrategy.h"

class BrushBitDStrategy : public BrushStrategy
{
  public:
    unsigned int getType(unsigned int para, unsigned int parab);
    unsigned int getFlags(unsigned int para, unsigned int parab);
};

#endif