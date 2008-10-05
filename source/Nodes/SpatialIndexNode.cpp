#include "SpatialIndexNode.h"
#include "../Visitors/Visitor.h"

SpatialIndexNode::SpatialIndexNode()
{}

void SpatialIndexNode::accept(Visitor* visitor)
{
  visitor->visit(this);
}

SpatialIndexNode::~SpatialIndexNode()
{}