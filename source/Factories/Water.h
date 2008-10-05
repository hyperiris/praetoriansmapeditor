#ifndef WATER_H
#define WATER_H

#include "../Tools/NamedObject.h"
#include "../Math/Tuple4.h"
#include "../Containers/ocarray.h"

class Water : public NamedObject
{
  public:
    Water();
    ~Water();
    
  public:
    void addVertex(const Tuple3f &vertex);
    void removeTriangle(unsigned int index);
    
    unsigned int getVertexCount();
    Tuple3f* getVertices();
    
    unsigned int getTriangleCount();
    Tuple3i* getTriangles();
    
    void setColor(float r, float g, float b, float a);
    void setColor(const Tuple4f& c);
    Tuple4f getColor();
    
    void draw(int info);
    
    enum RenderType
    {
      WIRE = 0x01,
      FILL = 0x02
    };
    
  private:
    void triangulate();
    bool withinRange(const Tuple3f &v, const Tuple3f &a, const Tuple3f &b, const Tuple3f &c, const Tuple3f &d);
    Tuple2f computeCircumCenter(const Tuple3f &a, const Tuple3f &b, const Tuple3f &c, const Tuple3f &d);
    bool hasInfiniteSlope(const Tuple3f &a, const Tuple3f &b);
    Array <Tuple3f> computeSuperTriangle();
    
  private:
    Array <Tuple3f> vertexList;
    Array <Tuple3i> triangleList;
    Tuple4f color;
};

#endif