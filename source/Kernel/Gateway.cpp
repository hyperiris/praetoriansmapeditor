#include "Gateway.h"
#include "../Databases/VillageDatabase.h"
#include "../Databases/TerrainDatabase.h"
#include "../Databases/WaterDatabase.h"

#include "../Managers/ManagersUtils.h"

#include "../Factories/PraetoriansDataset.h"
#include "../Factories/TerrainVisuals.h"
#include "../Factories/TerrainPasture.h"

#include "../Graphs/SpatialIndex.h"
#include "../Nodes/TransformGroup.h"

#include "../Visitors/SpatialBoundsUpdater.h"
#include "../Visitors/YFieldUPdater.h"

#include "../FileSystem/FileSystem.h"

#include "../Memory/PageFile.h"
#include "../Memory/BlockMemory.h"

Dataset* Gateway::dataset = 0;

TerrainDatabase* Gateway::terrainDatabase = 0;
ModelDatabase* Gateway::natureDatabase = 0;
ModelDatabase* Gateway::characterDatabase = 0;
ModelDatabase* Gateway::critterDatabase = 0;
ModelDatabase* Gateway::structureDatabase = 0;
ModelDatabase* Gateway::villageDatabase = 0;
WaterDatabase* Gateway::waterDatabase = 0;

SpatialIndex* Gateway::spatialIndex = 0;

PageFile* Gateway::pageFile = 0;
BlockMemory* Gateway::filebufferMemory = 0;
BlockMemory* Gateway::geometryMemory = 0;
BlockMemory* Gateway::terrainMemory = 0;

CurrentInfoStructure Gateway::currentInfoStructure;
TransformGroup* Gateway::activeStructureGroup = 0;
TransformGroup* Gateway::activeNatureGroup = 0;
TransformGroup* Gateway::activeCharacterGroup = 0;
TransformGroup* Gateway::activeCritterGroup = 0;
TransformGroup* Gateway::activeVillageGroup = 0;

EditorConfiguration Gateway::editorConfig;
MapDescriptor Gateway::mapDescriptor;

Texture Gateway::backgroundTexture;

String Gateway::exportPath;
String Gateway::menuMusicPath;
bool Gateway::menuMusicEnabled = true;

BrushMatrixDescriptor Gateway::brushMatrixDescriptor;

bool Gateway::initialize()
{
  bool succArray[12] = {false};
  
  pageFile = new PageFile();
  if (!pageFile->initialize(60))
    return Logger::writeErrorLog("Could not initialize Page File");
    
  geometryMemory = new BlockMemory(/*256, 2*/);
  succArray[0] = geometryMemory->initialize(1 * 1024 * 1024);
  
  terrainMemory = new BlockMemory(2048, 2);
  succArray[1] = terrainMemory->initialize(30 * 1024 * 1024);
  
  filebufferMemory = new BlockMemory(/*256, 2*/);
  succArray[0] = filebufferMemory->initialize(10 * 1024 * 1024);
  
  dataset = new PraetoriansDataset();
  
  spatialIndex = new SpatialIndex();
  
  terrainDatabase = new TerrainDatabase();
  natureDatabase = new ModelDatabase("Naturaleza");
  structureDatabase = new ModelDatabase("Estructuras");
  characterDatabase = new ModelDatabase("Unidades");
  critterDatabase = new ModelDatabase("Critters");
  villageDatabase = new VillageDatabase("VillageDatabase");
  waterDatabase = new WaterDatabase();
  
  natureDatabase->setType(WorldObjectTypes::NATURE);
  structureDatabase->setType(WorldObjectTypes::STRUCTURE);
  critterDatabase->setType(WorldObjectTypes::CRITTER);
  
  succArray[2] = terrainDatabase->initialize();
  succArray[3] = villageDatabase->initialize("VillageDatabase.xml");
  succArray[4] = natureDatabase->initialize("Naturaleza.xml");
  succArray[5] = structureDatabase->initialize("Estructuras.xml");
  succArray[6] = characterDatabase->initialize("Unidades.xml");
  succArray[7] = critterDatabase->initialize("Critters.xml");
  succArray[8] = waterDatabase->initialize("");
  
  succArray[9] = dataset->initialize();
  
  Gateway::loadBackground("FondoMenu.png");
  
  return (succArray[0] && succArray[1] && succArray[2] && succArray[3] &&
          succArray[4] && succArray[5] && succArray[6] && succArray[7] &&
          succArray[8] && succArray[9]);
}

