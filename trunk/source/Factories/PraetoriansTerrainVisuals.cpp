#include "PraetoriansTerrainVisuals.h"
#include "../Managers/ManagersUtils.h"
#include "../FileSystem/FileSystem.h"
#include "../FileSystem/ArchivedFile.h"
#include "../Kernel/Gateway.h"
#include "../Tools/ICString.h"
#include "FactoryUtils.h"
#include "TerrainTextures.h"
#include "../Renderer/Renderer.h"
#include "Tile.h"

unsigned short PraetoriansTerrainVisuals::indices[10] = {4,3,0,1,2,5,8,7,6,3};

struct BMPFileHeader
{
  unsigned short  magic;
  unsigned int    size;
  unsigned int    reserved;
  unsigned int    begin;
};

struct BMPInfoHeader
{
  unsigned int    size;
  unsigned int    width;
  unsigned int    height;
  unsigned short  bullshit;
  unsigned short  bpp;
  unsigned int    compression;
  unsigned int    area;
  unsigned int    resolutionX;
  unsigned int    resolutionY;
  unsigned int    colors;
  unsigned int    icolors;
};

PraetoriansTerrainVisuals::PraetoriansTerrainVisuals()
{
  pColorMap     = 0;
  pVertexmap    = 0;
  pTileIndexSet = 0;
  pHeightmap    = 0;
  heightRatio   = 0;
}

bool PraetoriansTerrainVisuals::createHeightMap(unsigned int width, unsigned int height)
{
  unsigned int size = (width * 2 + 1) * (height * 2 + 1);
  
  area.set(width, height);
  extrema.set(0);
  heightRatio = 0;//computeHeightRatio();
  
  setupDefaultTileset();
  
  pTileIndexSet = (TileIndexSet*) Gateway::aquirePageMemory(area.x * area.y * sizeof(TileIndexSet));
  pVertexmap = (Tuple3f*) Gateway::aquirePageMemory(size * sizeof(Tuple3f));
  
  setupXZSpace();
  setupTileIndices();
  setupTileVertices();
  
  Gateway::releasePageMemory(pVertexmap);
  
  pColorMap = (Tuple4ub*) Gateway::aquirePageMemory(size * sizeof(Tuple4ub));
  std::fill(pColorMap, pColorMap + size, Tuple4ub(0xff, 0xff, 0xff, 0xff));
  
  setupTileColors();
  
  Gateway::releasePageMemory(pTileIndexSet);
  Gateway::releasePageMemory(pColorMap);
  
  //compile();
  
  return true;
}

bool PraetoriansTerrainVisuals::loadVisuals(const char* path)
{
  ///unpacked files have higher priority
  const char* p = MediaPathManager::lookUpMediaPath(path);
  if (p)
    return loadUnpackedMedia(p);
    
  if (FileSystem::fileExist(path))
    return loadPackedMedia(path);
    
  return false;
}

