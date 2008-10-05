#include "BoundsViewer.h"
#include "../Nodes/SpatialIndexNode.h"
#include "../Nodes/SpatialIndexCell.h"
#include "../Tools/NodeIterator.h"
#include "../Tools/Logger.h"

void BoundsViewer::visit(SpatialIndexBaseNode *node)
{
  if (!node->isVisible())
    return;
    
  glColor3f(0, 0, 1);
  node->getBoundsDescriptor().render(BoundsDescriptor::AABB | BoundsDescriptor::WIRE);
  
  NodeIterator iter(node->getFirstChild());
  while (!iter.end())
  {
    iter.current()->accept(this);
    iter++;
  }
}

void BoundsViewer::visit(SpatialIndexNode *node)
{
  if (!node->isVisible())
    return;
    
  glColor3f(1, 0, 0);
  node->getBoundsDescriptor().render(BoundsDescriptor::AABB | BoundsDescriptor::WIRE);
  
  NodeIterator iter(node->getFirstChild());
  while (!iter.end())
  {
    iter.current()->accept(this);
    iter++;
  }
}

void BoundsViewer::visit(SpatialIndexCell *node)
{
  if (!node->isVisible())
    return;
    
  glColor3f(0, 1, 0);
  node->getBoundsDescriptor().render(BoundsDescriptor::AABB | BoundsDescriptor::WIRE);
}