//
///laboratory
//
void Gateway::setActiveNatureElement(const GSElementInfo& element)
{
  currentInfoStructure.currentNatureElement = element;
}

GSElementInfo* Gateway::getActiveNatureInfo()
{
  return &currentInfoStructure.currentNatureElement;
}

TransformGroup* Gateway::getActiveNature()
{
  return activeNatureGroup;
}

void Gateway::setActiveNature(TransformGroup* group)
{
  if (activeNatureGroup)
    deleteObject(activeNatureGroup);
    
  activeNatureGroup = group;
}


void Gateway::setActiveCharacterElement(const GSElementInfo& element)
{
  currentInfoStructure.currentCharacterElement = element;
}

GSElementInfo* Gateway::getActiveCharacterInfo()
{
  return &currentInfoStructure.currentCharacterElement;
}

TransformGroup* Gateway::getActiveCharacter()
{
  return activeCharacterGroup;
}

void Gateway::setActiveCharacter(TransformGroup* group)
{
  if (activeCharacterGroup)
    deleteObject(activeCharacterGroup);
    
  activeCharacterGroup = group;
}


void Gateway::setActiveCritterElement(const GSElementInfo& element)
{
  currentInfoStructure.currentCritterElement = element;
}

GSElementInfo* Gateway::getActiveCritterInfo()
{
  return &currentInfoStructure.currentCritterElement;
}

TransformGroup* Gateway::getActiveCritter()
{
  return activeCritterGroup;
}

void Gateway::setActiveCritter(TransformGroup* group)
{
  if (activeCritterGroup)
    deleteObject(activeCritterGroup);
    
  activeCritterGroup = group;
}


void Gateway::setActiveStructureElement(const GSElementInfo& element)
{
  currentInfoStructure.currentStructureElement = element;
}

GSElementInfo* Gateway::getActiveStructureInfo()
{
  return &currentInfoStructure.currentStructureElement;
}

TransformGroup* Gateway::getActiveStructure()
{
  return activeStructureGroup;
}

void Gateway::setActiveStructure(TransformGroup* group)
{
  if (activeStructureGroup)
    deleteObject(activeStructureGroup);
    
  activeStructureGroup = group;
}


void Gateway::setActiveVillageElement(const GSElementInfo& element)
{
  currentInfoStructure.currentVillageElement = element;
}

GSElementInfo* Gateway::getActiveVillageInfo()
{
  return &currentInfoStructure.currentVillageElement;
}

TransformGroup* Gateway::getActiveVillage()
{
  return activeVillageGroup;
}

void Gateway::setActiveVillage(TransformGroup* group)
{
  if (activeVillageGroup)
    deleteObject(activeVillageGroup);
    
  activeVillageGroup = group;
}


//
/// loading
//
bool Gateway::constructMap(const MapDescriptor& descriptor)
{
  Gateway::reset();
  
  mapDescriptor = descriptor;
  
  if (!dataset->load(descriptor))
    return Logger::writeErrorLog("Could not load dataset");
    
  terrainDatabase->compile();
  villageDatabase->compile();
  structureDatabase->compile();
  natureDatabase->compile();
  critterDatabase->compile();
  waterDatabase->compile();
  
  spatialIndex->compute();
  
  return true;
}

bool Gateway::exportMap()
{
  if (mapDescriptor.mapProjectName.isBlank())
    return false;
    
  return dataset->exportData(mapDescriptor.mapProjectName);
}

//
///laboratories
//
TransformGroup* Gateway::getNaturePrototype(const char* name)
{
  if (!name)
    return 0;
    
  return natureDatabase->getPrototype(name);
}

TransformGroup* Gateway::getCharacterPrototype(const char* name)
{
  if (!name)
    return 0;
    
  return characterDatabase->getPrototype(name);
}

TransformGroup* Gateway::getCritterPrototype(const char* name)
{
  if (!name)
    return 0;
    
  return critterDatabase->getPrototype(name);
}

TransformGroup* Gateway::getStructurePrototype(const char* name)
{
  if (!name)
    return 0;
    
  return structureDatabase->getPrototype(name);
}

TransformGroup* Gateway::getVillagePrototype(const char* name)
{
  if (!name)
    return 0;
    
  return villageDatabase->getPrototype(name);
}

