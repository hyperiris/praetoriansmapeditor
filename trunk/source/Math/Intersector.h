#ifndef INTERSECTOR
#define INTERSECTOR

#include "CollisionAlgorithmB.h"

class Intersector
{
  public:
    Intersector();
    ~Intersector();
    
  public:
    //Tuple4f intersectQuad(Ray3D* r, Tuple3f* v, unsigned int vcnt, unsigned int mode);
    Tuple4f intersectTriangles(Ray3D* r, Tuple3f* v, unsigned short* idx, unsigned int tc);
    Tuple4f intersectTriangles(Ray3D* r, Tuple3f* v, Tuple3i* idx, unsigned int tc);
    
  private:
    CollisionAlgorithmB algorithm;
};

#endif