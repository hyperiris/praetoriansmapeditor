#include "BoundsDescriptor.h"
#include "../Renderer/Renderer.h"
#include "../Tools/Logger.h"

unsigned short BoundsDescriptor::indicesFill[36] =
  {0, 4, 7, 7, 3, 0,
   3, 7, 6, 6, 2, 3,
   2, 6, 5, 5, 1, 2,
   1, 5, 0, 0, 5, 4,
   4, 5, 7, 7, 5, 6,
   0, 3, 2, 2, 1, 0
  };

BoundsDescriptor::BoundsDescriptor()
{
  initialized = false;
}

BoundsDescriptor:: BoundsDescriptor(const BoundsDescriptor &copy)
{
  initialized = false;
  operator =(copy);
}

BoundsDescriptor &BoundsDescriptor::operator=(const BoundsDescriptor &copy)
{
  if (this != &copy)
  {
    initialized = copy.initialized;
    minEndAABB  = copy.minEndAABB;
    maxEndAABB  = copy.maxEndAABB;
    centerAABB  = copy.centerAABB;
    centerOBB   = copy.centerOBB;
    extents     = copy.extents;
    memcpy(axii       , copy.axii       , sizeof(Tuple3f)*3);
    memcpy(verticesOBB, copy.verticesOBB, sizeof(Tuple3f)*8);
  }
  return *this;
}

void BoundsDescriptor::reset()
{
  memset(verticesOBB, 0, sizeof(Tuple3f)*8);
  initialized = false;
  minEndAABB.set(0,0,0);
  maxEndAABB.set(0,0,0);
  centerAABB.set(0,0,0);
  centerOBB.set(0,0,0);
  axii[0].set(0,0,0);
  axii[1].set(0,0,0);
  axii[2].set(0,0,0);
  extents.set(0,0,0);
}

void BoundsDescriptor::operator+=(const BoundsDescriptor &bounds)
{
  Tuple3f min,
  max;
  min.x = (minEndAABB.x < bounds.minEndAABB.x) ? minEndAABB.x : bounds.minEndAABB.x;
  min.y = (minEndAABB.y < bounds.minEndAABB.y) ? minEndAABB.y : bounds.minEndAABB.y;
  min.z = (minEndAABB.z < bounds.minEndAABB.z) ? minEndAABB.z : bounds.minEndAABB.z;
  
  max.x = (maxEndAABB.x > bounds.maxEndAABB.x) ? maxEndAABB.x : bounds.maxEndAABB.x;
  max.y = (maxEndAABB.y > bounds.maxEndAABB.y) ? maxEndAABB.y : bounds.maxEndAABB.y;
  max.z = (maxEndAABB.z > bounds.maxEndAABB.z) ? maxEndAABB.z : bounds.maxEndAABB.z;
  
  computeBounds(min, max);
}

BoundsDescriptor BoundsDescriptor::operator+(const BoundsDescriptor &bounds)
{
  BoundsDescriptor descriptor;
  Tuple3f min,
  max;
  
  min.x = (minEndAABB.x < bounds.minEndAABB.x) ? minEndAABB.x : bounds.minEndAABB.x;
  min.y = (minEndAABB.y < bounds.minEndAABB.y) ? minEndAABB.y : bounds.minEndAABB.y;
  min.z = (minEndAABB.z < bounds.minEndAABB.z) ? minEndAABB.z : bounds.minEndAABB.z;
  
  max.x = (maxEndAABB.x > bounds.maxEndAABB.x) ? maxEndAABB.x : bounds.maxEndAABB.x;
  max.y = (maxEndAABB.y > bounds.maxEndAABB.y) ? maxEndAABB.y : bounds.maxEndAABB.y;
  max.z = (maxEndAABB.z > bounds.maxEndAABB.z) ? maxEndAABB.z : bounds.maxEndAABB.z;
  
  descriptor.computeBounds(min, max);
  return descriptor;
}

void BoundsDescriptor::operator*=(const Matrix4f &matrix)
{
  if (initialized)
  {
    verticesOBB[0] *= matrix;
    verticesOBB[1] *= matrix;
    verticesOBB[2] *= matrix;
    verticesOBB[3] *= matrix;
    verticesOBB[4] *= matrix;
    verticesOBB[5] *= matrix;
    verticesOBB[6] *= matrix;
    verticesOBB[7] *= matrix;
    centerOBB      *= matrix;
    computeAABBEnds();
    computeAxiiAndExtents();
  }
}

