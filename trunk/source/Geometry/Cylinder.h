#ifndef CYLINDER
#define CYLINDER

#include "../Math/Tuple4.h"

class Cylinder
{
  public:
    Cylinder();
    ~Cylinder();
    
  public:
    void set(float fRadius, float fHeight, unsigned int nSides);
    void setTopVerticesColor(float r, float g, float b, float a);
    void setBottomVerticesColor(float r, float g, float b, float a);
    
    void setRadius(float r);
    void setHeight(float h);
    void setNumSides(unsigned int n);
    
    void draw();
    
  private:
    float radius;
    float height;
    unsigned int sides;
    Tuple4f topVertsColor;
    Tuple4f bottomVertsColor;
};

#endif