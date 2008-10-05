#include "TileRevealer.h"
#include "../Nodes/SpatialIndexNode.h"
#include "../Nodes/SpatialIndexCell.h"
#include "../Tools/NodeIterator.h"
#include "../Kernel/Gateway.h"
#include "../Viewer/Frustum.h"
#include "../Graphs/SpatialIndex.h"
#include "../Databases/TerrainDatabase.h"
#include "../Factories/TerrainVisuals.h"
#include "../Renderer/Renderer.h"

TileRevealer::TileRevealer()
{
  reset();
}

void TileRevealer::setFrustum(Frustum *frus)
{
  frustum = frus;
}

void TileRevealer::setCamera(Camera *cam)
{
  camera = cam;
}

void TileRevealer::setRadius(float r)
{
  radius = r;
}

void TileRevealer::visit(SpatialIndexBaseNode* node)
{
  if (frustum->AABBInFrustum(node->getBoundsDescriptor()))
  {
    node->setVisibility(true);
    
    eyePos = camera->getViewerPosition();
    
    NodeIterator iter(node->getFirstChild());
    while (!iter.end())
    {
      iter.current()->accept(this);
      iter++;
    }
  }
  else
  {
    cellVisibility(node->getRange(), false);
    node->setVisibility(false);
  }
}

void TileRevealer::visit(SpatialIndexNode* node)
{
  if (frustum->AABBInFrustum(node->getBoundsDescriptor()))
  {
    node->setVisibility(true);
    
    NodeIterator iter(node->getFirstChild());
    while (!iter.end())
    {
      iter.current()->accept(this);
      iter++;
    }
  }
  else
  {
    cellVisibility(node->getRange(), false);
    node->setVisibility(false);
  }
}

void TileRevealer::visit(SpatialIndexCell* node)
{
  bool aabbinfrustum = frustum->AABBInFrustum(node->getBoundsDescriptor());
  aabbinfrustum &= (node->getBoundsDescriptor().getCenterAABB().getDistance(eyePos) < radius);
  
  tileVisibility(node->getRange(), aabbinfrustum);
  node->setVisibility(aabbinfrustum);
}

void TileRevealer::cellVisibility(const Tuple4i& range, bool visibility)
{
  SpatialIndexBaseNode *cell;
  SpatialIndex* spatialIndex;
  TerrainVisuals* visuals;
  Tuple2i siarea;
  Tuple2i tarea;
  Tuple4i carea;
  
  spatialIndex = Gateway::getSpatialIndex();
  visuals = Gateway::getTerrainVisuals();
  
  siarea = spatialIndex->getArea();
  tarea = visuals->getArea();
  
  for (int y = range.y; y <= range.w; y++)
    for (int x = range.x; x <= range.z; x++)
    {
      cell = spatialIndex->getCell(y * siarea.x + x);
      if (cell->isVisible() == visibility)
        continue;
        
      tileVisibility(cell->getRange(), visibility);
      cell->setVisibility(visibility);
    }
}

void TileRevealer::tileVisibility(const Tuple4i& range, bool visibility)
{
  TerrainVisuals* visuals;
  TerrainDatabase* terrainDatabase;
  Tuple2i tarea;
  
  terrainDatabase = Gateway::getTerrainDatabase();
  visuals = Gateway::getTerrainVisuals();
  tarea = visuals->getArea();
  
  for (int y = range.y; y <= range.w; y++)
    for (int x = range.x; x <= range.z; x++)
      terrainDatabase->controllerVisibility(y * tarea.x + x, visibility);
}

void TileRevealer::reset()
{
  radius = 50.0f;
  frustum = 0;
  camera = 0;
}

TileRevealer::~TileRevealer()
{
}
