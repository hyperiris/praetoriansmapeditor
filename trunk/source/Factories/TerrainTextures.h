#ifndef TERRAINTEXTURES
#define TERRAINTEXTURES

#include "../Math/Tuple4.h"

class TerrainTextures
{
  public:
    TerrainTextures() {}
    virtual ~TerrainTextures() {}
    
  public:
    virtual bool load(const char* path) = 0;
    virtual bool exportData(const char* projectName) = 0;
    virtual void reset() = 0;
    virtual unsigned int getTextureCount() = 0;
    virtual unsigned int getTextureID(unsigned int index) = 0;
    virtual Tuple2f* getTextureCoords(unsigned int index) = 0;
};

#endif
