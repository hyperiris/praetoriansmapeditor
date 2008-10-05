#include "CollisionAlgorithmB.h"
#include "../Geometry/Ray3D.h"

CollisionAlgorithmB::CollisionAlgorithmB()
{}

Tuple4f CollisionAlgorithmB::computeRayTriangleIntersection(Ray3D *ray, const Tuple3f &p0, const Tuple3f &p1, const Tuple3f &p2)
{
  Planef trianglePlane;
  Tuple3f Ro;
  Tuple3f vect;
  Tuple3f collisionPoint;
  Tuple3f edge[3];
  Tuple3f planeN[3];
  float result;
  
  Ro = ray->getOrigin();
  
  trianglePlane.set(p0, p1, p2);
  
  if (-1 == CollisionAlgorithm::checkAgainstPlaneNormal(Ro - p0, trianglePlane.getNormal()))
    return Tuple4f(0, 0, 0, -1);
    
  edge[0] = p0;
  edge[1] = p1;
  edge[2] = p2;
  
  edge[0] -= Ro;
  edge[1] -= Ro;
  edge[2] -= Ro;
  
  planeN[0].crossProduct(edge[0], edge[1]).normalize();
  planeN[1].crossProduct(edge[1], edge[2]).normalize();
  planeN[2].crossProduct(edge[2], edge[0]).normalize();
  
  collisionPoint = CollisionAlgorithm::computeCollisionPoint(ray, trianglePlane, p0);
  
  vect = collisionPoint - Ro;
  
  result = 0.0f < CollisionAlgorithm::checkAgainstPlaneNormal(vect, planeN[0]) ? -1.0f/*fail*/ :
           0.0f < CollisionAlgorithm::checkAgainstPlaneNormal(vect, planeN[1]) ? -1.0f/*fail*/ :
           0.0f < CollisionAlgorithm::checkAgainstPlaneNormal(vect, planeN[2]) ? -1.0f/*fail*/ :
           1.0f/*pass*/;
           
  return Tuple4f(collisionPoint, result);
}

CollisionAlgorithmB::~CollisionAlgorithmB()
{}
