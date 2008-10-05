#include "BrushBitDStrategy.h"

unsigned int BrushBitDStrategy::getType(unsigned int para, unsigned int parab)
{
  return (para & ~(parab << 27));
}

unsigned int BrushBitDStrategy::getFlags(unsigned int para, unsigned int parab)
{
  return (para & ~parab);
}