#include "PraetoriansTerrainTextures.h"
#include "../FileSystem/FileSystem.h"
#include "../FileSystem/ArchivedFile.h"
#include "../Managers/ManagersUtils.h"
#include "../Kernel/Gateway.h"
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>

#ifndef GL_BGR
#define GL_BGR GL_BGR_EXT
#endif

PraetoriansTerrainTextures::PraetoriansTerrainTextures()
{}

bool PraetoriansTerrainTextures::load(const char* path)
{
  ///unpacked files have higher priority
  const char* p = MediaPathManager::lookUpMediaPath(path);
  if (p)
    return loadUnpackedMedia(p);
    
  if (FileSystem::fileExist(path))
    return loadPackedMedia(path);
    
  return false;
}

bool PraetoriansTerrainTextures::loadPackedMedia(const char* path)
{
  if (!(file = FileSystem::checkOut(path)))
    return Logger::writeErrorLog(String("Could not check out archived item -> ") + path);
    
  tempBuffer = new unsigned char[256 * 256 * 3];
  
  readHeader();
  setupCoords();
  
  deleteArray(tempBuffer);
  
  FileSystem::checkIn(file);
  
  return true;
}

bool PraetoriansTerrainTextures::loadUnpackedMedia(const char* path)
{
  unsigned int textureCount;
  unsigned int mipmapCount;
  unsigned int availableID;
  unsigned int resolution;
  ifstream in(path, ios_base::binary);
  
  if (!in.is_open())
    return Logger::writeErrorLog(String("Could not load -> ") + path);
    
  tempBuffer = new unsigned char[256 * 256 * 3];
  
  in.read((char*)&textureCount, 4);
  in.seekg(8, ios_base::cur);
  in.read((char*)&mipmapCount,  4);
  
  for (unsigned int i = 0; i < textureCount; i++)
  {
    in.read((char*)tempBuffer, 256 * 256 * 3);
    
    glGenTextures(1, &availableID);
    textureHandles.append(availableID);
    glBindTexture(GL_TEXTURE_2D, availableID);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, 256, 256, 0, GL_BGR, GL_UNSIGNED_BYTE, tempBuffer);
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, 256, 256, GL_BGR, GL_UNSIGNED_BYTE, tempBuffer);
    
    resolution = 2;
    for (unsigned int j = 0; j < mipmapCount; j++, resolution *= 2)
      in.seekg((256 / resolution) * (256 / resolution) * 3, ios_base::cur);
  }
  
  setupCoords();
  
  deleteArray(tempBuffer);
  
  in.close();
  
  return true;
}

bool PraetoriansTerrainTextures::exportData(const char* projectName)
{
  String path = Gateway::getExportPath();
  String tpath = Gateway::getMapDescriptor().mapTextures;
  String filepath(path + "Mapas/Texturas/" + projectName + ".PTE");
  
  if (const char* txpath = MediaPathManager::lookUpMediaPath(tpath))
  {
    ifstream in(txpath, ios_base::binary);
    if (!in.is_open())
      return Logger::writeErrorLog(String("Could not export -> ") + filepath);
    in.seekg(0, ios_base::end);
    unsigned int fsize = in.tellg();
    unsigned char* buff = (unsigned char*) Gateway::aquireFilebufferMemory(fsize);
    in.seekg(0, ios_base::beg);
    in.read((char*)buff, fsize);
    in.close();
    
    ofstream out(filepath.getBytes(), ios_base::binary);
    if (!out.is_open())
      return Logger::writeErrorLog(String("Could not export -> ") + filepath);
    out.write((char*)buff, fsize);
    out.close();
    Gateway::releaseFilebufferMemory(buff);
  }
  else
  {
    if (!(file = FileSystem::checkOut(tpath)))
      return Logger::writeErrorLog(String("Could not export -> ") + filepath);
    ofstream out(filepath.getBytes(), ios_base::binary);
    if (!out.is_open())
    {
      FileSystem::checkIn(file);
      return Logger::writeErrorLog(String("Could not export -> ") + filepath);
    }
    out.write((char*)file->data, file->size);
    out.close();
    FileSystem::checkIn(file);
  }
  
  return true;
}

void PraetoriansTerrainTextures::readHeader()
{
  unsigned int textureCount;
  unsigned int mipmapCount;
  
  file->read(&textureCount, 4);
  file->seek(8, SEEKD);
  file->read(&mipmapCount,  4);
  
  for (unsigned int i = 0; i < textureCount; i++)
  {
    readTexture();
    skipMipMaps(mipmapCount);
    //readMipmaps(mipmapCount);
  }
}

void PraetoriansTerrainTextures::readTexture()
{
  unsigned int availableID;
  
  file->read(tempBuffer, 256 * 256 * 3);
  
  glGenTextures(1, &availableID);
  textureHandles.append(availableID);
  
  glBindTexture(GL_TEXTURE_2D, availableID);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, 3, 256, 256, 0, GL_BGR, GL_UNSIGNED_BYTE, tempBuffer);
  gluBuild2DMipmaps(GL_TEXTURE_2D, 3, 256, 256, GL_BGR, GL_UNSIGNED_BYTE, tempBuffer);
}

void PraetoriansTerrainTextures::setupCoords()
{
  unsigned int xPixOffset = 0;
  unsigned int yPixOffset = 0;
  TileTextureCoords tileCoords;
  Tuple4i bounds;
  
  for (int y = 0; y < 4; y++, xPixOffset = 0, yPixOffset += 64)
    for (int x = 0; x < 4; x++, xPixOffset += 64)
    {
      bounds.set(xPixOffset, yPixOffset, xPixOffset + 64, yPixOffset + 64);
      tileCoords.computeTextureCoords(bounds);
      textureCoords.append(tileCoords);
    }
}

void PraetoriansTerrainTextures::skipMipMaps(unsigned int count)
{
  unsigned int resolution = 2;
  
  for (unsigned int i = 0; i < count; i++, resolution *= 2)
    file->seek((256 / resolution) * (256 / resolution) * 3, SEEKD);
}

void PraetoriansTerrainTextures::readMipmaps(unsigned int count)
{
  unsigned int w = 256;
  unsigned int h = 256;
  unsigned int resolution = 2;
  unsigned int level = 1;
  unsigned int elementsCount = (w/resolution) * (h/resolution) * 3;
  
  for (unsigned int i = 0; i < count; i++)
  {
    file->read(tempBuffer, elementsCount);
    glTexImage2D(GL_TEXTURE_2D, level++, 3, w/resolution, h/resolution, 0, GL_BGR, GL_UNSIGNED_BYTE, tempBuffer);
    resolution *= 2;
    elementsCount = (w/resolution) * (h/resolution) * 3;
  }
}

unsigned int PraetoriansTerrainTextures::getTextureCount()
{
  return textureHandles.length();
}

unsigned int PraetoriansTerrainTextures::getTextureID(unsigned int index)
{
  return textureHandles(index);
}

Tuple2f* PraetoriansTerrainTextures::getTextureCoords(unsigned int index)
{
  return textureCoords(index).getTextureCoords();
}

void PraetoriansTerrainTextures::reset()
{
  for (unsigned int i = 0; i < textureHandles.length(); i++)
    glDeleteTextures(1, &textureHandles(i));
    
  textureCoords.clearFast();
  textureHandles.clearFast();
}

PraetoriansTerrainTextures::~PraetoriansTerrainTextures()
{
  reset();
}