bool PraetoriansTerrainVisuals::loadColormap(const char* path)
{
  BMPFileHeader header;
  BMPInfoHeader infoheader;
  unsigned int index;
  unsigned int padding;
  unsigned int skip;
  unsigned int size;
  unsigned char value;
  TileIndexSet *indices;
  Tuple4ub *colors;
  float w;
  float h;
  
  size = (area.x * 2 + 1) * (area.y * 2 + 1);
  
  const char* p = MediaPathManager::lookUpMediaPath(path);
  
  ifstream in(p, ios_base::binary);
  
  if (!in.is_open())
    return Logger::writeErrorLog(String("Could not open ") + p);
    
  in.read((char*)&header.magic, 2);
  
  if (0x4d42 != header.magic)
  {
    in.close();
    return Logger::writeErrorLog(String(path) + " is not a bmp file");
  }
  
  in.read((char*)&header.size, 4);
  in.seekg(4, ios_base::cur);
  in.read((char*)&header.begin, 4);
  in.read((char*)&infoheader.size,        4);
  in.read((char*)&infoheader.width,       4);
  in.read((char*)&infoheader.height,      4);
  in.read((char*)&infoheader.bullshit,    2);
  in.read((char*)&infoheader.bpp,         2);
  in.read((char*)&infoheader.compression, 4);
  in.read((char*)&infoheader.area,        4);
  in.read((char*)&infoheader.resolutionX, 4);
  in.read((char*)&infoheader.resolutionY, 4);
  in.read((char*)&infoheader.colors,      4);
  in.read((char*)&infoheader.icolors,     4);
  in.seekg(256 * 4, std::ios::cur);
  
  w = (infoheader.width - 1) / 2.0f;
  h = (infoheader.height - 1) / 2.0f;
  
  if (infoheader.bpp   != 8 || infoheader.compression != 0 ||
      infoheader.width == 0 || infoheader.height      == 0 ||
      fmod(w, 1.0f)    != 0 || fmod(h, 1.0f)          != 0)
  {
    in.close();
    return Logger::writeErrorLog(String(path) + " contains an incorrect format");
  }
  
  padding = (infoheader.width + 3) & (~3);
  skip = padding - infoheader.width;
  
  pHeightmap = (unsigned char*) Gateway::aquirePageMemory(size);
  memset(pHeightmap, 0, size);
  for (unsigned int i = 0; i < infoheader.height; i++)
  {
    in.read((char*)pHeightmap + infoheader.width * i, infoheader.width);
    in.seekg(skip, std::ios::cur);
  }
  
  pColorMap = (Tuple4ub*) Gateway::aquirePageMemory(size * sizeof(Tuple4ub));
  memset(pColorMap, 0xff, size * sizeof(Tuple4ub));
  for (unsigned int i = 0; i < size; i++)
  {
    value = /*255 - */pHeightmap[i];
    pColorMap[i].set(value, value, value, 255);
  }
  
  Gateway::releasePageMemory(pHeightmap);
  
  pTileIndexSet = (TileIndexSet*) Gateway::aquirePageMemory(area.x * area.y * sizeof(TileIndexSet));
  setupTileIndices();
  
  for (int i = 0; i < area.x * area.y; i++)
  {
    indices = &pTileIndexSet[i];
    colors = tileset(i)->colors;
    
    for (int j = 0; j < 9; j++)
    {
      index = indices->indices[j];
      colors[j] = pColorMap[index];
    }
  }
  
  Gateway::releasePageMemory(pColorMap);
  Gateway::releasePageMemory(pTileIndexSet);
  
  in.close();
  
  return true;
}

bool PraetoriansTerrainVisuals::loadPackedMedia(const char* path)
{
  ICString filename(path);
  
  if (filename.substr(filename.rfind('.') + 1) == "bmp")
    return Logger::writeErrorLog(String("Packed BMP is currently not supported ->") + path);
    
  if (!(file = FileSystem::checkOut(path)))
    return Logger::writeErrorLog(String("Could not checkout archived item -> ") + path);
    
  readHeader();
  
  ///tile loading process
  readTileChunk();
  
  unsigned int size = (area.x * 2 + 1) * (area.y * 2 + 1);
  
  ///elevation loading process
  //elevation must remain static throughout vertex setup process
  pHeightmap = (unsigned char*) Gateway::aquirePageMemory(size);
  readElevationChunk();
  
  ///tile indices loading process
  //indices must remain static throughout entire setup process
  pTileIndexSet = (TileIndexSet*) Gateway::aquirePageMemory(area.x * area.y * sizeof(TileIndexSet));
  setupTileIndices();
  
  ///vertex setup process
  pVertexmap = (Tuple3f*) Gateway::aquirePageMemory(size * sizeof(Tuple3f));
  setupXZSpace();
  setupYSpace();
  setupTileVertices();
  
  Gateway::releasePageMemory(pVertexmap);
  Gateway::releasePageMemory(pHeightmap);
  
  ///color setup process
  pColorMap = (Tuple4ub*) Gateway::aquirePageMemory(size * sizeof(Tuple4ub));
  readColorChunk();
  setupTileColors();
  
  Gateway::releasePageMemory(pColorMap);
  Gateway::releasePageMemory(pTileIndexSet);
  
  FileSystem::checkIn(file);
  
  return true;
}

