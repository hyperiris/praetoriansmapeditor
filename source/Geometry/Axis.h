#ifndef AXIS
#define AXIS

#include "../Math/Tuple4.h"

class Axis
{
  public:
    Axis(void);
    ~Axis(void);
    
  public:
    void draw();
    void setOffset(const Tuple3f &offset);
    
  private:
    Tuple3f yOffset;
};

#endif