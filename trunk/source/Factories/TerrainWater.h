#ifndef TERRAINWATER
#define TERRAINWATER

class Water;

class TerrainWater
{
  public:
    TerrainWater() {}
    virtual ~TerrainWater() {}
    
  public:
    virtual bool load(const char* path) = 0;
    virtual bool exportData(const char* projectName) = 0;
    virtual void reset() = 0;
    virtual Water* createWater() = 0;
    virtual void addWater(Water *water) = 0;
    virtual bool destroyAllWaters() = 0;
    virtual bool destroyWater(const char* name) = 0;
    virtual unsigned int getWaterCount() = 0;
    virtual Water* getWater(unsigned int index) = 0;
};

#endif