bool PraetoriansTerrainVisuals::loadUnpackedPVE(const char* path)
{
  ifstream in(path, ios_base::binary);
  if (!in.is_open())
    return Logger::writeErrorLog(String("Could not load -> ") + path);
    
  in.seekg(4, ios_base::cur);
  in.read((char*)&area, 8);
  in.read((char*)&extrema, 8);
  computeHeightRatio();
  
  ///tile loading process
  Tile* tile = (Tile*) Gateway::aquireTerrainMemory(area.x * area.y * sizeof(Tile));
  
  for (int i = 0; i < area.x * area.y; i++)
  {
    in.read((char*)tile[i].textureID, 4);
    in.read((char*)&tile[i].flags, 2);
    in.seekg(2, ios_base::cur);
    tileset.append(&tile[i]);
  }
  
  unsigned int size = (area.x * 2 + 1) * (area.y * 2 + 1);
  
  ///elevation loading process
  //elevation must remain static throughout vertex setup process
  pHeightmap = (unsigned char*) Gateway::aquirePageMemory(size);
  in.read((char*)pHeightmap, size);
  
  ///tile indices loading process
  //indices must remain static throughout entire setup process
  pTileIndexSet = (TileIndexSet*) Gateway::aquirePageMemory(area.x * area.y * sizeof(TileIndexSet));
  setupTileIndices();
  
  ///vertex setup process
  pVertexmap = (Tuple3f*) Gateway::aquirePageMemory(size * sizeof(Tuple3f));
  setupXZSpace();
  setupYSpace();
  setupTileVertices();
  
  Gateway::releasePageMemory(pVertexmap);
  Gateway::releasePageMemory(pHeightmap);
  
  ///color setup process
  pColorMap = (Tuple4ub*) Gateway::aquirePageMemory(size * sizeof(Tuple4ub));
  in.read((char*)pColorMap, size * 4);
  setupTileColors();
  
  Gateway::releasePageMemory(pColorMap);
  Gateway::releasePageMemory(pTileIndexSet);
  
  in.close();
  
  return true;
}

