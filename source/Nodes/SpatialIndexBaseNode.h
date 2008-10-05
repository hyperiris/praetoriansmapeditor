#ifndef SPATIALINDEXBASENODE
#define SPATIALINDEXBASENODE

#include "Node.h"
#include "../Geometry/BoundsDescriptor.h"

class SpatialIndexBaseNode : public Node
{
  public:
    SpatialIndexBaseNode();
    virtual ~SpatialIndexBaseNode();
    
  public:
    void accept(Visitor* visitor);
    
    void setBoundsDescriptor(const BoundsDescriptor &descriptor);
    void updateBounds(const BoundsDescriptor &descriptor);
    BoundsDescriptor &getBoundsDescriptor(void);
    
    void setRange(const Tuple4i &range);
    Tuple4i getRange();
    void setLevel(unsigned int level);
    unsigned int getLevel();
    
    void setVisibility(bool value);
    bool isVisible();
    
    void reset();
    
  protected:
    BoundsDescriptor  boundsDescriptor;
    Tuple4i indexrange;
    unsigned int treelevel;
    bool visibility;
};

#endif