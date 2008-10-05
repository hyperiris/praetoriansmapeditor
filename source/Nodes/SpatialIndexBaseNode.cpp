#include "SpatialIndexBaseNode.h"
#include "../Visitors/Visitor.h"

SpatialIndexBaseNode::SpatialIndexBaseNode()
{
  treelevel = 0;
  visibility = false;
}

void SpatialIndexBaseNode::reset()
{
  Node::reset();
  boundsDescriptor.reset();
  treelevel = 0;
  visibility = false;
  indexrange.set(0);
}

void SpatialIndexBaseNode::accept(Visitor* visitor)
{
  visitor->visit(this);
}

void SpatialIndexBaseNode::setBoundsDescriptor(const BoundsDescriptor &descriptor)
{
  boundsDescriptor = descriptor;
}

void SpatialIndexBaseNode::updateBounds(const BoundsDescriptor &descriptor)
{
  boundsDescriptor = descriptor;
}

void SpatialIndexBaseNode::setRange(const Tuple4i &range)
{
  indexrange = range;
}

void SpatialIndexBaseNode::setLevel(unsigned int level)
{
  treelevel = level;
}

unsigned int SpatialIndexBaseNode::getLevel()
{
  return treelevel;
}

BoundsDescriptor &SpatialIndexBaseNode::getBoundsDescriptor(void)
{
  return boundsDescriptor;
}

Tuple4i SpatialIndexBaseNode::getRange()
{
  return indexrange;
}

void SpatialIndexBaseNode::setVisibility(bool value)
{
  visibility = value;
}

bool SpatialIndexBaseNode::isVisible()
{
  return visibility;
}

SpatialIndexBaseNode::~SpatialIndexBaseNode()
{
}