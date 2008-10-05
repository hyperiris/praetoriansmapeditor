#include "TransformGroupNode.h"
#include "../Visitors/Visitor.h"

TransformGroupNode::TransformGroupNode()
{
}

void TransformGroupNode::accept(Visitor* visitor)
{
  visitor->visit(this);
}

void TransformGroupNode::setController(ModelController *controller)
{
  modelController = controller;
}

ModelController* TransformGroupNode::getController()
{
  return modelController;
}

TransformGroupNode::~TransformGroupNode()
{
}