#include "Triangle.h"

Triangle::Triangle(Tuple3f *t1,  Tuple3f *t2,  Tuple3f *t3)
{
  setVertices(t1, t2, t3);
}

void Triangle::setVertices(Tuple3f *t1,  Tuple3f *t2,  Tuple3f *t3)
{
  vertices[0] = t1;
  vertices[1] = t2;
  vertices[2] = t3;
  if (t1 && t2 && t3)
    plane.set(*t1, *t2, *t3);
}

void Triangle::setNormals(Tuple3f *n1,  Tuple3f *n2,  Tuple3f *n3)
{
  normals[0] = n1;
  normals[1] = n2;
  normals[2] = n3;
}

void Triangle::resetNormals()
{
  if (vertices[0] && vertices[1] && vertices[2])
    plane.set(*vertices[0], *vertices[1], *vertices[2]);
    
  if (normals[0] && normals[1] && normals[2])
  {
    normals[0]->set(0,0,0);
    normals[1]->set(0,0,0);
    normals[2]->set(0,0,0);
  }
}

void Triangle::addToNormals()
{
  if (normals[0] && normals[1] && normals[2])
  {
    *normals[0] += plane.getNormal();
    *normals[1] += plane.getNormal();
    *normals[2] += plane.getNormal();
  }
}

void Triangle::setFacingPoint(const Tuple3f &point)
{
  facingLight = (plane.getNormal().getDotProduct(point) + plane.getOffset() >= 0.0f);
}