bool PraetoriansTerrainVisuals::loadUnpackedBMP(const char* path)
{
  BMPFileHeader header;
  BMPInfoHeader infoheader;
  unsigned int index;
  unsigned int size;
  unsigned int padding;
  unsigned int skip;
  TileIndexSet* indices;
  Tuple3f* vertices;
  Tuple4ub* colors;
  float w;
  float h;
  
  ifstream in(path, ios_base::binary);
  
  if (!in.is_open())
    return Logger::writeErrorLog(String("Could not open ") + path);
    
  in.read((char*)&header.magic, 2);
  
  if (0x4d42 != header.magic)
  {
    in.close();
    return Logger::writeErrorLog(String(path) + " is not a bmp file");
  }
  
  in.read((char*)&header.size, 4);
  in.seekg(4, ios_base::cur);
  in.read((char*)&header.begin, 4);
  in.read((char*)&infoheader.size,        4);
  in.read((char*)&infoheader.width,       4);
  in.read((char*)&infoheader.height,      4);
  in.read((char*)&infoheader.bullshit,    2);
  in.read((char*)&infoheader.bpp,         2);
  in.read((char*)&infoheader.compression, 4);
  in.read((char*)&infoheader.area,        4);
  in.read((char*)&infoheader.resolutionX, 4);
  in.read((char*)&infoheader.resolutionY, 4);
  in.read((char*)&infoheader.colors,      4);
  in.read((char*)&infoheader.icolors,     4);
  in.seekg(256 * 4, std::ios::cur);
  
  w = (infoheader.width - 1) / 2.0f;
  h = (infoheader.height - 1) / 2.0f;
  
  if (infoheader.bpp   != 8 || infoheader.compression != 0 ||
      infoheader.width == 0 || infoheader.height      == 0 ||
      fmod(w, 1.0f)    != 0 || fmod(h, 1.0f)          != 0)
  {
    in.close();
    return Logger::writeErrorLog(String(path) + " uses an incorrect format");
  }
  
  area.set((int)w, (int)h);
  size = (area.x * 2 + 1) * (area.y * 2 + 1);
  padding = (infoheader.width + 3) & (~3);
  skip = padding - infoheader.width;
  
  pHeightmap = (unsigned char*) Gateway::aquirePageMemory(size);
  for (unsigned int i = 0; i < infoheader.height; i++)
  {
    in.read((char*)pHeightmap + infoheader.width * i, infoheader.width);
    in.seekg(skip, std::ios::cur);
  }
  
  extrema = Gateway::getMapDescriptor().mapElevation;
  computeHeightRatio();
  
  pVertexmap = (Tuple3f*) Gateway::aquirePageMemory(size * sizeof(Tuple3f));
  setupXZSpace();
  setupYSpace();
  
  Gateway::releasePageMemory(pHeightmap);
  
  setupDefaultTileset();
  
  pTileIndexSet = (TileIndexSet*) Gateway::aquirePageMemory(area.x * area.y * sizeof(TileIndexSet));
  setupTileIndices();
  
  for (int i = 0; i < area.x * area.y; i++)
  {
    indices = &pTileIndexSet[i];
    vertices = tileset(i)->vertices;
    colors = tileset(i)->colors;
    
    for (int j = 0; j < 9; j++)
    {
      index = indices->indices[j];
      vertices[j] = pVertexmap[index];
      colors[j].set(0xff,0xff,0xff,0xff);
    }
  }
  
  Gateway::releasePageMemory(pTileIndexSet);
  Gateway::releasePageMemory(pVertexmap);
  
  in.close();
  
  return true;
}

bool PraetoriansTerrainVisuals::loadUnpackedMedia(const char* path)
{
  ICString filename(path);
  ICString fileext = filename.substr(filename.rfind('.') + 1);
  
  if (fileext == "pve")
    return loadUnpackedPVE(path);
    
  if (fileext == "bmp")
    return loadUnpackedBMP(path);
    
  return false;
}

void PraetoriansTerrainVisuals::readHeader()
{
  file->seek(4, SEEKD);
  file->read(area, 8);
  file->read(extrema, 8);
  computeHeightRatio();
}

void PraetoriansTerrainVisuals::readTileChunk()
{
  Tile* tile = (Tile*) Gateway::aquireTerrainMemory(area.x * area.y * sizeof(Tile));
  
  for (int i = 0; i < area.x * area.y; i++)
  {
    file->read(tile[i].textureID, 4);
    file->read(&tile[i].flags, 2);
    file->seek(2, SEEKD);
    tileset.append(&tile[i]);
  }
}

void PraetoriansTerrainVisuals::readElevationChunk()
{
  unsigned int size = (area.x * 2 + 1) * (area.y * 2 + 1);
  file->read(pHeightmap, size);
}

void PraetoriansTerrainVisuals::readColorChunk()
{
  file->read(pColorMap, (area.x * 2 + 1) * (area.y * 2 + 1) * 4);
}

void PraetoriansTerrainVisuals::compile()
{
  setupXZSpace();
  setupTileIndices();
  setupTileVertices();
  setupTileColors();
}

void PraetoriansTerrainVisuals::setupXZSpace()
{
  unsigned int width  = area.x * 2 + 1;
  unsigned int height = area.y * 2 + 1;
  unsigned int index  = 0;
  
  float xPos = 0;
  float zPos = 0;
  
  for (unsigned int row = 0; row < height; row++, xPos = 0, zPos += 0.5f)
    for (unsigned int col = 0; col < width; col++, xPos += 0.5f)
    {
      index = row * width + col;
      pVertexmap[index].x = zPos;
      pVertexmap[index].z = xPos;
    }
}

