#ifndef SPATIALINDEXNODE
#define SPATIALINDEXNODE

#include "SpatialIndexBaseNode.h"

class SpatialIndexNode : public SpatialIndexBaseNode
{
  public:
    SpatialIndexNode();
    ~SpatialIndexNode();
    
    void accept(Visitor* visitor);
};

#endif