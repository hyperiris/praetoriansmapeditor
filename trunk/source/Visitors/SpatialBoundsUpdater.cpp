#include "SpatialBoundsUpdater.h"
#include "../Nodes/SpatialIndexNode.h"
#include "../Nodes/SpatialIndexCell.h"
#include "../Tools/NodeIterator.h"
#include "../Tools/RangeIterator.h"
#include "../Controllers/TileController.h"
#include "../Kernel/Gateway.h"
#include "../Factories/TerrainVisuals.h"
#include "../Databases/TerrainDatabase.h"

SpatialBoundsUpdater::SpatialBoundsUpdater()
{
  reset();
}

void SpatialBoundsUpdater::visit(SpatialIndexBaseNode *node)
{
  BoundsDescriptor *bounds;
  Tuple3f minCellHeight;
  Tuple3f maxCellHeight;
  
  bounds = &node->getBoundsDescriptor();
  
  minCellHeight = node->getBoundsDescriptor().getMinEndAABB();
  maxCellHeight = node->getBoundsDescriptor().getMaxEndAABB();
  
  if (newBounds[1] > maxCellHeight.y)
    maxCellHeight.y = newBounds[1];
    
  if (newBounds[0] < minCellHeight.y)
    minCellHeight.y = newBounds[0];
    
  bounds->computeBounds(minCellHeight, maxCellHeight);
}

void SpatialBoundsUpdater::visit(SpatialIndexNode *node)
{
  BoundsDescriptor *bounds;
  Tuple3f minCellHeight;
  Tuple3f maxCellHeight;
  
  bounds = &node->getBoundsDescriptor();
  
  minCellHeight = bounds->getMinEndAABB();
  maxCellHeight = bounds->getMaxEndAABB();
  
  if (newBounds[1] > maxCellHeight.y)
    maxCellHeight.y = newBounds[1];
    
  if (newBounds[0] < minCellHeight.y)
    minCellHeight.y = newBounds[0];
    
  bounds->computeBounds(minCellHeight, maxCellHeight);
  
  node->getParentNode()->accept(this);
}

void SpatialBoundsUpdater::visit(SpatialIndexCell *node)
{
  TileController *controller;
  Tuple3f *vertices;
  Tuple3f minCellHeight;
  Tuple3f maxCellHeight;
  BoundsDescriptor *bounds;
  float element;
  RangeIterator iter;
  
  TerrainVisuals* visuals = Gateway::getTerrainVisuals();
  TerrainDatabase* tdatabase = Gateway::getTerrainDatabase();
  
  bounds = &node->getBoundsDescriptor();
  
  minCellHeight = bounds->getMinEndAABB();
  maxCellHeight = bounds->getMaxEndAABB();
  
  iter.set(visuals->getArea().x, node->getRange());
  
  while (!iter.end())
  {
    controller = tdatabase->getController(iter.current());
    vertices = controller->getVertices();
    
    for (int i = 0; i < 9; i++)
    {
      element = vertices[i].y;
      newBounds[1] = newBounds[1] > element ? newBounds[1] : element;
      newBounds[0] = newBounds[0] < element ? newBounds[0] : element;
    }
    
    iter++;
  }
  
  maxCellHeight.y = newBounds[1];
  minCellHeight.y = newBounds[0];
  
  bounds->computeBounds(minCellHeight, maxCellHeight);
  
  node->getParentNode()->accept(this);
}

void SpatialBoundsUpdater::reset()
{
  newBounds.set(1000.0f, -1000.0f);
}

SpatialBoundsUpdater::~SpatialBoundsUpdater()
{}