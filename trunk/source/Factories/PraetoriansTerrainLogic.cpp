#include "PraetoriansTerrainLogic.h"
#include "../Kernel/Gateway.h"
#include "../FileSystem/FileSystem.h"
#include "../FileSystem/ArchivedFile.h"
#include "../Managers/ManagersUtils.h"
#include "TerrainVisuals.h"

PraetoriansTerrainLogic::PraetoriansTerrainLogic()
{
}

bool PraetoriansTerrainLogic::load(const char* path)
{
  ///unpacked files have higher priority
  const char* p = MediaPathManager::lookUpMediaPath(path);
  if (p)
    return loadUnpackedMedia(p);
    
  if (FileSystem::fileExist(path))
    return loadPackedMedia(path);
    
  return false;
}

bool PraetoriansTerrainLogic::loadPackedMedia(const char* path)
{
  unsigned int flag;
  ArchivedFile* file;
  
  if (!(file = FileSystem::checkOut(path)))
    return Logger::writeErrorLog(String("Could not load ") + path);
    
  file->read(area, 8);
  
  for (int i = 0; i < area.x * area.y; i++)
  {
    file->read(&flag, 4);
    logicFlags.append(flag);
  }
  
  FileSystem::checkIn(file);
  
  return true;
}

bool PraetoriansTerrainLogic::loadUnpackedMedia(const char* path)
{
  unsigned int flag;
  ifstream in(path, ios_base::binary);
  
  if (!in.is_open())
    return Logger::writeErrorLog(String("Could not load -> ") + path);
    
  in.read((char*)&area, 8);
  
  for (int a = 0; a < area.x * area.y; a++)
  {
    in.read((char*)&flag, 4);
    logicFlags.append(flag);
  }
  
  in.close();
  
  return true;
}

bool PraetoriansTerrainLogic::exportData(const char* projectName)
{
  unsigned int flag;
  String path = Gateway::getExportPath();
  
  ofstream out((path + "Mapas/" + projectName + ".MLG").getBytes(), ios_base::binary);
  
  if (!out.is_open())
    return false;
    
  out.write((char*)&area, 8);
  
  for (unsigned int i = 0; i < logicFlags.length(); i++)
  {
    flag = logicFlags(i);
    out.write((char*)&flag, 4);
  }
  
  out.close();
  
  return true;
}

bool PraetoriansTerrainLogic::createLogicMap(unsigned int width, unsigned height)
{
  unsigned int flag;
  
  area.set(width, height);
  
  if (width < 30 || height < 30)
    return Logger::writeErrorLog("Map dimensions are too small");
    
  flag = /*TileLogic::FLAG_FLAT | TileLogic::FLAG_BUILDABLE*/TileLogic::FLAG_FLAT | (TileTypes::NORMAL<<27);
  for (unsigned int i = 0; i < width * height; i++)
    logicFlags.append(flag);
    
  flag = TileLogic::FLAG_SCENERY_END /*| (TileTypes::SCENERY_END<<27)*/;
  for (unsigned int x = 0; x < width; x++)
  {
    logicFlags(0 * width + x) = (logicFlags(0 * width + x) & MASK_FLAGS) | flag;
    logicFlags((height-1) * width + x) = (logicFlags((height-1) * width + x) & MASK_FLAGS) | flag;
  }
  
  for (unsigned int y = 1; y < height-1; y++)
  {
    logicFlags(y * width + 0) = (logicFlags(y * width + 0) & MASK_FLAGS) | flag;
    logicFlags(y * width + (width-1)) = (logicFlags(y * width + (width-1)) & MASK_FLAGS) | flag;
  }
  
  return true;
}

unsigned int PraetoriansTerrainLogic::getFlags(unsigned int index)
{
  return logicFlags(index);
}

unsigned int PraetoriansTerrainLogic::getFlagCount()
{
  return logicFlags.length();
}

void PraetoriansTerrainLogic::setFlags(unsigned int index, unsigned int flags)
{
  logicFlags(index) = flags;
}

void PraetoriansTerrainLogic::reset()
{
  logicFlags.clear();
  area.set(0);
}

PraetoriansTerrainLogic::~PraetoriansTerrainLogic()
{
}
