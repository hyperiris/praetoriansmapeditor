#include "NatureModelController.h"

NatureModelController::NatureModelController()
{
  windFactor = 0.79f;
}

NatureModelController::NatureModelController(TransformGroup* group) : ModelController(group)
{
  windFactor = 0.79f;
}

void NatureModelController::setWindFactor(float factor)
{
  windFactor = factor;
}

float NatureModelController::getWindFactor()
{
  return windFactor;
}

NatureModelController::~NatureModelController()
{
}