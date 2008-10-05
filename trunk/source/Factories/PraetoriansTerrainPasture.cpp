#include "PraetoriansTerrainPasture.h"
#include "../Kernel/Gateway.h"
#include "../FileSystem/FileSystem.h"
#include "../FileSystem/ArchivedFile.h"
#include "../Databases/TerrainDatabase.h"
#include "../Factories/TerrainLogic.h"
#include "../Managers/ManagersUtils.h"
#include "TerrainVisuals.h"

PraetoriansTerrainPasture::PraetoriansTerrainPasture()
{
  pasturename = "PRADERA_EUROPA";
}

void PraetoriansTerrainPasture::setName(const char* name)
{
  if (!name)
    return;
    
  if (!strcmp(name, "Nieve"))
  {
    pasturename = "PRADERA_NIEVE";
    return;
  }
  if (!strcmp(name, "EuropaCentral"))
  {
    pasturename = "PRADERA_EUROPA";
    return;
  }
  if (!strcmp(name, "Mediterraneo"))
  {
    pasturename = "PRADERA_MEDITERRANEO_2";
    return;
  }
  if (!strcmp(name, "Desierto"))
  {
    pasturename = "PRADERA_DESIERTO_2";
    return;
  }
  if (!strcmp(name, "Default"))
  {
    return;
  }
}

bool PraetoriansTerrainPasture::load(const char* path)
{
  ///unpacked files have higher priority
  const char* p = MediaPathManager::lookUpMediaPath(path);
  if (p)
    return loadUnpackedMedia(p);
    
  if (FileSystem::fileExist(path))
    return loadPackedMedia(path);
    
  return false;
}

bool PraetoriansTerrainPasture::loadPackedMedia(const char* path)
{
  if (!(file = FileSystem::checkOut(path)))
    return Logger::writeErrorLog(String("Could not check out archived item -> ") + path);
    
  readMeadowsChunk();
  
  FileSystem::checkIn(file);
  
  return true;
}

bool PraetoriansTerrainPasture::loadUnpackedMedia(const char* path)
{
  unsigned int pbanamelength;
  unsigned char meadownamelength;
  unsigned int meadowcount;
  unsigned int patchcount;
  char str32[32];
  Tuple4i meadowbounds;
  Tuple2i area;
  GrassPatch* patch;
  
  ifstream in(path, ios_base::binary);
  if (!in.is_open())
    return Logger::writeErrorLog(String("Could not load -> ") + path);
    
  memset(str32, 0, 32);
  
  area = Gateway::getTerrainVisuals()->getArea();
  
  in.seekg(8, ios_base::cur);
  in.read((char*)&pbanamelength, 4);
  in.read((char*)str32, pbanamelength);
  in.read((char*)&meadowcount, 4);
  pasturename = str32;
  
  for (unsigned int i = 0; i < meadowcount; i++)
  {
    in.read((char*)&patchcount, 4);
    in.read((char*)&meadowbounds, 16);
    in.read((char*)&meadownamelength, 1);
    in.seekg(meadownamelength, ios_base::cur);
    
    Meadow* meadow = new Meadow();
    meadow->setBounds(meadowbounds);
    meadow->setName(String("PRADERA") + int(i));
    //meadow.setPBAName(pbaname);
    
    for (unsigned int j = 0; j < patchcount; j++)
    {
      patch = new GrassPatch();
      in.read((char*)&patch->position, 8);
      in.read((char*)&patch->range, 8);
      in.read((char*)&patch->color, 3);
      meadow->addPatch(patch);
      
      meadow->addTileIndex(patch->position.y * area.x + patch->position.x);
    }
    
    meadowsList.append(meadow);
  }
  
  in.close();
  
  return true;
}

void PraetoriansTerrainPasture::readMeadowsChunk()
{
  unsigned int pbanamelength;
  unsigned char meadownamelength;
  unsigned int meadowcount;
  unsigned int patchcount;
  char str32[32];
  Tuple4i meadowbounds;
  Tuple2i area;
  GrassPatch* patch;
  
  memset(str32, 0, 32);
  
  area = Gateway::getTerrainVisuals()->getArea();
  
  file->seek(8, SEEKD);
  file->read(&pbanamelength, 4);
  file->read(str32, pbanamelength);
  file->read(&meadowcount, 4);
  pasturename = str32;
  
  for (unsigned int i = 0; i < meadowcount; i++)
  {
    file->read(&patchcount, 4);
    file->read(meadowbounds, 16);
    file->read(&meadownamelength, 1);
    file->seek(meadownamelength, SEEKD);
    
    Meadow* meadow = new Meadow();
    meadow->setBounds(meadowbounds);
    meadow->setName(String("PRADERA") + int(i));
    //meadow.setPBAName(pbaname);
    
    for (unsigned int j = 0; j < patchcount; j++)
    {
      patch = new GrassPatch();
      file->read(patch->position, 8);
      file->read(patch->range, 8);
      file->read(patch->color, 3);
      meadow->addPatch(patch);
      
      meadow->addTileIndex(patch->position.y * area.x + patch->position.x);
    }
    
    meadowsList.append(meadow);
  }
}

