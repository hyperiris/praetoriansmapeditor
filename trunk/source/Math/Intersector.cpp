#include "Intersector.h"
#include "CollisionAlgorithmB.h"
#include "../Tools/Logger.h"
#include "../Containers/ocarray.h"
#include "../Geometry/DistanceObject.h"
#include "../Geometry/Ray3D.h"
#include "../Containers/ocsort.h"

Intersector::Intersector()
{
}

Tuple4f Intersector::intersectTriangles(Ray3D* r, Tuple3f* v, unsigned short* idx, unsigned int tc)
{
  //Tuple4f packet;
  //Tuple4f cp(0, 0, 0, -1);
  //Array < DistanceObject<Tuple4f> > triangleList(ic/3);
  //Tuple3f ro = r->getOrigin();
  //
  //for (unsigned int i = 0; i < 8; i++)
  //{
  //  packet = algorithm.computeRayTriangleIntersection(r, v[idx[i]], v[idx[i+1]], v[idx[i+2]]);
  //  if (1 == packet.w)
  //  {
  //    cp = packet;
  //    DistanceObject<Tuple4f> distObj(cp);
  //    distObj.setDistance(ro.getDistance(Tuple3f(cp.x, cp.y, cp.z)));
  //    triangleList.append(distObj);
  //    //break;
  //  }
  //}
  //
  //if (triangleList.isEmpty())
  //  return cp;
  //
  //OCQuickSort(triangleList, 0, triangleList.length());
  //return triangleList(0).getObject();
  
  Tuple3f *vert = v;
  Tuple4f packet;
  Tuple4f cp(0, 0, 0, -1);
  
  for (unsigned int i = 0; i < tc; i++)
  {
    packet = algorithm.computeRayTriangleIntersection(r, vert[idx[0]], vert[idx[i+1]], vert[idx[i+2]]);
    if (1 == packet.w)
    {
      cp = packet;
      break;
    }
  }
  
  return cp;
}

Tuple4f Intersector::intersectTriangles(Ray3D* r, Tuple3f* v, Tuple3i* idx, unsigned int tc)
{
  Tuple3f *vert = v;
  Tuple4f packet;
  Tuple4f cp(0, 0, 0, -1);
  
  for (unsigned int i = 0; i < tc; i++)
  {
    packet = algorithm.computeRayTriangleIntersection(r, vert[idx[i].x], vert[idx[i].y], vert[idx[i].z]);
    if (1 == packet.w)
    {
      cp = packet;
      break;
    }
  }
  
  return cp;
}
Intersector::~Intersector()
{
}