//
/// visitation
//
void Gateway::inspectTerrainGraph(unsigned int layer, Visitor* visitor)
{
  terrainDatabase->inspectGraph(layer, visitor);
}

void Gateway::inspectSpatialIndex(Visitor* visitor)
{
  spatialIndex->inspect(visitor);
}

void Gateway::updateSpatialIndex(const Tuple3f &point, float radius)
{
  int index;
  Tuple2i cellarea;
  Tuple2i tilearea;
  unsigned int cellsize;
  SpatialBoundsUpdater bUpdater;
  //YFieldUpdater yUpdater;
  
  cellarea = spatialIndex->getArea();
  cellsize = spatialIndex->getCellSize();
  tilearea = dataset->createTerrainVisuals()->getArea();
  
  unsigned int xOff = clamp(int(point.x - radius), 0, tilearea.y - 1);
  unsigned int yOff = clamp(int(point.z - radius), 0, tilearea.x - 1);
  unsigned int zOff = clamp(int(point.x + radius), 0, tilearea.y - 1);
  unsigned int wOff = clamp(int(point.z + radius), 0, tilearea.x - 1);
  
  unsigned int yratio = (unsigned int) floor((float)yOff / cellsize);
  unsigned int wratio = (unsigned int) floor((float)wOff / cellsize);
  unsigned int xratio = (unsigned int) floor((float)xOff / cellsize);
  unsigned int zratio = (unsigned int) floor((float)zOff / cellsize);
  
  for (unsigned int x = xratio; x <= zratio; x++)
    for (unsigned int y = yratio; y <= wratio; y++)
    {
      index = x * cellarea.x + y;
      spatialIndex->getCell(index)->accept(&bUpdater);
      //spatialIndex->getCell(index)->accept(&yUpdater);
      bUpdater.reset();
    }
}

//
///dataset
//
TerrainTextures* Gateway::getTerrainTextures()
{
  return dataset->createTerrainTextures();
}

TerrainVisuals* Gateway::getTerrainVisuals()
{
  return dataset->createTerrainVisuals();
}

TerrainLogic* Gateway::getTerrainLogic()
{
  return dataset->createTerrainLogic();
}

WorldVisuals* Gateway::getWorldVisuals()
{
  return dataset->createWorldVisuals();
}

TerrainPasture* Gateway::getTerrainPasture()
{
  return dataset->createTerrainPasture();
}

TerrainWater* Gateway::getTerrainWater()
{
  return dataset->createTerrainWater();
}

//
///databases
//
TerrainDatabase* Gateway::getTerrainDatabase()
{
  return terrainDatabase;
}

ModelDatabase* Gateway::getNatureDatabase()
{
  return natureDatabase;
}

ModelDatabase* Gateway::getCharacterDatabase()
{
  return characterDatabase;
}

ModelDatabase* Gateway::getCritterDatabase()
{
  return critterDatabase;
}

ModelDatabase* Gateway::getStructureDatabase()
{
  return structureDatabase;
}

ModelDatabase* Gateway::getVillageDatabase()
{
  return villageDatabase;
}

WaterDatabase* Gateway::getWaterDatabase()
{
  return waterDatabase;
}

bool Gateway::findType(const char* objName, int& typeVal)
{
  if ((typeVal = natureDatabase->getType(objName)) != -1)
    return true;
  else if ((typeVal = structureDatabase->getType(objName)) != -1)
    return true;
  else if ((typeVal = critterDatabase->getType(objName)) != -1)
    return true;
    
  return false;
}

//
///configuration
//
EditorConfiguration& Gateway::getConfiguration()
{
  return editorConfig;
}

MapDescriptor& Gateway::getMapDescriptor()
{
  return mapDescriptor;
}

//
///memory
//
void* Gateway::aquireGeometryMemory(unsigned int bytes)
{
  return geometryMemory->aquire(bytes);
}

bool Gateway::releaseGeometryMemory(void* addr)
{
  return geometryMemory->release(addr);
}

void* Gateway::aquireTerrainMemory(unsigned int bytes)
{
  return terrainMemory->aquire(bytes);
}

bool Gateway::releaseTerrainMemory(void* addr)
{
  return terrainMemory->release(addr);
}

void* Gateway::aquirePageMemory(unsigned int bytes)
{
  return pageFile->commit(bytes);
}

