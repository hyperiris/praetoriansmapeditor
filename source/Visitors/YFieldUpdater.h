#ifndef YFIELDUPDATER
#define YFIELDUPDATER

#include "Visitor.h"

class YFieldUpdater : public Visitor
{
  public:
    YFieldUpdater();
    
    void visit(SpatialIndexCell *cell);
    void visit(TransformGroupNode *node);
};

#endif