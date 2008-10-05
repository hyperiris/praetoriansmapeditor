#include "FlagViewer.h"

#include "../Nodes/SpatialIndexNode.h"
#include "../Nodes/SpatialIndexCell.h"

#include "../Tools/NodeIterator.h"
#include "../Tools/RangeIterator.h"

#include "../Controllers/TileController.h"
#include "../Kernel/Gateway.h"

#include "../Factories/FactoryUtils.h"
#include "../Factories/TerrainVisuals.h"
#include "../Factories/TerrainLogic.h"

#include "../Databases/TerrainDatabase.h"

FlagViewer::FlagViewer()
{
  reset();
}

void FlagViewer::setRenderColor(const Tuple4f& color)
{
  std::fill(renderColor, renderColor + 9, color);
}

void FlagViewer::visit(SpatialIndexBaseNode *node)
{
  if (!node->isVisible())
    return;
    
  glPushMatrix();
  glTranslatef(0.0f, 0.01f, 0.0f);
  
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  
  glColorPointer(4, GL_FLOAT, 0, renderColor);
  
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  NodeIterator iter(node->getFirstChild());
  
  while (!iter.end())
  {
    iter.current()->accept(this);
    iter++;
  }
  
  glDisable(GL_BLEND);
  
  glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
  
  glPopMatrix();
}

void FlagViewer::visit(SpatialIndexNode *node)
{
  if (!node->isVisible())
    return;
    
  NodeIterator iter(node->getFirstChild());
  
  while (!iter.end())
  {
    iter.current()->accept(this);
    iter++;
  }
}

void FlagViewer::visit(SpatialIndexCell *node)
{
  if (!node->isVisible())
    return;
    
  TerrainDatabase* tdatabase;
  TerrainLogic* tlogic;
  TerrainVisuals* visuals;
  RangeIterator iter;
  
  visuals = Gateway::getTerrainVisuals();
  tdatabase = Gateway::getTerrainDatabase();
  tlogic = Gateway::getTerrainLogic();
  Tuple2i area(visuals->getArea());
  iter.set(area.x, node->getRange());
  
  while (!iter.end())
  {
    if (tlogic->getFlags(iter.current()) & logic)
    {
      glVertexPointer(3, GL_FLOAT, 0, tdatabase->getController(iter.current())->getVertices());
      glDrawElements(GL_TRIANGLE_FAN, 10, GL_UNSIGNED_SHORT, visuals->getTileIndices(0));
    }
    
    iter++;
  }
}

void FlagViewer::setLogic(unsigned int value)
{
  logic = value;
}

unsigned int FlagViewer::getLogic()
{
  return logic;
}

void FlagViewer::setMode(unsigned int value)
{
  mode = value;
}

unsigned int FlagViewer::getMode()
{
  return mode;
}

void FlagViewer::reset()
{
  logic = TileFlags::TILE_FLAT;
  mode = 2;
}

FlagViewer::~FlagViewer()
{}