bool Gateway::releasePageMemory(void* addr)
{
  return pageFile->decommit(addr);
}

void* Gateway::aquireFilebufferMemory(unsigned int bytes)
{
  return filebufferMemory->aquire(bytes);
}

bool Gateway::releaseFilebufferMemory(void* addr)
{
  return filebufferMemory->release(addr);
}

PageFile* Gateway::getPageFile()
{
  return pageFile;
}

MemoryProfile Gateway::getMemoryProfile()
{
  MemoryProfile profile;
  
  profile.total = pageFile->getTotalSize();
  profile.used = pageFile->getUsedSize();
  profile.peak = pageFile->getPeakSize();
  
  profile.usedgeo = geometryMemory->getUsedSize();
  profile.usedterr = terrainMemory->getUsedSize();
  profile.totalfile = filebufferMemory->getTotalSize();
  
  profile.totalgeo = geometryMemory->getTotalSize();
  profile.totalterr = terrainMemory->getTotalSize();
  profile.totalfile = filebufferMemory->getTotalSize();
  
  return profile;
}

//
///spatial index
//
SpatialIndex* Gateway::getSpatialIndex()
{
  return spatialIndex;
}

void Gateway::loadBackground(const char* path)
{
  backgroundTexture.load2D(path);
}

void Gateway::bindBackground()
{
  if (!backgroundTexture.getID())
    Gateway::loadBackground("FondoMenu.png");
    
  glBindTexture(GL_TEXTURE_2D, backgroundTexture.getID());
}

void Gateway::destroyBackground()
{
  backgroundTexture.destroy();
}

void Gateway::setExportPath(const char* path)
{
  exportPath = path;
}

const char* Gateway::getExportPath()
{
  return exportPath;
}

void Gateway::setMenuMusicPath(const char* path)
{
  menuMusicPath = path;
}

const char* Gateway::getMenuMusicPath()
{
  return menuMusicPath;
}

void Gateway::enableMenuMusic(bool enable)
{
  menuMusicEnabled = enable;
}

bool Gateway::isMenuMusicEnabled()
{
  return menuMusicEnabled;
}

void Gateway::update()
{
  SoundManager::update();
}

void Gateway::setBrushMatrixDescriptor(const BrushMatrixDescriptor& descriptor)
{
  brushMatrixDescriptor = descriptor;
}

BrushMatrixDescriptor& Gateway::getBrushMatrixDescriptor()
{
  return brushMatrixDescriptor;
}

void Gateway::reset()
{
  terrainDatabase->reset();
  natureDatabase->reset();
  characterDatabase->reset();
  critterDatabase->reset();
  villageDatabase->reset();
  structureDatabase->reset();
  waterDatabase->reset();
  
  spatialIndex->reset();
  
  dataset->reset();
  
  Gateway::destroyBackground();
  
  if (activeStructureGroup)
    activeStructureGroup->destroy();
    
  if (activeNatureGroup)
    activeNatureGroup->destroy();
    
  if (activeCharacterGroup)
    activeCharacterGroup->destroy();
    
  if (activeCritterGroup)
    activeCritterGroup->destroy();
    
  if (activeVillageGroup)
    activeVillageGroup->destroy();
    
  PrototypeManager::flushAllPrototypes();
  
  FileSystem::reset();
  
  filebufferMemory->reset();
  geometryMemory->reset();
  terrainMemory->reset();
  pageFile->reset();
  
  Logger::flush();
}

void Gateway::destroy()
{
  deleteObject(terrainDatabase);
  deleteObject(villageDatabase);
  deleteObject(natureDatabase);
  deleteObject(characterDatabase);
  deleteObject(critterDatabase);
  deleteObject(structureDatabase);
  deleteObject(waterDatabase);
  deleteObject(spatialIndex);
  deleteObject(dataset);
  
  deleteObject(activeStructureGroup);
  deleteObject(activeNatureGroup);
  deleteObject(activeCharacterGroup);
  deleteObject(activeCritterGroup);
  deleteObject(activeVillageGroup);
  
  //geometryMemory->printMemoryProfile("geomem.txt");
  //terrainMemory->printMemoryProfile("terrmem.txt");
  deleteObject(filebufferMemory);
  deleteObject(geometryMemory);
  deleteObject(terrainMemory);
  deleteObject(pageFile);
}