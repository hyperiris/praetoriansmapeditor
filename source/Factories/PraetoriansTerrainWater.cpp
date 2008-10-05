#include "PraetoriansTerrainWater.h"
#include "Water.h"
#include "../Kernel/Gateway.h"
#include "../Managers/ManagersUtils.h"
#include "../Databases/WaterDatabase.h"
#include "../Nodes/TransformGroup.h"
#include "../Geometry/Model.h"
#include "../Geometry/Geometry.h"
#include "../Appearance/Appearance.h"

#include "../FileSystem/FileSystem.h"
#include "../FileSystem/ArchivedFile.h"

#include "../Renderer/Renderer.h"

#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>

struct WaterFileChunk
{
  unsigned int pointer;
  unsigned int length;
};

PraetoriansTerrainWater::PraetoriansTerrainWater()
{
}

bool PraetoriansTerrainWater::load(const char* path)
{
  const char* p = MediaPathManager::lookUpMediaPath(path);
  if (p)
    return loadUnpackedMedia(p);
    
  if (FileSystem::fileExist(path))
    return loadPackedMedia(path);
    
  return false;
}

bool PraetoriansTerrainWater::loadPackedMedia(const char* path)
{
  unsigned int signature;
  unsigned short chunkid;
  unsigned int chunklength;
  unsigned int texturescount;///use one in this version
  unsigned int watercount;
  unsigned int vertexcount;
  unsigned int indexcount;
  
  Tuple3f* vertices;
  unsigned short* indices;
  Tuple4ub* colors;
  Tuple2f* txcoords;
  ArchivedFile* file;
  WaterDatabase* wdatabase;
  
  if (!(file = FileSystem::checkOut(path)))
    return Logger::writeErrorLog(String("Could not load -> ") + path);
    
  wdatabase = Gateway::getWaterDatabase();
  
  file->read(&signature, 4);
  
  file->read(&chunkid, 2);
  file->read(&chunklength, 4);
  file->read(&texturescount, 4);
  for (unsigned int i = 0; i < texturescount; i++)
    file->seek((256 * 256 * 4) + 6, SEEKD);
    
  file->read(&watercount, 4);
  for (unsigned int i = 0; i < watercount; i++)
  {
    file->read(&chunkid, 2);
    file->read(&chunklength, 4);
    file->seek(48, SEEKD);
    file->read(&vertexcount, 4);
    
    vertices = new Tuple3f[vertexcount];
    colors = new Tuple4ub[vertexcount];
    txcoords = new Tuple2f[vertexcount];
    for (unsigned int j = 0; j < vertexcount; j++)
    {
      file->read(vertices[j], 12);
      Swap(vertices[j].x, vertices[j].z);
      file->read(colors[j], 4);
      Swap(colors[j].x, colors[j].z);
      file->read(txcoords[j], 8);
    }
    
    file->read(&indexcount, 4);
    indices = new unsigned short[indexcount];
    file->read(indices, indexcount * 2);
    
    String watername = String("H2O_") + int(wdatabase->getWatersCount());
    Geometry* geometry;
    geometry = new Geometry(watername, indexcount, vertexcount);
    geometry->setIndices(indices, false);
    geometry->setVertices(vertices, false);
    geometry->setColors(colors, false);
    geometry->setTextureElements(txcoords, 2, false);
    geometry->computeBounds();
    
    Appearance* appearance = new Appearance();
    appearance->setBlendAttributes(BlendAttributes(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    appearance->setTexture(0, wdatabase->getWaterTexture());
    
    Model* model = new Model();
    model->setAppearance(appearance);
    model->setGeometry(geometry);
    
    TransformGroup* group = new TransformGroup();
    group->addChild(model);
    group->updateBoundsDescriptor();
    wdatabase->addWaterModel(group);
    
    deleteArray(vertices);
    deleteArray(indices);
    deleteArray(colors);
    deleteArray(txcoords);
  }
  
  FileSystem::checkIn(file);
  
  return true;
}

bool PraetoriansTerrainWater::loadUnpackedMedia(const char* path)
{
  unsigned int signature;
  unsigned short chunkid;
  unsigned int chunklength;
  unsigned int texturescount;///use one in this version
  unsigned int watercount;
  unsigned int vertexcount;
  unsigned int indexcount;
  
  Tuple3f* vertices;
  unsigned short* indices;
  Tuple4ub* colors;
  Tuple2f* txcoords;
  
  WaterDatabase* wdatabase;
  
  ifstream in(path, ios_base::binary);
  if (!in.is_open())
    return Logger::writeErrorLog(String("Could not load -> ") + path);
    
  wdatabase = Gateway::getWaterDatabase();
  
  in.read((char*)&signature, 4);
  
  in.read((char*)&chunkid, 2);
  in.read((char*)&chunklength, 4);
  in.read((char*)&texturescount, 4);
  for (unsigned int i = 0; i < texturescount; i++)
    in.seekg((256 * 256 * 4) + 6, ios_base::cur);
    
  in.read((char*)&watercount, 4);
  for (unsigned int i = 0; i < watercount; i++)
  {
    in.read((char*)&chunkid, 2);
    in.read((char*)&chunklength, 4);
    in.seekg(32 + 16, ios_base::cur);
    in.read((char*)&vertexcount, 4);
    vertices = new Tuple3f[vertexcount];
    colors = new Tuple4ub[vertexcount];
    txcoords = new Tuple2f[vertexcount];
    for (unsigned int j = 0; j < vertexcount; j++)
    {
      in.read((char*)&vertices[j], 12);
      Swap(vertices[j].x, vertices[j].z);
      in.read((char*)&colors[j], 4);
      Swap(colors[j].x, colors[j].z);
      in.read((char*)&txcoords[j], 8);
    }
    in.read((char*)&indexcount, 4);
    indices = new unsigned short[indexcount];
    in.read((char*)indices, indexcount * 2);
    
    String watername = String("H2O_") + int(wdatabase->getWatersCount());
    Geometry* geometry;
    geometry = new Geometry(watername, indexcount, vertexcount);
    geometry->setIndices(indices, false);
    geometry->setVertices(vertices, false);
    geometry->setColors(colors, false);
    geometry->setTextureElements(txcoords, 2, false);
    geometry->computeBounds();
    
    Appearance* appearance = new Appearance();
    appearance->setBlendAttributes(BlendAttributes(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    appearance->setTexture(0, wdatabase->getWaterTexture());
    
    Model* model = new Model();
    model->setAppearance(appearance);
    model->setGeometry(geometry);
    
    TransformGroup* group = new TransformGroup();
    group->addChild(model);
    group->updateBoundsDescriptor();
    wdatabase->addWaterModel(group);
    
    deleteArray(vertices);
    deleteArray(indices);
    deleteArray(colors);
    deleteArray(txcoords);
  }
  
  in.close();
  
  return true;
}

bool PraetoriansTerrainWater::exportData(const char* projectName)
{
  WaterDatabase* wdatabase;
  TransformGroup* tgroup;
  TransformGroup* tgroupchild;
  Geometry* geo;
  const char* inp;
  unsigned char* colsbuf;
  Tuple3f* verts;
  Tuple4ub* colors;
  Tuple2f *txcoords;
  unsigned short* indices;
  
  unsigned int signature = 2;
  unsigned short texturechunkid = 22912;
  unsigned int texturechunklen = 262164;
  unsigned int texturecount = 1;
  
  unsigned short textureid = 22914;
  unsigned int texturelen = 262150;
  
  unsigned short waterid = 22913;
  unsigned int waterlen = 0;
  
  unsigned int watercount;
  unsigned char bullshit0 = 0;
  unsigned char bullshit01[4] = {0xF0, 0xAD, 0xBA, 0x0D};
  unsigned char bullshit1[4] = {0x00};
  unsigned int vertcount;
  unsigned int idxcount;
  Tuple4ub refcol;
  Tuple3f refvert;
  String path = Gateway::getExportPath();
  
  if (!(inp = MediaPathManager::lookUpMediaPath("water_2_1.col")))
    return Logger::writeErrorLog("Could not locate water_2_1.col");
  ifstream in(inp, ios_base::binary);
  if (!in.is_open())
    return false;
  colsbuf = (unsigned char*) Gateway::aquireGeometryMemory(256 * 256 * 4);
  in.read((char*)colsbuf, 256 * 256 * 4);
  in.close();
  
  ofstream out((path + "Mapas/" + projectName + ".H2O").getBytes(), ios_base::binary);
  
  if (!out.is_open())
    return false;
    
  out.write((char*)&signature, 4);
  out.write((char*)&texturechunkid, 2);
  out.write((char*)&texturechunklen, 4);
  out.write((char*)&texturecount, 4);
  
  out.write((char*)&textureid, 2);
  out.write((char*)&texturelen, 4);
  out.write((char*)colsbuf, 256 * 256 * 4);
  
  Gateway::releaseGeometryMemory(colsbuf);
  
  wdatabase = Gateway::getWaterDatabase();
  tgroup = wdatabase->getRootGroup();
  
  watercount = tgroup->getGroupsCount();
  out.write((char*)&watercount, 4);
  
  for (unsigned int i = 0; i < watercount; i++)
  {
    tgroupchild = tgroup->getGroup(i);
    geo = tgroupchild->getModel(0)->getGeometry();
    verts = geo->getVertices();
    colors = geo->getColors();
    txcoords = geo->getTextureCoords();
    indices = geo->getIndices();
    vertcount = geo->getVerticesCount();
    idxcount = geo->getIndicesCount();
    
    out.write((char*)&waterid, 2);
    waterlen = 52 + (vertcount*24) + 4 + (idxcount*2) + 6;
    out.write((char*)&waterlen, 4);
    
    out.write((char*)&bullshit0, 1);
    for (unsigned int j = 0; j < 7; j++)
      out.write((char*)bullshit01, 4);
    out.write((char*)bullshit01, 3);
    for (unsigned int j = 0; j < 4; j++)
      out.write((char*)bullshit1, 4);
      
    out.write((char*)&vertcount, 4);
    
    for (unsigned int j = 0; j < vertcount; j++)
    {
      refvert = verts[j];
      Swap(refvert.x, refvert.z);
      out.write((char*)&refvert, 12);
      
      refcol = colors[j];
      Swap(refcol.x, refcol.z);
      out.write((char*)&refcol, 4);
      
      out.write((char*)&txcoords[j], 8);
    }
    
    out.write((char*)&idxcount, 4);
    out.write((char*)indices, idxcount * 2);
  }
  
  out.close();
  
  return true;
}

Water* PraetoriansTerrainWater::createWater()
{
  Water *water = new Water();
  return water;
}

void PraetoriansTerrainWater::addWater(Water* water)
{
  watersList.append(water);
}

bool PraetoriansTerrainWater::destroyAllWaters()
{
  watersList.clearAndDestroy();
  return true;
}

bool PraetoriansTerrainWater::destroyWater(const char* name)
{
  Water* water = 0;
  for (unsigned int i = 0; i < watersList.length(); i++)
  {
    if (watersList(i)->getName() == name)
    {
      water = watersList.removeAt(i);
      deleteObject(water);
      break;
    }
  }
  return (water != 0);
}

void PraetoriansTerrainWater::reset()
{
  watersList.clearAndDestroy();
}

unsigned int PraetoriansTerrainWater::getWaterCount()
{
  return watersList.length();
}

Water* PraetoriansTerrainWater::getWater(unsigned int index)
{
  return watersList(index);
}

PraetoriansTerrainWater::~PraetoriansTerrainWater()
{
  watersList.clearAndDestroy();
}