BoundsDescriptor BoundsDescriptor::operator *(const Matrix4f &matrix)
{
  BoundsDescriptor descriptor;
  if (initialized)
  {
    descriptor.initialized = true;
    descriptor.verticesOBB[0] = verticesOBB[0]*matrix;
    descriptor.verticesOBB[1] = verticesOBB[1]*matrix;
    descriptor.verticesOBB[2] = verticesOBB[2]*matrix;
    descriptor.verticesOBB[3] = verticesOBB[3]*matrix;
    descriptor.verticesOBB[4] = verticesOBB[4]*matrix;
    descriptor.verticesOBB[5] = verticesOBB[5]*matrix;
    descriptor.verticesOBB[6] = verticesOBB[6]*matrix;
    descriptor.verticesOBB[7] = verticesOBB[7]*matrix;
    descriptor.centerOBB      = centerOBB     *matrix;
    descriptor.computeAABBEnds();
    descriptor.computeAxiiAndExtents();
  }
  return descriptor;
}

bool BoundsDescriptor::isInitialized()
{
  return initialized;
}

void  BoundsDescriptor::computeBounds(const Tuple3f &minEnd, const Tuple3f &maxEnd)
{
  computeBounds(minEnd.x, minEnd.y, minEnd.z,
                maxEnd.x, maxEnd.y, maxEnd.z);
}

void  BoundsDescriptor::computeBounds(float minX, float minY, float minZ,
                                      float maxX, float maxY, float maxZ)
{
  initialized  = true;
  
  // 7---------6
  // |\        |\
  // | \       | \
  // |  3---------2
  // |  |      |  |
  // 4 -| - - -5  |
  //  \ |       \ |
  //   \|        \|
  //    0---------1
  
  verticesOBB[0].set(minX, minY, minZ);
  verticesOBB[1].set(maxX, minY, minZ);
  verticesOBB[2].set(maxX, maxY, minZ);
  verticesOBB[3].set(minX, maxY, minZ);
  
  verticesOBB[4].set(minX, minY, maxZ);
  verticesOBB[5].set(maxX, minY, maxZ);
  verticesOBB[6].set(maxX, maxY, maxZ);
  verticesOBB[7].set(minX, maxY, maxZ);
  
  maxEndAABB.set(maxX, maxY, maxZ);
  minEndAABB.set(minX, minY, minZ);
  
  centerAABB  = maxEndAABB;
  centerAABB += minEndAABB;
  centerAABB /= 2.0f;
  
  centerOBB   = centerAABB;
  computeAxiiAndExtents();
}

void  BoundsDescriptor::computeAxiiAndExtents()
{
  // [1]
  //  |
  //  |
  //  |
  //  4-------[0]
  //   \
  //    \
  //     \
  //     [2]
  
  axii[0]     = verticesOBB[5];
  axii[0]    -= verticesOBB[4];
  extents.x   = axii[0].getLength()/2.0f;
  
  axii[1]     = verticesOBB[7];
  axii[1]    -= verticesOBB[4];
  extents.y   = axii[1].getLength()/2.0f;
  
  axii[2]     = verticesOBB[0];
  axii[2]    -= verticesOBB[4];
  extents.z   = axii[2].getLength()/2.0f;
  
  axii[0].normalize();
  axii[1].normalize();
  axii[2].normalize();
}

const bool BoundsDescriptor::sphereOverlapsOBB(const Tuple4f &t) const
{
  return sphereOverlapsOBB(Tuple3f(t.x, t.y, t.z) , t.w);
}

const bool BoundsDescriptor::lineOverlapsOBB(const Tuple3f &start, const Tuple3f& end) const
{
  float fAWdU[3], fAWxDdU[3], fRhs;
  
  Tuple3f kDiff = start;
  kDiff -= centerAABB;
  
  Tuple3f direction = end;
  direction -= start;
  
  Tuple3f kWxD = direction;
  kWxD ^= kDiff;
  
  direction.normalize();
  
  fAWdU[1] = fabs(direction.getDotProduct(axii[1]));
  fAWdU[2] = fabs(direction.getDotProduct(axii[2]));
  fAWxDdU[0] = fabs(kWxD.getDotProduct(axii[0]));
  fRhs = extents[1]*fAWdU[2] + extents[2]*fAWdU[1];
  if (fAWxDdU[0] > fRhs)
    return false;
    
  fAWdU[0] = fabs(direction.getDotProduct(axii[0]));
  fAWxDdU[1] = fabs(kWxD.getDotProduct(axii[1]));
  fRhs = extents[0]*fAWdU[2] + extents[2]*fAWdU[0];
  if (fAWxDdU[1] > fRhs)
    return false;
    
  fAWxDdU[2] = fabs(kWxD.getDotProduct(axii[2]));
  fRhs = extents[0]*fAWdU[1] + extents[1]*fAWdU[0];
  if (fAWxDdU[2] > fRhs)
    return false;
    
  return true;
}

