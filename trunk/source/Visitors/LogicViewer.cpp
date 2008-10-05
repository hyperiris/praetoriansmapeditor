#include "LogicViewer.h"

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

LogicViewer::LogicViewer()
{
  reset();
}

void LogicViewer::setRenderColor(const Tuple4f& color)
{
  std::fill(renderColor, renderColor + 9, color);
}

void LogicViewer::visit(SpatialIndexBaseNode *node)
{
  if (!node->isVisible())
    return;
    
  glPushMatrix();
  glTranslatef(0.0f, 0.005f, 0.0f);
  
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

void LogicViewer::visit(SpatialIndexNode *node)
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

void LogicViewer::visit(SpatialIndexCell *node)
{
  if (!node->isVisible())
    return;
    
  TerrainDatabase* tdatabase;
  TerrainLogic* tlogic;
  TerrainVisuals* visuals;
  RangeIterator iter;
  
  visuals = Gateway::getTerrainVisuals();
  tlogic = Gateway::getTerrainLogic();
  tdatabase = Gateway::getTerrainDatabase();
  Tuple2i area(visuals->getArea());
  iter.set(area.x, node->getRange());
  
  while (!iter.end())
  {
    if ((tlogic->getFlags(iter.current()) >> 27) == logic)
    {
      glVertexPointer(3, GL_FLOAT, 0, tdatabase->getController(iter.current())->getVertices());
      glDrawElements(GL_TRIANGLE_FAN, 10, GL_UNSIGNED_SHORT, visuals->getTileIndices(0));
    }
    
    //if (controller->hasGrass())
    //{
    //  glVertexPointer(3, GL_FLOAT, 0, controller->getVertices());
    //  glColorPointer(4, GL_UNSIGNED_BYTE, 0, grasscolor);
    //  glDrawElements(GL_TRIANGLE_FAN, 10, GL_UNSIGNED_INT, indices);
    //}
    
    //if (controller->isBoundary())
    //{
    //  glVertexPointer(3, GL_FLOAT, 0, controller->getVertices());
    //  glColorPointer(4, GL_UNSIGNED_BYTE, 0, wirecolor);
    //  glDrawElements(GL_TRIANGLE_FAN, 10, GL_UNSIGNED_INT, indices);
    //}
    
    iter++;
  }
}

void LogicViewer::setLogic(unsigned int value)
{
  logic = value;
}

unsigned int LogicViewer::getLogic()
{
  return logic;
}

void LogicViewer::setMode(unsigned int value)
{
  mode = value;
}

unsigned int LogicViewer::getMode()
{
  return mode;
}

void LogicViewer::reset()
{
  logic = TileTypes::UNUSED;
  mode = 2;
}

LogicViewer::~LogicViewer()
{
}
