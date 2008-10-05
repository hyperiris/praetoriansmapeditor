#include "Meadow.h"

Meadow::Meadow(const char* strname) : NamedObject(strname)
{
}

void Meadow::addPatch(GrassPatch* patch)
{
  patchesList.append(patch);
  //updateBounds();
}

void Meadow::setBounds(const Tuple4i& bounds)
{
  boundary = bounds;
}

Tuple4i Meadow::getBounds()
{
  return boundary;
}

GrassPatch* Meadow::getPatch(unsigned int index)
{
  return patchesList(index);
}

unsigned int Meadow::getPatchCount()
{
  return patchesList.length();
}

void Meadow::updateBounds()
{
  if (patchesList.isEmpty())
    return;
    
  Tuple4i bounds(1000, 1000, -1000, -1000);
  GrassPatch* patch;
  
  bounds.x = patchesList(0)->position.x;
  bounds.y = patchesList(0)->position.y;
  bounds.z = bounds.x;
  bounds.w = bounds.y;
  for (unsigned int i = 0; i < patchesList.length(); i++)
  {
    patch = patchesList(i);
    bounds.x = bounds.x < patch->position.x ? bounds.x : patch->position.x;
    bounds.y = bounds.y < patch->position.y ? bounds.y : patch->position.y;
    bounds.z = bounds.z > patch->position.x ? bounds.z : patch->position.x;
    bounds.w = bounds.w > patch->position.y ? bounds.w : patch->position.y;
  }
  
  bounds.z = 1 + bounds.z - bounds.x;
  bounds.w = 1 + bounds.w - bounds.y;
  
  boundary = bounds;
}

void Meadow::addTileIndex(unsigned int index)
{
  tileIdentifierList.append(index);
}

bool Meadow::containsTileIndex(unsigned int index)
{
  return tileIdentifierList.contains(index);
}

void Meadow::clearPatches()
{
  patchesList.clearAndDestroy();
  tileIdentifierList.clear();
}

Meadow::~Meadow()
{
  patchesList.clearAndDestroy();
  tileIdentifierList.clear();
}