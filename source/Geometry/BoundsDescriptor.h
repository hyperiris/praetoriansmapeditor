#ifndef BOUNDSDESCRIPTOR_H
#define BOUNDSDESCRIPTOR_H

#include "../Math/Matrix4.h"
#include "Plane.h"


class BoundsDescriptor
{
  public:
    BoundsDescriptor();
    BoundsDescriptor(const BoundsDescriptor &copy);
    BoundsDescriptor &operator =(const BoundsDescriptor &copy);
    
    void operator*=(const Matrix4f &matrix);
    void operator+=(const BoundsDescriptor &copy);
    
    BoundsDescriptor  operator *(const Matrix4f &matrix);
    BoundsDescriptor  operator +(const BoundsDescriptor &copy);
    
    const bool  frustumOverlapsAABB(const Planef *frustumPlane, int planeCount) const;
    const bool  sphereOverlapsAABB(const Tuple3f &center, float radius)   const;
    const bool  sphereOverlapsAABB(const Tuple4f &centerAndRad)           const;
    const bool  sphereOverlapsOBB(const Tuple3f &center, float radius)    const;
    const bool  sphereOverlapsOBB(const Tuple4f &centerAndRad)            const;
    const bool  lineOverlapsOBB(const Tuple3f& start, const Tuple3f &end) const;
    bool  isInitialized();
    
    const void  render(int info) const;
    void  reset();
    
    void  computeBounds(const Tuple3f &minEnd, const Tuple3f &maxEnd);
    void  computeBounds(float minX, float minY, float minZ,
                        float maxX, float maxY, float maxZ);
                        
    const Tuple3f &getMinEndAABB()         const;
    const Tuple3f &getMaxEndAABB()         const;
    const Tuple3f &getCenterAABB()         const;
    const Tuple3f &getVertexOBB(int index) const;
    const Tuple3f &getCenterOBB()          const;
    const Tuple3f &getExtents()            const;
    const Tuple3f &getAxis(int index)      const;
    
    enum BRenderType
    {
      WIRE = 0x01,
      FILL = 0x02,
      AABB = 0x04,
      OBB  = 0x08
    };
    
  private:
    static unsigned short indicesFill[36];
    
    void  computeAxiiAndExtents();
    void  computeAABBEnds();
    
    Tuple3f verticesOBB[8],
    minEndAABB,
    maxEndAABB,
    centerAABB,
    centerOBB,
    extents,
    axii[3];
    
    bool    initialized;
    int     type;
};

#endif