void PraetoriansTerrainVisuals::setupYSpace()
{
  for (int i = 0; i < (area.x * 2 + 1) * (area.y * 2 + 1); i++)
    pVertexmap[i].y = heightRatio * (float)pHeightmap[i] + extrema.y;
}

void PraetoriansTerrainVisuals::setupTileIndices()
{
  unsigned int width = area.x * 2 + 1;
  unsigned int xOff = 0, yOff = 0, index = 0, i = 0;
  
  for (int a = 0; a < area.y; a++, yOff += 2, xOff = 0)
    for (int b = 0; b < area.x; b++, xOff += 2, index = 0, i++)
      for (unsigned int y = 0; y < 3; y++)
        for (unsigned int x = 0; x < 3; x++, index++)
          pTileIndexSet[i].indices[index] = (yOff + y) * width + (xOff + x);
}

void PraetoriansTerrainVisuals::setupTileVertices()
{
  TileIndexSet *indexset;
  Tuple3f *vertices;
  
  for (int i = 0; i < area.x * area.y; i++)
  {
    indexset = &pTileIndexSet[i];
    vertices = tileset(i)->vertices;
    
    for (int j = 0; j < 9; j++)
      vertices[j] = pVertexmap[indexset->indices[j]];
  }
}

void PraetoriansTerrainVisuals::setupTileColors()
{
  TileIndexSet *indexset;
  Tuple4ub *colors;
  
  for (int i = 0; i < area.x * area.y; i++)
  {
    indexset = &pTileIndexSet[i];
    colors = tileset(i)->colors;
    
    for (int j = 0; j < 9; j++)
      colors[j] = pColorMap[indexset->indices[j]];
  }
}

bool PraetoriansTerrainVisuals::exportData(const char* projectName)
{
  bool success[2] = {false};
  
  unsigned int pixwidth = area.x * 2 + 1;
  unsigned int pixheight = area.y * 2 + 1;
  unsigned int size = pixwidth * pixheight;
  pColorMap = (Tuple4ub*) Gateway::aquirePageMemory(size * sizeof(Tuple4ub));
  pHeightmap = (unsigned char*) Gateway::aquirePageMemory(size);
  memset(pColorMap, 0xff, size * sizeof(Tuple4ub));
  memset(pHeightmap, 0, size);
  success[0] = exportPVEData(projectName);
  success[1] = exportBMPData(projectName);
  Gateway::releasePageMemory(pHeightmap);
  Gateway::releasePageMemory(pColorMap);
  
  exportMiniMap(projectName);
  
  return (success[0] && success[1]);
}

