#ifndef TILECONTROLLER
#define TILECONTROLLER

#include "../Containers/ocarray.h"
#include "../Math/Tuple4.h"

class Tile;
class TileGraphNode;

class TileController
{
  public:
    TileController();
    ~TileController();
    
  public:
    void setTile(Tile* tilemodel);
    void setTextureID(unsigned int index, int id);
    void setFlags(unsigned short flags);
    void setVisible(bool vis);
    
    void setGrass(bool value);
    void setBoundary(bool value);
    
    void addGraphNode(TileGraphNode* node);
    TileGraphNode* getGraphNode(unsigned int index);
    
    Tuple3f* getVertices();
    Tuple4ub* getColors();
    
    unsigned short getFlags();
    
    bool isBoundary();
    bool isVisible();
    bool hasGrass();
    
    void clearFlags();
    
    void reset();
    
  private:
    Tile* tile;
    ArrayPtr <TileGraphNode> graphNodes;
    bool visible;
};

#endif
