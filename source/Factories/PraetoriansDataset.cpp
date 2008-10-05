#include "PraetoriansDataset.h"
#include "PraetoriansTerrainTextures.h"
#include "PraetoriansTerrainPasture.h"
#include "PraetoriansTerrainWater.h"
#include "PraetoriansTerrainVisuals.h"
#include "PraetoriansTerrainLogic.h"
#include "PraetoriansWorldVisuals.h"
#include "PraetoriansMissionScript.h"
#include "../Tools/Logger.h"
#include "../Kernel/Gateway.h"

PraetoriansDataset::PraetoriansDataset()
{}

bool PraetoriansDataset::initialize()
{
  terrainTextures = new PraetoriansTerrainTextures();
  terrainVisuals = new PraetoriansTerrainVisuals();
  terrainLogic = new PraetoriansTerrainLogic();
  terrainPasture = new PraetoriansTerrainPasture();
  terrainWater = new PraetoriansTerrainWater();
  worldVisuals = new PraetoriansWorldVisuals();
  missionScript = new PraetoriansMissionScript();
  return true;
}

bool PraetoriansDataset::exportData(const char* projectName)
{
  bool succArray[12] = {false};
  succArray[0] = terrainVisuals->exportData(projectName);
  succArray[1] = terrainLogic->exportData(projectName);
  succArray[2] = terrainPasture->exportData(projectName);
  succArray[3] = missionScript->exportData(projectName);
  succArray[4] = worldVisuals->exportData(projectName);
  succArray[5] = terrainTextures->exportData(projectName);
  succArray[6] = terrainWater->exportData(projectName);
  return (succArray[0] && succArray[1] && succArray[2] && succArray[3] && succArray[4] && succArray[5] && succArray[6]);
}

bool PraetoriansDataset::load(const MapDescriptor& descriptor)
{
  ///terrain textures
  TerrainTextures* textures = Gateway::getTerrainTextures();
  if (!terrainTextures->load(descriptor.mapTextures))
    return Logger::writeErrorLog("Could not load terrain textures");
    
  ///terrain visuals
  TerrainVisuals* visuals = Gateway::getTerrainVisuals();
  if (descriptor.mapVisual && !descriptor.mapColors)
  {
    if (!terrainVisuals->loadVisuals(descriptor.mapVisual))
      return Logger::writeErrorLog("Could not create terrain visuals");
  }
  else if (descriptor.mapVisual && descriptor.mapColors)
  {
    if (!terrainVisuals->loadVisuals(descriptor.mapVisual))
      return Logger::writeErrorLog("Could not create height map");
      
    if (!terrainVisuals->loadColormap(descriptor.mapColors))
      return Logger::writeErrorLog("Could not create color map");
  }
  else if (descriptor.mapDimensions.x && descriptor.mapDimensions.y)
  {
    if (!terrainVisuals->createHeightMap(descriptor.mapDimensions.x, descriptor.mapDimensions.y))
      return Logger::writeErrorLog("Could not create height map");
  }
  else
    return Logger::writeErrorLog("Could not load terrain visuals");
    
  ///terrain logic
  TerrainLogic* logic = Gateway::getTerrainLogic();
  if (descriptor.mapLogic)
  {
    if (!logic->load(descriptor.mapLogic))
      return Logger::writeErrorLog("Could not load terrain logic");
  }
  else
    if (!logic->createLogicMap(visuals->getArea().x, visuals->getArea().y))
      return Logger::writeErrorLog("Could not create terrain logic map");
      
  ///world visuals
  WorldVisuals* worldVisuals = Gateway::getWorldVisuals();
  if (descriptor.mapObjects)
    if (!worldVisuals->load(descriptor.mapObjects))
      return Logger::writeErrorLog("Could not load world visuals");
      
  ///terrain pasture
  TerrainPasture* terrainPasture = Gateway::getTerrainPasture();
  if (descriptor.mapMeadows)
  {
    if (!terrainPasture->load(descriptor.mapMeadows))
      return Logger::writeErrorLog("Could not load terrain pasture");
  }
  terrainPasture->setName(descriptor.mapPastureType);
  
  ///terrain water
  TerrainWater* terrainWater = Gateway::getTerrainWater();
  if (descriptor.mapWaters)
    if (!terrainWater->load(descriptor.mapWaters))
      return Logger::writeErrorLog("Could not load terrain waters");
      
  return true;
}

TerrainTextures* PraetoriansDataset::createTerrainTextures()
{
  return terrainTextures;
}

TerrainVisuals* PraetoriansDataset::createTerrainVisuals()
{
  return terrainVisuals;
}

TerrainLogic* PraetoriansDataset::createTerrainLogic()
{
  return terrainLogic;
}

WorldVisuals* PraetoriansDataset::createWorldVisuals()
{
  return worldVisuals;
}

TerrainPasture* PraetoriansDataset::createTerrainPasture()
{
  return terrainPasture;
}

TerrainWater* PraetoriansDataset::createTerrainWater()
{
  return terrainWater;
}

MissionScript* PraetoriansDataset::createMissionScript()
{
  return missionScript;
}

void PraetoriansDataset::reset()
{
  terrainTextures->reset();
  terrainVisuals->reset();
  terrainLogic->reset();
  terrainPasture->reset();
  terrainWater->reset();
  worldVisuals->reset();
}

PraetoriansDataset::~PraetoriansDataset()
{
  deleteObject(terrainTextures);
  deleteObject(terrainVisuals);
  deleteObject(terrainLogic);
  deleteObject(terrainPasture);
  deleteObject(terrainWater);
  deleteObject(worldVisuals);
  deleteObject(missionScript);
}