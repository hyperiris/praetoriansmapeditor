#ifndef DATASET
#define DATASET

#include "FactoryUtils.h"

class TerrainTextures;
class TerrainVisuals;
class TerrainLogic;
class TerrainPasture;
class TerrainWater;

class WorldVisuals;

class MissionScript;

class Dataset
{
  public:
    Dataset() {}
    virtual ~Dataset() {}
    
  public:
    virtual bool load(const MapDescriptor& descriptor) = 0;
    virtual bool initialize() = 0;
    virtual void reset() = 0;
    
    virtual bool exportData(const char* projectName) = 0;
    
    virtual TerrainTextures* createTerrainTextures() = 0;
    virtual TerrainVisuals* createTerrainVisuals() = 0;
    virtual TerrainLogic* createTerrainLogic() = 0;
    virtual TerrainPasture* createTerrainPasture() = 0;
    virtual TerrainWater* createTerrainWater() = 0;
    
    virtual WorldVisuals* createWorldVisuals() = 0;
    
    virtual MissionScript* createMissionScript() = 0;
};

#endif