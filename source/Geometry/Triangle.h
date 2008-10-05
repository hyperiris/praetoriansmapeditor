#ifndef TRIANGLE_H
#define TRIANGLE_H
#include "../Math/MathUtils.h"
#include "Plane.h"

class Triangle
{
  public:
    Triangle(Tuple3f *t1 = NULL,
             Tuple3f *t2 = NULL,
             Tuple3f *t3 = NULL);
             
    void setFacingPoint(const Tuple3f &point);
    void addToNormals();
    void resetNormals();
    
    void setVertices(Tuple3f *t1,
                     Tuple3f *t2,
                     Tuple3f *t3);
                     
    void setNormals(Tuple3f *n1,
                    Tuple3f *n2,
                    Tuple3f *n3);
                    
    Tuple3f *vertices[3],
    *normals[3];
    Planef   plane;
    bool     facingLight;
    
};

#endif