const bool BoundsDescriptor::sphereOverlapsOBB(const Tuple3f &sphrCen, float radius) const
{
  static Tuple3f normals[6] = { Tuple3f(0.0f, 0.0f, 1.0f), Tuple3f(0.0f, 0.0f,-1.0f),
                                Tuple3f(0.0f, 1.0f, 0.0f), Tuple3f(0.0f,-1.0f, 0.0f),
                                Tuple3f(1.0f, 0.0f, 0.0f), Tuple3f(-1.0f, 0.0f, 0.0f)
                              };
                              
  if (normals[0].getDotProduct(sphrCen) - normals[0].getDotProduct(verticesOBB[0]) >= radius) return false;
  if (normals[3].getDotProduct(sphrCen) - normals[3].getDotProduct(verticesOBB[0]) >= radius) return false;
  if (normals[5].getDotProduct(sphrCen) - normals[5].getDotProduct(verticesOBB[0]) >= radius) return false;
  
  if (normals[1].getDotProduct(sphrCen) - normals[1].getDotProduct(verticesOBB[6]) >= radius) return false;
  if (normals[2].getDotProduct(sphrCen) - normals[2].getDotProduct(verticesOBB[6]) >= radius) return false;
  if (normals[4].getDotProduct(sphrCen) - normals[4].getDotProduct(verticesOBB[6]) >= radius) return false;
  
  return true;
}

const bool BoundsDescriptor::sphereOverlapsAABB(const Tuple4f &t) const
{
  return sphereOverlapsAABB(Tuple3f(t.x, t.y, t.z), t.w);
}

const bool BoundsDescriptor::frustumOverlapsAABB(const Planef *frustumPlane, int planeCount) const
{
  if (!frustumPlane)
    return false;
    
  Tuple3f  halfDiag  = maxEndAABB;
  halfDiag -= centerAABB;
  
  float    m          = 0.0f,
                        n          = 0.0f,
                                     d          = 0.0f;
                                     
  for (int i = 0; i < planeCount; i++)
  {
    const Tuple3f &normal = frustumPlane[i].getNormal();
    d      = frustumPlane[i].getOffset();
    m      = (centerAABB.x * normal.x) + (centerAABB.y * normal.y) +
             (centerAABB.z * normal.z) + d;
    n      = (halfDiag.x * fabs(normal.x)) + (halfDiag.y * fabs(normal.y)) +
             (halfDiag.z * fabs(normal.z));
    if (m + n < 0)
      return false;
  }
  return true;
}

const bool BoundsDescriptor::sphereOverlapsAABB(const Tuple3f &sphrCen, float radius) const
{
  float s, d = 0;
  for (int i=0 ; i<3 ; i++)
  {
    if (sphrCen[i] < minEndAABB[i])
    {
      s = sphrCen[i] - minEndAABB[i];
      d += s*s;
    }
    else
      if (sphrCen[i] > maxEndAABB[i])
      {
        s = sphrCen[i] - maxEndAABB[i];
        d += s*s;
      }
  }
  return d <= radius*radius;
}

