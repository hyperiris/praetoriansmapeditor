#ifndef WATERDATABASE
#define WATERDATABASE

#include "../Containers/ocarray.h"
#include "../Appearance/Texture.h"

class Water;
class Model;
class TransformGroup;

class WaterDatabase
{
  public:
    WaterDatabase();
    ~WaterDatabase();
    
  public:
    bool initialize(const char* path);
    void compile();
    void reset();
    
    void addWaterModel(Water* water);
    void addWaterModel(Model* model);
    void addWaterModel(TransformGroup* group);
    unsigned int getWatersCount();
    TransformGroup* getRootGroup();
    
    Texture& getWaterTexture();
    
  protected:
    TransformGroup* rootGroup;
    ArrayPtr <Water> watersList;
    Texture watertexture;
};

#endif