#ifndef PRAETORIANSTERRAINTEXTURES
#define PRAETORIANSTERRAINTEXTURES

#include "TerrainTextures.h"
#include "../Containers/ocarray.h"
#include "../Factories/TileTextureCoords.h"

class ArchivedFile;

class PraetoriansTerrainTextures : public TerrainTextures
{
  public:
    PraetoriansTerrainTextures();
    ~PraetoriansTerrainTextures();
    
  public:
    bool load(const char* path);
    bool exportData(const char* projectName);
    void reset();
    
    unsigned int getTextureCount();
    unsigned int getTextureID(unsigned int index);
    Tuple2f* getTextureCoords(unsigned int index);
    
  private:
    void readHeader();
    void readTexture();
    bool loadPackedMedia(const char* path);
    bool loadUnpackedMedia(const char* path);
    
    void setupCoords();
    void skipMipMaps(unsigned int count);
    void readMipmaps(unsigned int count);
    
  private:
    unsigned char* tempBuffer;
    ArchivedFile *file;
    Array <unsigned int> textureHandles;
    Array <TileTextureCoords> textureCoords;
};

#endif