const void BoundsDescriptor::render(int info)  const
{
  if (info & OBB)
  {
    if (info & FILL)
    {
      glVertexPointer(3, GL_FLOAT, 0, verticesOBB);
      glEnableClientState(GL_VERTEX_ARRAY);
      glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, indicesFill);
      glDisableClientState(GL_VERTEX_ARRAY);
    }
    else
    {
      glBegin(GL_LINE_STRIP);
      glVertex3fv(verticesOBB[3]);
      glVertex3fv(verticesOBB[0]);
      glVertex3fv(verticesOBB[1]);
      glVertex3fv(verticesOBB[2]);
      glVertex3fv(verticesOBB[3]);
      glVertex3fv(verticesOBB[7]);
      glVertex3fv(verticesOBB[6]);
      glVertex3fv(verticesOBB[5]);
      glVertex3fv(verticesOBB[4]);
      glVertex3fv(verticesOBB[7]);
      glEnd();
      
      glBegin(GL_LINES);
      glVertex3fv(verticesOBB[0]);
      glVertex3fv(verticesOBB[4]);
      glVertex3fv(verticesOBB[1]);
      glVertex3fv(verticesOBB[5]);
      glVertex3fv(verticesOBB[2]);
      glVertex3fv(verticesOBB[6]);
      glEnd();
    }
  }
  else
  {
    glBegin(GL_LINES);
    glVertex3fv(minEndAABB);
    glVertex3f(maxEndAABB.x, minEndAABB.y, minEndAABB.z);
    glVertex3fv(minEndAABB);
    glVertex3f(minEndAABB.x, maxEndAABB.y, minEndAABB.z);
    glVertex3fv(minEndAABB);
    glVertex3f(minEndAABB.x, minEndAABB.y, maxEndAABB.z);
    
    glVertex3fv(maxEndAABB);
    glVertex3f(minEndAABB.x, maxEndAABB.y, maxEndAABB.z);
    glVertex3fv(maxEndAABB);
    glVertex3f(maxEndAABB.x, maxEndAABB.y, minEndAABB.z);
    glVertex3fv(maxEndAABB);
    glVertex3f(maxEndAABB.x, minEndAABB.y, maxEndAABB.z);
    
    glVertex3f(minEndAABB.x, minEndAABB.y, maxEndAABB.z);
    glVertex3f(maxEndAABB.x, minEndAABB.y, maxEndAABB.z);
    
    glVertex3f(minEndAABB.x, maxEndAABB.y, minEndAABB.z);
    glVertex3f(maxEndAABB.x, maxEndAABB.y, minEndAABB.z);
    
    glVertex3f(minEndAABB.x, minEndAABB.y, maxEndAABB.z);
    glVertex3f(minEndAABB.x, maxEndAABB.y, maxEndAABB.z);
    
    glVertex3f(maxEndAABB.x, maxEndAABB.y, minEndAABB.z);
    glVertex3f(maxEndAABB.x, maxEndAABB.y, maxEndAABB.z);
    
    glVertex3f(maxEndAABB.x, minEndAABB.y, minEndAABB.z);
    glVertex3f(maxEndAABB.x, maxEndAABB.y, minEndAABB.z);
    
    glVertex3f(maxEndAABB.x, minEndAABB.y, minEndAABB.z);
    glVertex3f(maxEndAABB.x, minEndAABB.y, maxEndAABB.z);
    
    glVertex3f(minEndAABB.x, maxEndAABB.y, minEndAABB.z);
    glVertex3f(minEndAABB.x, maxEndAABB.y, maxEndAABB.z);
    
    glEnd();
  }
}

void BoundsDescriptor::computeAABBEnds()
{
  minEndAABB.x = maxEndAABB.x = verticesOBB[0].x;
  minEndAABB.y = maxEndAABB.y = verticesOBB[0].y;
  minEndAABB.z = maxEndAABB.z = verticesOBB[0].z;
  
  for (int i=1;i<8;i++)
  {
    if (verticesOBB[i].x < minEndAABB.x) minEndAABB.x = verticesOBB[i].x;
    if (verticesOBB[i].x > maxEndAABB.x) maxEndAABB.x = verticesOBB[i].x;
    if (verticesOBB[i].y < minEndAABB.y) minEndAABB.y = verticesOBB[i].y;
    if (verticesOBB[i].y > maxEndAABB.y) maxEndAABB.y = verticesOBB[i].y;
    if (verticesOBB[i].z < minEndAABB.z) minEndAABB.z = verticesOBB[i].z;
    if (verticesOBB[i].z > maxEndAABB.z) maxEndAABB.z = verticesOBB[i].z;
  }
  centerAABB  = maxEndAABB;
  centerAABB += minEndAABB;
  centerAABB /= 2.0f;
}

const Tuple3f &BoundsDescriptor::getAxis(int index)const
{
  return (index < 0 || index > 2) ? axii[0] : axii[index];
}
const Tuple3f &BoundsDescriptor::getMinEndAABB() const
{
  return minEndAABB;
}
const Tuple3f &BoundsDescriptor::getMaxEndAABB() const
{
  return maxEndAABB;
}
const Tuple3f &BoundsDescriptor::getCenterAABB() const
{
  return centerAABB;
}
const Tuple3f &BoundsDescriptor::getCenterOBB()  const
{
  return centerOBB;
}
const Tuple3f &BoundsDescriptor::getExtents()    const
{
  return extents;
}

const Tuple3f &BoundsDescriptor::getVertexOBB(int index) const
{
  return (index <0 || index > 7) ? verticesOBB[0] : verticesOBB[index];
}