#include "ModelController.h"
#include "../Nodes/TransformGroup.h"

ModelController::ModelController()
{
  tgNode.setController(this);
}

ModelController::ModelController(TransformGroup* group)
{
  controlledGroup = group;
  tgNode.setController(this);
}

TransformGroup* ModelController::getTransformGroup()
{
  return controlledGroup;
}

TransformGroupNode* ModelController::getTGNode()
{
  return &tgNode;
}

void ModelController::translateModel(float x, float y, float z)
{
  controlledGroup->getTransform().setTranslations(x, y, z);
  controlledGroup->updateBoundsDescriptor();
}

Tuple3f ModelController::getPosition()
{
  const Matrix4f* m = &controlledGroup->getTransform().getMatrix4f();
  return Tuple3f(m->m[12], m->m[13], m->m[14]);
}

ModelController::~ModelController()
{
}