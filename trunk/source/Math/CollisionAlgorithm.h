#ifndef COLLISIONALGORITHM
#define COLLISIONALGORITHM

#include "../Geometry/Plane.h"
#include "../Math/Tuple4.h"

class Ray3D;

enum CollisionState
{
  NOT_COLLIDED = 0,
  COLLIDED
};
//
//enum Sign
//{
//  NEGATIVE = -1,
//  POSITIVE =  1
//};

class CollisionAlgorithm
{
  public:
    CollisionAlgorithm();
    virtual ~CollisionAlgorithm();
    
  public:
    virtual Tuple4f computeRayTriangleIntersection(Ray3D *ray, const Tuple3f &p0, const Tuple3f &p1, const Tuple3f &p2);
    virtual Tuple3f computeCollisionPoint(Ray3D *ray, const Planef &plane, const Tuple3f &point);
    virtual int checkAgainstPlaneNormal(const Tuple3f &vector, const Tuple3f &Pn);
    virtual int checkAgainstTriangleBounds(const Tuple4f &cp, const Tuple3f &p0, const Tuple3f &p1, const Tuple3f &p2);
    virtual int checkAgainstTriangleBounds(const Tuple3f &cp, const Tuple3f &p0, const Tuple3f &p1, const Tuple3f &p2);
};

#endif
