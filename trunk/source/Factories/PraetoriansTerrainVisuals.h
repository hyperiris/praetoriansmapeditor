#ifndef PRAETORIANSTERRAINVISUALS
#define PRAETORIANSTERRAINVISUALS

#include "TerrainVisuals.h"
#include "../Containers/ocarray.h"
//#include "../Memory/GenericPool.h"

class ArchivedFile;
struct TileIndexSet;

class PraetoriansTerrainVisuals : public TerrainVisuals
{
  public:
    PraetoriansTerrainVisuals();
    ~PraetoriansTerrainVisuals();
    
  public:
    bool loadVisuals(const char* path);
    bool loadColormap(const char* path);
    bool exportData(const char* projectName);
    bool createHeightMap(unsigned int width, unsigned int height);
    
    void setExtrema(const Tuple2f& ext);
    void setExtrema(float max, float min);
    
    Tuple3f getTileMinEnd(unsigned int index);
    Tuple3f getTileMaxEnd(unsigned int index);
    unsigned short* getTileIndices(unsigned int index);
    Tuple3f* getTileVertices(unsigned int index);
    Tuple4ub* getTileColors(unsigned int index);
    Tile* getTile(unsigned int index);
    float getTileSize();
    
    Tuple2i getArea();
    Tuple2f getExtrema();
    float getHeightRatio();
    
    void reset();
    
  private:
    void readHeader();
    void readTileChunk();
    void readColorChunk();
    void readElevationChunk();
    
    bool exportPVEData(const char* fn);
    bool exportBMPData(const char* fn);
    bool exportMiniMap(const char* name);
    
    bool loadPackedMedia(const char* path);
    bool loadUnpackedMedia(const char* path);
    bool loadUnpackedPVE(const char* path);
    bool loadUnpackedBMP(const char* path);
    
    void compile();
    void setupXZSpace();
    void setupYSpace();
    void setupTileIndices();
    void setupTileVertices();
    void setupTileColors();
    void setupDefaultTileset();
    
    void computeExtrema();
    void computeHeightRatio();
    Tuple2f getTileExtrema(unsigned int index);// [0] = max, [1] = min
    
  private:
    static unsigned short indices[10];
    
    ArchivedFile *file;
    
    Tuple2i area;
    Tuple2f extrema;
    float heightRatio;
    
    ArrayPtr <Tile> tileset;
    
    Tuple4ub* pColorMap;
    Tuple3f* pVertexmap;
    TileIndexSet* pTileIndexSet;
    unsigned char* pHeightmap;
    
    //GenericPool <Tile, 40000, 10000> tilePool;
};

#endif
