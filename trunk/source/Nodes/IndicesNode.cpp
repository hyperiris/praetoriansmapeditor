#include "IndicesNode.h"
#include "../Visitors/Visitor.h"

void IndicesNode::accept(Visitor* visitor)
{
  visitor->visit(this);
}

void IndicesNode::setIndices(unsigned int *idx)
{
  indices = idx;
}

unsigned int* IndicesNode::getIndices()
{
  return indices;
}