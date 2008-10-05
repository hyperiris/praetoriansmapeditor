#ifndef PLANE_H
#define PLANE_H

#include "../Math/Tuple3.h"

template <class T>
class Plane
{
  public:
    Plane()
    {
      normal.set(0,0,0);
      offset = 0;
    }
    
    Plane(const Plane &copy)
    {
      operator=(copy);
    }
    
    Plane &operator= (const Plane &copy)
    {
      normal = copy.normal;
      offset = copy.offset
               return *this;
    }
    
    void set(const Tuple3<T> &p1,
             const Tuple3<T> &p2,
             const Tuple3<T> &p3)
    {
      static Tuple3<T> v1, v2;
      v1 = p2;
      v2 = p3;
      
      v1 -= p1;
      v2 -= p1;
      normal.crossProduct(v1, v2);
      normal.normalize();
      offset = -normal.getDotProduct(p1);
    }
    
    void replaceOffset(const Tuple3<T> &p1)
    {
      offset = -normal.getDotProduct(p1);
    }
    
    const Tuple3<T> &getNormal() const
    {
      return normal;
    }
    const float      getOffset() const
    {
      return offset;
    }
    
  private:
    Tuple3<T> normal;
    T         offset;
};

typedef Plane<int>    Planei;
typedef Plane<float>  Planef;
typedef Plane<double> Planed;

#endif