bool PraetoriansTerrainVisuals::exportMiniMap(const char* name)
{
  Tuple2f* tcoords;
  int txid;
  int mwidth = area.y * 2;
  int mheight = area.x * 2;
  unsigned char TGAheader[12] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  unsigned char infoHeader[6];
  infoHeader[0] = (mwidth  & 0x00FF);
  infoHeader[1] = (mwidth  & 0xFF00) >> 8;
  infoHeader[2] = (mheight & 0x00FF);
  infoHeader[3] = (mheight & 0xFF00) >> 8;
  infoHeader[4] = 32;
  infoHeader[5] = 0;
  unsigned char *data;
  String path = Gateway::getExportPath();
  
  ofstream out((path + "Mapas/Descripcion/" + name + ".TGA").getBytes(), ios_base::binary);
  if (!out.is_open())
    return Logger::writeWarningLog(String("Could not export minimap - >") + name + ".TGA");
    
  Tuple4i viewport;
  glClearColor(0,0,0,0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0, area.y, 0, area.x, 1000, -1000);
  
  glGetIntegerv(GL_VIEWPORT, viewport);
  glViewport(0, 0, area.y * 2, area.x * 2);
  
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glRotatef(-90,1,0,0);
  
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  
  for (unsigned int i = 0; i < 2; i++)
  {
    if (i>0)
    {
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    for (int x = 0; x < area.y * area.x; x++)
    {
      txid = tileset(x)->textureID[i];
      if (txid == -1)
        continue;
        
      tcoords = Gateway::getTerrainTextures()->getTextureCoords(txid - (int(txid / 16) * 16));
      txid = Gateway::getTerrainTextures()->getTextureID(int(txid / 16));
      
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, txid);
      glTexCoordPointer(2, GL_FLOAT, 0, tcoords);
      glVertexPointer(3, GL_FLOAT, 0, tileset(x)->vertices);
      glColorPointer(4, GL_UNSIGNED_BYTE, 0, tileset(x)->colors);
      glDrawElements(GL_TRIANGLE_FAN, 10, GL_UNSIGNED_SHORT, getTileIndices(0));
      glDisable(GL_TEXTURE_2D);
    }
    if (i==1)
      glDisable(GL_BLEND);
  }
  
  glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  
  data = (unsigned char*) Gateway::aquirePageMemory(4 * mwidth * mheight);
  glReadPixels(0, 0, mwidth, mheight, GL_BGRA, GL_UNSIGNED_BYTE, data);
  out.write((char*)TGAheader, 12);
  out.write((char*)infoHeader, 6);
  out.write((char*)data, 4 * mwidth * mheight);
  out.close();
  Gateway::releasePageMemory(data);
  
  glClearColor(0,0,0,1);
  glViewport(0, 0, viewport.z, viewport.w);
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  
  return true;
}

bool PraetoriansTerrainVisuals::exportPVEData(const char* fn)
{
  unsigned int index;
  TileIndexSet *indices;
  Tuple3f *vertices;
  Tuple4ub *colors;
  Tile* tile;
  unsigned int sig = 0x00003333;
  unsigned char ex = 0x00;
  unsigned short mhex = 0x0000;
  unsigned int pixwidth = area.x * 2 + 1;
  unsigned int pixheight = area.y * 2 + 1;
  unsigned int pixelindex;
  unsigned int size = pixwidth * pixheight;
  String path = Gateway::getExportPath();
  
  ofstream out((path + "Mapas/" + fn + ".PVE").getBytes(), ios_base::binary);
  
  if (!out.is_open())
    return false;
    
  pTileIndexSet = (TileIndexSet*) Gateway::aquirePageMemory(area.x * area.y * sizeof(TileIndexSet));
  memset(pTileIndexSet, 0, area.x * area.y * sizeof(TileIndexSet));
  
  setupTileIndices();
  computeExtrema();
  computeHeightRatio();
  
  if (heightRatio == 0.0f)
    heightRatio = 1.0f;
    
  for (int i = 0; i < area.x * area.y; i++)
  {
    indices = &pTileIndexSet[i];
    vertices = tileset(i)->vertices;
    colors = tileset(i)->colors;
    
    for (int j = 0; j < 9; j++)
    {
      index = indices->indices[j];
      pHeightmap[index] = (unsigned char)((vertices[j].y - extrema.y) / heightRatio);
      pColorMap[index] = colors[j];
    }
  }
  
  for (unsigned int x = 0; x < pixwidth; x++)
  {
    pixelindex = 0 * pixwidth + x;
    pColorMap[pixelindex].set(0,0,0,pColorMap[pixelindex].w);
    pixelindex = (pixheight - 1) * pixwidth + x;
    pColorMap[pixelindex].set(0,0,0,pColorMap[pixelindex].w);
  }
  
  for (unsigned int y = 0; y < pixheight; y++)
  {
    pixelindex = y * pixwidth + 0;
    pColorMap[pixelindex].set(0,0,0,pColorMap[pixelindex].w);
    pixelindex = y * pixwidth + (pixwidth - 1);
    pColorMap[pixelindex].set(0,0,0,pColorMap[pixelindex].w);
  }
  
  out.write((char*)&sig, 4);
  out.write((char*)&area, 8);
  out.write((char*)&extrema, 8);
  
  unsigned char th;
  
  for (unsigned int i = 0; i < tileset.length(); i++)
  {
    tile = tileset(i);
    th = (char)((getTileExtrema(i).x - extrema.y) / heightRatio);
    out.write((char*)tile->textureID, 4);
    out.write((char*)&tile->flags, 2);
    out.write((char*)&th, 1);
    out.write((char*)&ex, 1);
  }
  
  out.write((char*)pHeightmap, size);
  out.write((char*)pColorMap, size * sizeof(Tuple4ub));
  out.close();
  
  Gateway::releasePageMemory(pTileIndexSet);
  
  return true;
}

