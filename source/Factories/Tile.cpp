#include "Tile.h"
#include "FactoryUtils.h"

Tile::Tile()
{
  reset();
}

Tile::Tile(const Tile& copy)
{
  textureID[0] = copy.textureID[0];
  textureID[1] = copy.textureID[1];
  flags = copy.flags;
}

Tile& Tile::operator = (const Tile & right)
{
  if (this != &right)
  {
    textureID[0] = right.textureID[0];
    textureID[1] = right.textureID[1];
    flags = right.flags;
  }
  return *this;
}

void Tile::setPrimaryTextureID(short id)
{
  textureID[0] = id;
}

void Tile::setSecondaryTextureID(short id)
{
  textureID[1] = id;
}

void Tile::setFlags(unsigned short bits)
{
  flags = bits;
}

short Tile::getPrimaryTextureID()
{
  return textureID[0];
}

short Tile::getSecondaryTextureID()
{
  return textureID[1];
}

unsigned short Tile::getFlags()
{
  return flags;
}

void Tile::reset()
{
  textureID[0] = -1;
  textureID[1] = -1;
  flags = TileFlags::TILE_FLAT;
  //logic = TileLogic::FLAG_FLAT | (TileTypes::NORMAL << 27);
  
  grass = false;
  boundary = false;
}

Tile::~Tile()
{
}
