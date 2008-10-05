#ifndef TERRAINPASTURE
#define TERRAINPASTURE

class Meadow;
class GrassPatch;

class TerrainPasture
{
  public:
    TerrainPasture() {}
    virtual ~TerrainPasture() {}
    
  public:
    virtual bool load(const char* path) = 0;
    virtual void reset() = 0;
    virtual bool exportData(const char* projectName) = 0;
    
    virtual unsigned int getMeadowCount() = 0;
    virtual Meadow* getMeadow(unsigned int index) = 0;
    virtual Meadow* getMeadow(const char* name) = 0;
    virtual int getMeadowIndex(const char* name) = 0;
    
    virtual Meadow* createMeadow() = 0;
    virtual GrassPatch* createPatch(unsigned int meadowIdx) = 0;
    virtual GrassPatch* createPatch(const char* meadowName) = 0;
    
    virtual void clearMeadowPatches(const char* meadowName) = 0;
    
    virtual void setName(const char* name) = 0;
    virtual const char* getName() = 0;
};

#endif