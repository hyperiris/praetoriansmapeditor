#ifndef COLLISIONALGORITHMB
#define COLLISIONALGORITHMB

#include "CollisionAlgorithm.h"

class CollisionAlgorithmB : public CollisionAlgorithm
{
  public:
    CollisionAlgorithmB();
    ~CollisionAlgorithmB();
    
  public:
    Tuple4f computeRayTriangleIntersection(Ray3D *ray, const Tuple3f &p0, const Tuple3f &p1, const Tuple3f &p2);
};

#endif