bool PraetoriansTerrainVisuals::exportBMPData(const char* fn)
{
  unsigned int size = (area.x * 2 + 1) * (area.y * 2 + 1);
  unsigned short bmpsig = 0x4d42;
  unsigned int bmpsize = 40;
  unsigned short bmpreserved = 0;
  unsigned int bmpoff = 0x0436;
  
  unsigned int bmphsize = 0x28;
  unsigned int bmpwidth = area.x * 2 + 1;
  unsigned int bmpheight = area.y * 2 + 1;
  unsigned short bmpplanes = 0;
  unsigned short bmpbpp = 8;
  unsigned int bmpcomp = 0;
  unsigned int bmpszimg = 0;
  unsigned int bmpxppm = 0;
  unsigned int bmpyppm = 0;
  unsigned int bmpclrs = 0;
  unsigned int bmpiclrs = 0;
  
  Tuple4ub bmpcol;
  unsigned char xxx;
  unsigned int padding = (bmpwidth + 3) & (~3);
  unsigned int skip = padding - bmpwidth;
  unsigned int bmppad = 0;
  
  ///height map
  String path = Gateway::getExportPath();
  
  ofstream out((path + "Mapas/" + fn + ".bmp").getBytes(), ios_base::binary);
  
  out.write((char*)&bmpsig, 2);
  out.write((char*)&bmpsize, 4);
  out.write((char*)&bmpreserved, 2);
  out.write((char*)&bmpreserved, 2);
  out.write((char*)&bmpoff, 4);
  
  out.write((char*)&bmphsize, 4);
  out.write((char*)&bmpwidth, 4);
  out.write((char*)&bmpheight, 4);
  out.write((char*)&bmpplanes, 2);
  out.write((char*)&bmpbpp, 2);
  out.write((char*)&bmpcomp, 4);
  out.write((char*)&bmpszimg, 4);
  out.write((char*)&bmpxppm, 4);
  out.write((char*)&bmpyppm, 4);
  out.write((char*)&bmpclrs, 4);
  out.write((char*)&bmpiclrs, 4);
  
  for (unsigned int i = 0; i < 256; i++)
  {
    xxx = (unsigned char) i;
    bmpcol.set(xxx, xxx, xxx, 0);
    out.write((char*)&bmpcol, 4);
  }
  
  for (unsigned int i = 0; i < bmpheight; i++)
  {
    out.write((char*)pHeightmap + bmpwidth * i, bmpwidth);
    out.write((char*)&bmppad, skip);
  }
  
  out.close();
  
  ///color map
  out.open((path + "Mapas/" + fn + "_color.bmp").getBytes(), ios_base::binary);
  
  out.write((char*)&bmpsig, 2);
  out.write((char*)&bmpsize, 4);
  out.write((char*)&bmpreserved, 2);
  out.write((char*)&bmpreserved, 2);
  out.write((char*)&bmpoff, 4);
  
  out.write((char*)&bmphsize, 4);
  out.write((char*)&bmpwidth, 4);
  out.write((char*)&bmpheight, 4);
  out.write((char*)&bmpplanes, 2);
  out.write((char*)&bmpbpp, 2);
  out.write((char*)&bmpcomp, 4);
  out.write((char*)&bmpszimg, 4);
  out.write((char*)&bmpxppm, 4);
  out.write((char*)&bmpyppm, 4);
  out.write((char*)&bmpclrs, 4);
  out.write((char*)&bmpiclrs, 4);
  
  for (unsigned int i = 0; i < 256; i++)
  {
    xxx = (char) i;
    bmpcol.set(xxx, xxx, xxx, 0);
    out.write((char*)&bmpcol, 4);
  }
  
  for (unsigned int i = 0; i < size; i++)
    pHeightmap[i] = pColorMap[i].x;
    
  for (unsigned int i = 0; i < bmpheight; i++)
  {
    out.write((char*)pHeightmap + bmpwidth * i, bmpwidth);
    out.write((char*)&bmppad, skip);
  }
  
  out.close();
  
  return true;
}

