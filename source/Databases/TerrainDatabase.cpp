#include "TerrainDatabase.h"
#include "../Factories/TerrainTextures.h"
#include "../Factories/TerrainVisuals.h"
#include "../Factories/TerrainLogic.h"
#include "../Factories/Tile.h"
#include "../Kernel/Gateway.h"
#include "../Controllers/TileController.h"
#include "../Visitors/Visitor.h"
#include "../Nodes/TileGraphNode.h"
#include "../Graphs/TileGraph.h"

TerrainDatabase::TerrainDatabase() : tileControllers(300*300)
{
  tileGraph = new TileGraph();
}

bool TerrainDatabase::initialize()
{
  return true;
}

void TerrainDatabase::inspectGraph(unsigned int level, Visitor *visitor)
{
  tileGraph->inspect(level, visitor);
}

void TerrainDatabase::compile()
{
  setupGraph();
  finalize();
}

void TerrainDatabase::setupGraph()
{
  TileController *controller;
  TileGraphNode *graphnode;
  Tuple2f *coords;
  Tuple2i area;
  Tile *tile;
  unsigned int txPageID;
  int priid;
  int secid;
  
  TerrainVisuals* visuals = Gateway::getTerrainVisuals();
  TerrainTextures* textures = Gateway::getTerrainTextures();
  
  tileGraph->initialize(2);
  
  for (unsigned int i = 0; i < textures->getTextureCount(); i++)
  {
    txPageID = textures->getTextureID(i);
    
    for (unsigned int j = 0; j < 16; j++)
    {
      coords = textures->getTextureCoords(j);
      tileGraph->integrate(0, txPageID, coords);
      tileGraph->integrate(1, txPageID, coords);
    }
  }
  
  area = visuals->getArea();
  
  for (int j = 0; j < area.x * area.y; j++)
  {
    tile = visuals->getTile(j);
    priid = tile->getPrimaryTextureID();
    secid = tile->getSecondaryTextureID();
    
    controller = new TileController();
    controller->setTile(tile);
    
    graphnode = tileGraph->addModelNode(0, priid);
    graphnode->setController(controller);
    controller->addGraphNode(graphnode);
    
    graphnode = tileGraph->addModelNode(1, secid);
    graphnode->setController(controller);
    controller->addGraphNode(graphnode);
    
    tileControllers.append(controller);
  }
}

void TerrainDatabase::finalize()
{
  //TerrainVisuals* visuals;
  //TerrainLogic* logic;
  //Tuple2i area;
  //
  //visuals = Gateway::getTerrainVisuals();
  //logic = Gateway::getTerrainLogic();
  //area = visuals->getArea();
  //
  //if (logic->getFlagCount() == area.x * area.y)
  //  for (int i = 0; i < area.x * area.y; i++)
  //    tileControllers(i)->setLogic(logic->getFlags(i));
}

void TerrainDatabase::controllerVisibility(unsigned int index, bool visible)
{
  tileControllers(index)->setVisible(visible);
}

TileController* TerrainDatabase::getController(unsigned int index)
{
  if (index >= tileControllers.length())
    return 0;
  return tileControllers(index);
}

unsigned int TerrainDatabase::getControllerCount()
{
  return tileControllers.length();
}

void TerrainDatabase::changeTileTexture(unsigned int level, int index, unsigned int contrIndex, bool visible)
{
  TileController* controller = tileControllers(contrIndex);
  tileGraph->moveModelNode(controller->getGraphNode(level), level, index, visible);
  controller->setTextureID(level, index);
}

void TerrainDatabase::clearColorMap()
{
  Tuple4ub *cols;
  for (size_t i = 0; i < tileControllers.length(); i++)
  {
    cols = tileControllers(i)->getColors();
    for (unsigned int c = 0; c < 9; c++)
    {
      cols[c].x = char(255);
      cols[c].y = char(255);
      cols[c].z = char(255);
    }
  }
}

void TerrainDatabase::clearAlphaMap()
{
  Tuple4ub *cols;
  for (size_t i = 0; i < tileControllers.length(); i++)
  {
    cols = tileControllers(i)->getColors();
    for (unsigned int c = 0; c < 9; c++)
      cols[c].w = char(255);
  }
}

void TerrainDatabase::reset()
{
  tileGraph->reset();
  tileControllers.clearAndDestroy();
}

TerrainDatabase::~TerrainDatabase()
{
  deleteObject(tileGraph);
  tileControllers.clearAndDestroy();
}
