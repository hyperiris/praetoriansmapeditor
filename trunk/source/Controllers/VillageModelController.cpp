#include "VillageModelController.h"

VillageModelController::VillageModelController()
{
  population = 400;
  maxpopulation = 400;
}

VillageModelController::VillageModelController(TransformGroup* group) : ModelController(group)
{
  population = 400;
  maxpopulation = 400;
}

void VillageModelController::setPopulation(unsigned int n)
{
  population = n;
}

void VillageModelController::setMaxPopulation(unsigned int n)
{
  maxpopulation = n;
}

unsigned int VillageModelController::getPopulation()
{
  return population;
}

unsigned int VillageModelController::getMaxPopulation()
{
  return maxpopulation;
}

VillageModelController::~VillageModelController()
{
}