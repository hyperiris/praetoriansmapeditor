#ifndef EDGE_H
#define EDGE_H

#include "../Math/Tuple2.h"

class Edge
{
  public:
    Edge() {}
    Edge(int a, int b)
    {
      e.x=a;
      e.y=b;
    }
    Edge(const Edge &t)
    {
      e=t.e;
    }
    Edge& operator = (const Edge &t)
    {
      e=t.e;
      return *this;
    }
    bool operator == (const Edge &t)
    {
      return (e==t.e) || (e.x==t.e.y&&e.y==t.e.x);
    }
    Tuple2i e;
};

#endif