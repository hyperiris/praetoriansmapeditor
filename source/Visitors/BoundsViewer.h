#ifndef BOUNDSVIEWER
#define BOUNDSVIEWER

#include "Visitor.h"

class BoundsViewer : public Visitor
{
  public:
    void visit(SpatialIndexBaseNode* node);
    void visit(SpatialIndexNode* node);
    void visit(SpatialIndexCell* node);
};

#endif