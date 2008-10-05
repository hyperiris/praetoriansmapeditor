#ifndef DEBUGVIEWER
#define DEBUGVIEWER

#include "Visitor.h"

class DebugViewer : public Visitor
{
  public:
    void visit(SpatialIndexBaseNode*  node);
    void visit(SpatialIndexNode*      node);
    void visit(SpatialIndexCell*      node);
};

#endif