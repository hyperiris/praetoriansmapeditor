#ifndef PRAETORIANSTERRAINWATER
#define PRAETORIANSTERRAINWATER

#include "TerrainWater.h"
#include "../Containers/ocarray.h"

class PraetoriansTerrainWater : public TerrainWater
{
  public:
    PraetoriansTerrainWater();
    ~PraetoriansTerrainWater();
    
  public:
    bool load(const char* path);
    bool exportData(const char* projectName);
    void reset();
    
    Water* createWater();
    void addWater(Water *water);
    
    bool destroyAllWaters();
    bool destroyWater(const char* name);
    
    unsigned int getWaterCount();
    Water* getWater(unsigned int index);
    
  private:
    bool loadPackedMedia(const char* path);
    bool loadUnpackedMedia(const char* path);
    
  private:
    ArrayPtr <Water> watersList;
};

#endif