#include "CollisionAlgorithm.h"
#include "../Geometry/Ray3D.h"
#include "../Math/MathUtils.h"

CollisionAlgorithm::CollisionAlgorithm()
{}

int CollisionAlgorithm::checkAgainstPlaneNormal(const Tuple3f &vector, const Tuple3f &Pn)
{
  return 0.0f < vector.getDotProduct(Pn) ? 1 : -1;
}

int CollisionAlgorithm::checkAgainstTriangleBounds(const Tuple4f &cp, const Tuple3f &p0, const Tuple3f &p1, const Tuple3f &p2)
{
  Tuple3f vectorA;
  Tuple3f vectorB;
  Tuple3f vectorC;
  
  float epsilon;
  float angle;
  float n1;
  float n2;
  float n3;
  
  vectorA.x = cp.x - p0.x;
  vectorA.y = cp.y - p0.y;
  vectorA.z = cp.z - p0.z;
  
  vectorB.x = cp.x - p1.x;
  vectorB.y = cp.y - p1.y;
  vectorB.z = cp.z - p1.z;
  
  vectorC.x = cp.x - p2.x;
  vectorC.y = cp.y - p2.y;
  vectorC.z = cp.z - p2.z;
  
  vectorA.normalize();
  vectorB.normalize();
  vectorC.normalize();
  
  n1 = vectorA.getDotProduct(vectorB);
  n2 = vectorB.getDotProduct(vectorC);
  n3 = vectorC.getDotProduct(vectorA);
  
  n1 = n1 < -1 ? -1 : n1 > 1 ? 1 : n1;
  n2 = n2 < -1 ? -1 : n2 > 1 ? 1 : n2;
  n3 = n3 < -1 ? -1 : n3 > 1 ? 1 : n3;
  
  angle = acos(n1) + acos(n2) + acos(n3);
  
  epsilon = fabs(angle - 2 * PI);
  
  if (/*0.01*/0.005 >= epsilon)
    return COLLIDED;
    
  return NOT_COLLIDED;
}

int CollisionAlgorithm::checkAgainstTriangleBounds(const Tuple3f &cp, const Tuple3f &p0, const Tuple3f &p1, const Tuple3f &p2)
{
  return checkAgainstTriangleBounds(Tuple4f(cp), p0, p1, p2);
}

Tuple4f CollisionAlgorithm::computeRayTriangleIntersection(Ray3D *ray, const Tuple3f &p0, const Tuple3f &p1, const Tuple3f &p2)
{
  Planef plane;
  Tuple4f packet;
  
  plane.set(p0, p1, p2);
  
  packet   = computeCollisionPoint(ray, plane, p0);
  packet.w = (float) checkAgainstTriangleBounds(packet, p0, p1, p2);
  
  return packet;
}

Tuple3f CollisionAlgorithm::computeCollisionPoint(Ray3D *ray, const Planef &plane, const Tuple3f &point)
{
  Tuple3f Ro  = ray->getOrigin();
  Tuple3f Rd  = ray->getDestination();
  Tuple3f Pn  = plane.getNormal();
  Tuple3f Pcp;
  
  float PnRo = Pn.getDotProduct(Ro);
  float PnRd = Pn.getDotProduct(Rd);
  float PnPp = Pn.getDotProduct(point);
  float q    = PnRd - PnPp;
  float r    = PnRo - PnRd;
  float t    = 0;
  
  //if( 0 < (r - q) )
  //  return Pcp;
  
  if ((Ro.x == Rd.x) && (Ro.z == Rd.z))
  {
    Pcp.x = Ro.x;
    Pcp.y = Ro.y + (-(PnRo - PnPp) / Pn.y);// added Ro to the equation
    Pcp.z = Ro.z;
  }
  else
  {
    t   = r ? (-q / r) : 0;
    Pcp = Ro * t + Rd * (1 - t);
  }
  
  return Pcp;
}

CollisionAlgorithm::~CollisionAlgorithm()
{}
