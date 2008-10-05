#ifndef SPATIALINDEXCELL
#define SPATIALINDEXCELL

#include "SpatialIndexBaseNode.h"

class SpatialIndexCell : public SpatialIndexBaseNode
{
  public:
    SpatialIndexCell();
    ~SpatialIndexCell();
    
    void accept(Visitor* visitor);
};

#endif