#ifndef PRAETORIANSTERRAINMEADOWS
#define PRAETORIANSTERRAINMEADOWS

#include "TerrainPasture.h"
#include "../Containers/ocarray.h"
#include "../Math/Tuple4.h"
#include "../Tools/String.h"
#include "Meadow.h"

class ArchivedFile;
class Meadow;

class PraetoriansTerrainPasture : public TerrainPasture
{
  public:
    PraetoriansTerrainPasture();
    ~PraetoriansTerrainPasture();
    
  public:
    bool load(const char* path);
    void reset();
    bool exportData(const char* projectName);
    
    unsigned int getMeadowCount();
    Meadow* getMeadow(unsigned int index);
    Meadow* getMeadow(const char* name);
    int getMeadowIndex(const char* name);
    
    Meadow* createMeadow();
    GrassPatch* createPatch(unsigned int meadowIdx);
    GrassPatch* createPatch(const char* meadowName);
    
    void clearMeadowPatches(const char* meadowName);
    
    const char* getName();
    void setName(const char* name);
    
  private:
    void readMeadowsChunk();
    bool loadPackedMedia(const char* path);
    bool loadUnpackedMedia(const char* path);
    
  private:
    ArchivedFile* file;
    String pasturename;
    ArrayPtr <Meadow> meadowsList;
};

#endif