#include "TileController.h"
#include "../Factories/Tile.h"
#include "../Nodes/TileGraphNode.h"

TileController::TileController()
{
  reset();
}

void TileController::setTile(Tile* tilemodel)
{
  tile = tilemodel;
}

void TileController::setTextureID(unsigned int index, int id)
{
  tile->textureID[index] = id;
}

void TileController::addGraphNode(TileGraphNode* node)
{
  graphNodes.append(node);
}

TileGraphNode* TileController::getGraphNode(unsigned int index)
{
  return graphNodes(index);
}

void TileController::reset()
{
  tile = 0;
  visible = false;
  graphNodes.clear();
}

Tuple3f* TileController::getVertices()
{
  return tile->vertices;
}

Tuple4ub* TileController::getColors()
{
  return tile->colors;
}

void TileController::setVisible(bool vis)
{
  unsigned int length = graphNodes.length();
  
  if (vis)
  {
    for (unsigned int i = 0; i < length; i++)
      graphNodes(i)->reveal();
  }
  else
  {
    for (unsigned int j = 0; j < length; j++)
      graphNodes(j)->hide();
  }
  
  visible = vis;
}

void TileController::setFlags(unsigned short flags)
{
  tile->flags ^= flags;
}

unsigned short TileController::getFlags()
{
  return tile->flags;
}

void TileController::clearFlags()
{
  tile->flags = 0;
}

void TileController::setGrass(bool value)
{
  tile->grass = value;
}

void TileController::setBoundary(bool value)
{
  tile->boundary = value;
}

bool TileController::isBoundary()
{
  return tile->boundary;
}

bool TileController::isVisible()
{
  return visible;
}

bool TileController::hasGrass()
{
  return tile->grass;
}

TileController::~TileController()
{
}
