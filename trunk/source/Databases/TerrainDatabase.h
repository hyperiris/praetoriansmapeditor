#ifndef TERRAINDATABASE
#define TERRAINDATABASE

#include "../Factories/FactoryUtils.h"
#include "../Containers/ocarray.h"

class TileController;
class TileGraph;
class Visitor;

class TerrainDatabase
{
  public:
    TerrainDatabase();
    ~TerrainDatabase();
    
  public:
    bool initialize();
    void compile();
    void reset();
    
    void inspectGraph(unsigned int level, Visitor *visitor);
    
    void controllerVisibility(unsigned int index, bool visible);
    
    TileController* getController(unsigned int index);
    unsigned int getControllerCount();
    
    void changeTileTexture(unsigned int level, int index, unsigned int contrIndex, bool visible = false);
    
    void clearAlphaMap();
    void clearColorMap();
    
  private:
    void setupGraph();
    void finalize();
    
  private:
    TileGraph* tileGraph;
    ArrayPtr <TileController> tileControllers;
};

#endif