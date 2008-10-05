#include "BrushBitEStrategy.h"

unsigned int BrushBitEStrategy::getType(unsigned int para, unsigned int parab)
{
  return (para | (parab << 27));
}

unsigned int BrushBitEStrategy::getFlags(unsigned int para, unsigned int parab)
{
  return (para | parab);
}