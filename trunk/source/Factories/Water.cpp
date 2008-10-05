#include "Water.h"
#include "../Geometry/Edge.h"
#include "../Geometry/DistanceObject.h"
#include "../Containers/ocsort.h"
#include "../Math/MathUtils.h"

Water::Water() : triangleList(200), vertexList(100)
{
  color.set(0.0f, 0.48f, 0.68f, 1.0f);
}

void Water::addVertex(const Tuple3f &vertex)
{
  vertexList.append(vertex);
  
  Array < DistanceObject<Tuple3f> > distanceObjectList(vertexList.length());
  DistanceObject <Tuple3f> distanceObject;
  DistanceObject <Tuple3f> *objs;
  Tuple3f* verts = vertexList.data();
  int j = vertexList.length();
  
  //*/optimization
  for (unsigned int i = 0; i < j; i++)
  {
    distanceObject.set(verts[i].x, verts[i]);
    distanceObjectList.append(distanceObject);
  }
  
  objs = distanceObjectList.data();
  OCInsertionSort(objs, 0, j);
  
  for (unsigned int i = 0; i < j; i++)
    vertexList(i) = objs[i].getObject();
  //*/
  
  if (j>2)
    triangulate();
}

void Water::triangulate()
{
  triangleList.clearFast();
  
  Array <Edge> pe(6);
  Array <Tuple2i> te(3);
  Tuple3i tri, *tp;
  Tuple3f *vp;
  Edge *ep;
  unsigned int i,s,j,k,l,o[3]={1,3,2};
  
  vertexList += computeSuperTriangle();
  s = vertexList.length();
  vp = vertexList.data();
  triangleList.append(Tuple3i(s-3,s-2,s-1));
  
  for (i = 0; i < s-3; i++)
  {
    pe.clearFast();
    for (j = 0; j < triangleList.length(); j++)
    {
      te.clearFast();
      tri = triangleList(j);
      for (k = 0; k < 3; k++)
      {
        Tuple2i e(tri[k], tri[o[k]-k]);
        if (!hasInfiniteSlope(vp[e[0]], vp[e[1]]))
          te.append(e);
      }
      if (te.length()<2)
        continue;
      Tuple2i a(te[0]), b(te[1]);
      if (withinRange(vp[i], vp[a[0]], vp[a[1]], vp[b[0]], vp[b[1]]))
      {
        for (k = 0; k < 3; k++)
        {
          Edge e(tri[k], tri[o[k]-k]);
          if (!pe.remove(e))
            pe.append(e);
        }
        triangleList.removeAt(j--);
      }
    }
    ep = pe.data();
    for (l = 0; l < pe.length(); l++)
      triangleList.append(Tuple3i(i, ep[l].e[0], ep[l].e[1]));
  }
  
  tp = triangleList.data();
  for (i = 0; i < triangleList.length(); i++)
    if (tp[i].x >= s-3 || tp[i].y >= s-3 || tp[i].z >= s-3)
      triangleList.removeAt(i--);
  vertexList.removeRange(s-3,3);
}

bool Water::withinRange(const Tuple3f &v, const Tuple3f &a, const Tuple3f &b, const Tuple3f &c, const Tuple3f &d)
{
  Tuple2f cc = computeCircumCenter(a,b,c,d);
  float dx = v.x-cc.x, dy = v.z-cc.y;
  float cx = a.x-cc.x, cy = a.z-cc.y;
  return ((dx*dx+dy*dy)<=(cx*cx+cy*cy));
}

Tuple2f Water::computeCircumCenter(const Tuple3f &a, const Tuple3f &b, const Tuple3f &c, const Tuple3f &d)
{
  Tuple2f midAB,midCD,cc;
  float srAB,srCD,b1,b2;
  
  midAB.set((a.x+b.x)/2, (a.z+b.z)/2);
  midCD.set((c.x+d.x)/2, (c.z+d.z)/2);
  srAB = -(b.x-a.x)/(b.z-a.z);
  srCD = -(c.x-d.x)/(c.z-d.z);
  
  b1 = midAB.y-srAB*midAB.x;
  b2 = midCD.y-srCD*midCD.x;
  cc.x = (b2-b1)/(srAB-srCD);
  cc.y = srAB*cc.x+b1;
  
  /*cc.x = ((srAB-srCD)+(midCD.y-midAB.y))/(srAB-srCD);
  cc.y = srAB*(cc.x-midAB.x)+midAB.y;*/
  return cc;
}

bool Water::hasInfiniteSlope(const Tuple3f &a, const Tuple3f &b)
{
  return (abs(b.z-a.z)<EPSILON);
}

Array <Tuple3f> Water::computeSuperTriangle()
{
  Array <Tuple3f> temp;
  Tuple3f minB(vertexList(0));
  Tuple3f maxB(vertexList(0));
  Tuple3f *v = vertexList.data();
  float x,y;
  
  for (unsigned int i = 0; i < vertexList.length(); i++)
  {
    x=v[i].x;
    y=v[i].z;
    minB.x = x < minB.x ? x : minB.x;
    maxB.x = x > maxB.x ? x : maxB.x;
    minB.z = y < minB.z ? y : minB.z;
    maxB.z = y > maxB.z ? y : maxB.z;
  }
  
  minB.z -= (maxB.z-minB.z)*20;
  maxB.x += (maxB.x-minB.x)*20;
  minB.x -= 10;
  maxB.z += 10;
  
  temp.append(minB);
  temp.append(Tuple3f(minB.x, 0, maxB.z));
  temp.append(maxB);
  return temp;
}

void Water::draw(int info)
{
  if (triangleList.isEmpty())
    return;
    
  if (info & FILL)
  {
    glColor4fv(color);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, getVertices());
    glEnable(GL_BLEND);
    //glBlendFunc(GL_DST_COLOR, GL_SRC_ALPHA);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDrawElements(GL_TRIANGLES, getTriangleCount()*3, GL_UNSIGNED_INT, getTriangles());
    glDisable(GL_BLEND);
    glDisableClientState(GL_VERTEX_ARRAY);
  }
  
  if (info & WIRE)
  {
    glColor4f(0,0,1,1);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, getVertices());
    glEnable(GL_POLYGON_OFFSET_LINE);
    glPolygonOffset(-1.0f, -1.0f);
    glPolygonMode(GL_FRONT, GL_LINE);
    glDrawElements(GL_TRIANGLES, getTriangleCount()*3, GL_UNSIGNED_INT, getTriangles());
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glPolygonOffset(1.0f, 1.0f);
    glDisable(GL_POLYGON_OFFSET_LINE);
    glDisableClientState(GL_VERTEX_ARRAY);
  }
  
  glColor4f(1,1,1,1);
}

void Water::removeTriangle(unsigned int index)
{
  triangleList.removeAt(index);
}

unsigned int Water::getTriangleCount()
{
  return triangleList.length();
}

unsigned int Water::getVertexCount()
{
  return vertexList.length();
}

Tuple3f* Water::getVertices()
{
  return vertexList.data();
}

Tuple3i* Water::getTriangles()
{
  return triangleList.data();
}

void Water::setColor(float r, float g, float b, float a)
{
  color.set(r,g,b,a);
}

void Water::setColor(const Tuple4f& c)
{
  color = c;
}

Tuple4f Water::getColor()
{
  return color;
}

Water::~Water()
{
}