bool PraetoriansTerrainPasture::exportData(const char* projectName)
{
  Tuple4i mbounds;
  unsigned int pcnt;
  unsigned int version = 3;
  unsigned int pbanamelen = pasturename.getLength() + 1;
  unsigned int meadowscnt = meadowsList.length();
  char sig[4] = {'.','P','R','A'};
  char mnamelen = 8;
  char pasname[8] = {'P','R','A','D','E','R','A','\0'};
  String path = Gateway::getExportPath();
  
  ofstream out((path + "Mapas/" + projectName + ".PRA").getBytes(), ios_base::binary);
  
  if (!out.is_open())
    return false;
    
  for (unsigned int i = 0; i < meadowscnt; i++)
    meadowsList(i)->updateBounds();
    
  out.write(sig, 4);
  out.write((char*)&version, 4);
  out.write((char*)&pbanamelen, 4);
  out.write(pasturename.getBytes(), pbanamelen);
  out.write((char*)&meadowscnt, 4);
  
  for (unsigned int i = 0; i < meadowsList.length(); i++)
  {
    pcnt = meadowsList(i)->getPatchCount();
    mbounds = meadowsList(i)->getBounds();
    
    out.write((char*)&pcnt, 4);
    out.write((char*)&mbounds, 16);
    out.write((char*)&mnamelen, 1);
    out.write(pasname, mnamelen);
    
    for (unsigned int j = 0; j < pcnt; j++)
    {
      out.write((char*)&meadowsList(i)->getPatch(j)->position, 8);
      out.write((char*)&meadowsList(i)->getPatch(j)->range, 8);
      out.write((char*)&meadowsList(i)->getPatch(j)->color, 3);
    }
  }
  
  out.close();
  
  return true;
}

unsigned int PraetoriansTerrainPasture::getMeadowCount()
{
  return meadowsList.length();
}

Meadow* PraetoriansTerrainPasture::getMeadow(unsigned int index)
{
  return meadowsList(index);
}

Meadow* PraetoriansTerrainPasture::getMeadow(const char* name)
{
  for (unsigned int i = 0; i < meadowsList.length(); i++)
    if (meadowsList(i)->getName() == name)
      return meadowsList(i);
      
  return 0;
}

int PraetoriansTerrainPasture::getMeadowIndex(const char* name)
{
  for (unsigned int i = 0; i < meadowsList.length(); i++)
    if (meadowsList(i)->getName() == name)
      return i;
      
  return -1;
}

Meadow* PraetoriansTerrainPasture::createMeadow()
{
  Meadow* meadow = new Meadow();
  //newmeadow.setPBAName(pasturename);
  meadow->setName(String("PRADERA") + int(meadowsList.length()));
  meadowsList.append(meadow);
  return meadow;
}

GrassPatch* PraetoriansTerrainPasture::createPatch(unsigned int meadowIdx)
{
  GrassPatch* patch = new GrassPatch();
  meadowsList(meadowIdx)->addPatch(patch);
  return patch;
}

GrassPatch* PraetoriansTerrainPasture::createPatch(const char* meadowName)
{
  GrassPatch* patch;
  
  for (unsigned int i = 0; i < meadowsList.length(); i++)
    if (meadowsList(i)->getName() == meadowName)
    {
      patch = new GrassPatch();
      meadowsList(i)->addPatch(patch);
      return patch;
    }
    
  return 0;
}

void PraetoriansTerrainPasture::clearMeadowPatches(const char* meadowName)
{
  Meadow* meadow;
  GrassPatch* patch;
  TerrainLogic* tlogic = Gateway::getTerrainLogic();
  Tuple2i area = Gateway::getTerrainVisuals()->getArea();
  unsigned int lflag, index;
  meadow = getMeadow(meadowName);
  
  for (unsigned int i = 0; i < meadow->getPatchCount(); i++)
  {
    patch = meadow->getPatch(i);
    index = patch->position.y * area.x + patch->position.x;
    lflag = tlogic->getFlags(index);
    tlogic->setFlags(index, (lflag & MASK_FLAGS) | (TileTypes::UNUSED << 27));
  }
  
  meadow->clearPatches();
}

const char* PraetoriansTerrainPasture::getName()
{
  return pasturename;
}

void PraetoriansTerrainPasture::reset()
{
  meadowsList.clearAndDestroy();
}

PraetoriansTerrainPasture::~PraetoriansTerrainPasture()
{
  meadowsList.clearAndDestroy();
}
