#ifndef TERRAINVISUALS
#define TERRAINVISUALS

#include "../Math/Tuple4.h"

class Tile;

class TerrainVisuals
{
  public:
    TerrainVisuals() {}
    virtual ~TerrainVisuals() {}
    
  public:
    virtual bool createHeightMap(unsigned int width, unsigned int height) = 0;
    
  public:
    virtual bool loadVisuals(const char* path) = 0;
    virtual bool loadColormap(const char* path) = 0;
    virtual bool exportData(const char* projectName) = 0;
    
    virtual void setExtrema(const Tuple2f& ext) = 0;
    virtual void setExtrema(float max, float min) = 0;
    
    virtual Tuple3f getTileMinEnd(unsigned int index) = 0;
    virtual Tuple3f getTileMaxEnd(unsigned int index) = 0;
    virtual unsigned short* getTileIndices(unsigned int index) = 0;
    virtual Tuple3f* getTileVertices(unsigned int index) = 0;
    virtual Tuple4ub* getTileColors(unsigned int index) = 0;
    virtual Tile* getTile(unsigned int index) = 0;
    virtual float getTileSize() = 0;
    
    virtual Tuple2i getArea() = 0;
    virtual Tuple2f getExtrema() = 0;
    virtual float getHeightRatio() = 0;
    
    virtual void reset() = 0;
};

#endif
