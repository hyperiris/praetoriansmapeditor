#include "TileTextureCoords.h"

TileTextureCoords::TileTextureCoords()
{}

TileTextureCoords::TileTextureCoords(const TileTextureCoords &copy)
{
  operator = (copy);
}

TileTextureCoords::TileTextureCoords(const Tuple4i &bounds)
{
  computeTextureCoords(bounds.x, bounds.y, bounds.z, bounds.w);
}

TileTextureCoords::TileTextureCoords(int xOff, int yOff, int zOff, int wOff)
{
  computeTextureCoords(xOff, yOff, zOff, wOff);
}

TileTextureCoords& TileTextureCoords::operator = (const TileTextureCoords & right)
{
  if (this != &right)
    std::copy(right.textCoords, right.textCoords + 10, textCoords);
    
  return *this;
}

void TileTextureCoords::computeTextureCoords(const Tuple4i &bounds)
{
  computeTextureCoords(bounds.x, bounds.y, bounds.z, bounds.w);
}

void TileTextureCoords::computeTextureCoords(int x, int y, int z, int w)
{
  float xOffset = (float)x / 256;
  float yOffset = (float)y / 256;
  float zOffset = (float)z / 256;
  float wOffset = (float)w / 256;
  float iOffset = (float)(x + (64 / 2)) / 256;
  float jOffset = (float)(y + (64 / 2)) / 256;
  
  textCoords[0].set(xOffset, yOffset);
  textCoords[1].set(iOffset, yOffset);
  textCoords[2].set(zOffset, yOffset);
  textCoords[3].set(xOffset, jOffset);
  textCoords[4].set(iOffset, jOffset);
  textCoords[5].set(zOffset, jOffset);
  textCoords[6].set(xOffset, wOffset);
  textCoords[7].set(iOffset, wOffset);
  textCoords[8].set(zOffset, wOffset);
  textCoords[9].set(zOffset, wOffset);
}

void TileTextureCoords::rotate(float angle)
{
}

void TileTextureCoords::transformCoords(Tuple2f *coords)
{
}

void TileTextureCoords::transformImage(unsigned int flags)
{
  Tuple2f rotatedCoords[10],
  mirroredCoords[10];
  
  if (flags & TileFlags::TEXTURE_MIRRORX)
  {
    mirroredCoords[0] = textCoords[0];
    mirroredCoords[1] = textCoords[3];
    mirroredCoords[2] = textCoords[6];
    mirroredCoords[3] = textCoords[2];
    mirroredCoords[4] = textCoords[5];
    mirroredCoords[5] = textCoords[8];
    mirroredCoords[6] = textCoords[2];
    
    textCoords[0] = mirroredCoords[3];
    textCoords[3] = mirroredCoords[4];
    textCoords[6] = mirroredCoords[5];
    textCoords[2] = mirroredCoords[0];
    textCoords[5] = mirroredCoords[1];
    textCoords[8] = mirroredCoords[2];
    textCoords[9] = mirroredCoords[6];
  }
  
  if (flags & TileFlags::TEXTURE_MIRRORY)
  {
    mirroredCoords[0] = textCoords[0];
    mirroredCoords[1] = textCoords[1];
    mirroredCoords[2] = textCoords[2];
    mirroredCoords[3] = textCoords[6];
    mirroredCoords[4] = textCoords[7];
    mirroredCoords[5] = textCoords[8];
    mirroredCoords[6] = textCoords[6];
    
    textCoords[0] = mirroredCoords[3];
    textCoords[1] = mirroredCoords[4];
    textCoords[2] = mirroredCoords[5];
    textCoords[6] = mirroredCoords[0];
    textCoords[7] = mirroredCoords[1];
    textCoords[8] = mirroredCoords[2];
    textCoords[9] = mirroredCoords[6];
  }
  
  if (flags & TileFlags::TEXTURE_ROTATE90)
  {
    rotatedCoords[0] = textCoords[2];
    rotatedCoords[1] = textCoords[5];
    rotatedCoords[2] = textCoords[8];
    rotatedCoords[3] = textCoords[1];
    rotatedCoords[4] = textCoords[4];
    rotatedCoords[5] = textCoords[7];
    rotatedCoords[6] = textCoords[0];
    rotatedCoords[7] = textCoords[3];
    rotatedCoords[8] = textCoords[6];
    rotatedCoords[9] = textCoords[2];
    
    for (unsigned int i = 0; i < 10; i++)
      textCoords[i] = rotatedCoords[i];
  }
  
  if (flags & TileFlags::TEXTURE_ROTATE180)
  {
    rotatedCoords[0] = textCoords[8];
    rotatedCoords[1] = textCoords[7];
    rotatedCoords[2] = textCoords[6];
    rotatedCoords[3] = textCoords[5];
    rotatedCoords[4] = textCoords[4];
    rotatedCoords[5] = textCoords[3];
    rotatedCoords[6] = textCoords[2];
    rotatedCoords[7] = textCoords[1];
    rotatedCoords[8] = textCoords[0];
    rotatedCoords[9] = textCoords[8];
    
    for (unsigned int i = 0; i < 10; i++)
      textCoords[i] = rotatedCoords[i];
  }
  
  if (flags & TileFlags::TEXTURE_ROTATE270)
  {
    rotatedCoords[0] = textCoords[6];
    rotatedCoords[1] = textCoords[3];
    rotatedCoords[2] = textCoords[0];
    rotatedCoords[3] = textCoords[7];
    rotatedCoords[4] = textCoords[4];
    rotatedCoords[5] = textCoords[1];
    rotatedCoords[6] = textCoords[8];
    rotatedCoords[7] = textCoords[5];
    rotatedCoords[8] = textCoords[2];
    rotatedCoords[9] = textCoords[6];
    
    for (unsigned int i = 0; i < 10; i++)
      textCoords[i] = rotatedCoords[i];
  }
}

Tuple2f *TileTextureCoords::getTextureCoords()
{
  return textCoords;
}

TileTextureCoords::~TileTextureCoords()
{
}
