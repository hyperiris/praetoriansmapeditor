#ifndef RAY3D
#define RAY3D

#include "../Math/Tuple4.h"

class Ray3D
{
  public:
    Ray3D();
    Ray3D(const Tuple3f &Po, const Tuple3f &Pd);
    ~Ray3D();
    
  public:
    void setOrigin(const Tuple3f &Po);
    void setDestination(const Tuple3f &Pd);
    void set(const Tuple3f &Po, const Tuple3f &Pd);
    
    float getLength();
    const Tuple3f &getOrigin();
    Tuple3f getDirection();
    const Tuple3f &getDestination();
    
    void normalizeDirection();
    
  private:
    Tuple3f origin;
    Tuple3f destination;
};

#endif