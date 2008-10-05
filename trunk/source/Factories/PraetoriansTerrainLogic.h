#ifndef PRAETORIANSTERRAINLOGIC
#define PRAETORIANSTERRAINLOGIC

#include "TerrainLogic.h"
#include "../Containers/ocarray.h"
#include "../Math/Tuple2.h"

class PraetoriansTerrainLogic : public TerrainLogic
{
  public:
    PraetoriansTerrainLogic();
    ~PraetoriansTerrainLogic();
    
  public:
    bool          load(const char* path);
    bool          exportData(const char* projectName);
    
    bool          createLogicMap(unsigned int width, unsigned height);
    unsigned int  getFlags(unsigned int index);
    unsigned int  getFlagCount();
    void          setFlags(unsigned int index, unsigned int flags);
    void          reset();
    
  private:
    bool loadPackedMedia(const char* path);
    bool loadUnpackedMedia(const char* path);
    
  private:
    Array <unsigned int> logicFlags;
    Tuple2i area;
};

#endif