#include "TileGraphNode.h"
#include "../Visitors/Visitor.h"

TileGraphNode::TileGraphNode()
{
}

void TileGraphNode::accept(Visitor* visitor)
{
  visitor->visit(this);
}

void TileGraphNode::setController(TileController *contr)
{
  controller = contr;
}

TileController* TileGraphNode::getController()
{
  return controller;
}

TileGraphNode::~TileGraphNode()
{
}
