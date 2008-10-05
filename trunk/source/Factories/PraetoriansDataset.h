#ifndef PRAETORIANSDATASET
#define PRAETORIANSDATASET

#include "Dataset.h"

class PraetoriansDataset : public Dataset
{
  public:
    PraetoriansDataset();
    ~PraetoriansDataset();
    
  public:
    bool load(const MapDescriptor& descriptor);
    bool initialize();
    void reset();
    
    bool exportData(const char* projectName);
    
    TerrainTextures* createTerrainTextures();
    TerrainVisuals* createTerrainVisuals();
    TerrainLogic* createTerrainLogic();
    TerrainPasture* createTerrainPasture();
    TerrainWater* createTerrainWater();
    
    WorldVisuals* createWorldVisuals();
    
    MissionScript* createMissionScript();
    
  private:
    TerrainTextures* terrainTextures;
    TerrainVisuals* terrainVisuals;
    TerrainLogic* terrainLogic;
    TerrainPasture* terrainPasture;
    TerrainWater* terrainWater;
    
    WorldVisuals* worldVisuals;
    
    MissionScript* missionScript;
};

#endif