Tuple3f PraetoriansTerrainVisuals::getTileMinEnd(unsigned int index)
{
  return tileset(index)->vertices[0];
}

Tuple3f PraetoriansTerrainVisuals::getTileMaxEnd(unsigned int index)
{
  return tileset(index)->vertices[8];
}

Tuple3f* PraetoriansTerrainVisuals::getTileVertices(unsigned int index)
{
  return tileset(index)->vertices;
}

Tuple4ub* PraetoriansTerrainVisuals::getTileColors(unsigned int index)
{
  return tileset(index)->colors;
}

Tile* PraetoriansTerrainVisuals::getTile(unsigned int index)
{
  return tileset(index);
}

Tuple2i PraetoriansTerrainVisuals::getArea()
{
  return area;
}

Tuple2f PraetoriansTerrainVisuals::getExtrema()
{
  return extrema;
}

float PraetoriansTerrainVisuals::getHeightRatio()
{
  return heightRatio;
}

void PraetoriansTerrainVisuals::computeHeightRatio()
{
  heightRatio = (extrema.x - extrema.y) / 255.0f;
}

void PraetoriansTerrainVisuals::computeExtrema()
{
  float min = 1000;
  float max = -1000;
  Tuple2f newdepth;
  
  for (unsigned int i = 0; i < tileset.length(); i++)
  {
    newdepth = getTileExtrema(i);
    max = max > newdepth.x ? max : newdepth.x;
    min = min < newdepth.y ? min : newdepth.y;
  }
  
  extrema.set(max, min);
}

Tuple2f PraetoriansTerrainVisuals::getTileExtrema(unsigned int index)
{
  float max = -1000;
  float min = 1000;
  float element;
  
  for (int i = 0; i < 9; i++)
  {
    element = tileset(index)->vertices[i].y;
    max = max > element ? max : element;
    min = min < element ? min : element;
  }
  
  return Tuple2f(max, min);
}

float PraetoriansTerrainVisuals::getTileSize()
{
  return 0.5f;
}

unsigned short* PraetoriansTerrainVisuals::getTileIndices(unsigned int index)
{
  return indices;
}

void PraetoriansTerrainVisuals::setupDefaultTileset()
{
  Tile* tile = (Tile*) Gateway::aquireTerrainMemory(area.x * area.y * sizeof(Tile));
  for (int i = 0; i < area.x * area.y; i++)
  {
    tile[i].setSecondaryTextureID(-1);
    tile[i].setPrimaryTextureID(-1);
    tileset.append(&tile[i]);
  }
}

void PraetoriansTerrainVisuals::setExtrema(const Tuple2f& ext)
{
  extrema = ext;
}

void PraetoriansTerrainVisuals::setExtrema(float max, float min)
{
  extrema.set(max, min);
}

void PraetoriansTerrainVisuals::reset()
{
  area.set(0);
  extrema.set(0);
  heightRatio = 1;
  if (!tileset.isEmpty())
    Gateway::releaseTerrainMemory(tileset[0]);
  tileset.clear();
}

PraetoriansTerrainVisuals::~PraetoriansTerrainVisuals()
{
  reset();
}
