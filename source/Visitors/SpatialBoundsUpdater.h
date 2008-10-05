#ifndef SPATIALBOUNDSUPDATER
#define SPATIALBOUNDSUPDATER

#include "Visitor.h"
#include "../Math/Tuple2.h"

class SpatialBoundsUpdater : public Visitor
{
  public:
    SpatialBoundsUpdater();
    ~SpatialBoundsUpdater();
    
  public:
    void visit(SpatialIndexBaseNode  *node);
    void visit(SpatialIndexNode *node);
    void visit(SpatialIndexCell *node);
    
    void reset();
    
  private:
    Tuple2f newBounds;
};

#endif