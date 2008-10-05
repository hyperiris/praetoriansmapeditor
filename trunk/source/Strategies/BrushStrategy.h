#ifndef BRUSHSTRATEGY
#define BRUSHSTRATEGY

class BrushStrategy
{
  public:
    virtual unsigned int getType(unsigned int para, unsigned int parab) = 0;
    virtual unsigned int getFlags(unsigned int para, unsigned int parab) = 0;
};

#endif