#ifndef TILEGRAPH
#define TILEGRAPH

#include "../Math/Tuple4.h"
#include "../Containers/ocarray.h"
#include "../Memory/GenericPool.h"

class Node;
class TextureNode;
class TileGraphNode;
class TextureCoordsNode;
class IndicesNode;
class Visitor;

class AttachmentPoint
{
  public:
    void reset()
    {
      aggregateNodes.clear();
    }
    
    ArrayPtr <TextureCoordsNode> aggregateNodes;
};

class TileGraph
{
  public:
    TileGraph();
    ~TileGraph();
    
  public:
    void initialize(unsigned int nlayers);
    void reset();
    void destroy();
    
    unsigned int integrate(unsigned int layer, unsigned int txid, Tuple2f *coords);
    void inspect(unsigned int layer, Visitor* visitor);
    TileGraphNode* addModelNode(unsigned int layer, int index);
    void moveModelNode(TileGraphNode *node, unsigned int layer, int index, bool hide = false);
    
  private:
    TextureNode* checkForRepeat(TextureNode* node, unsigned int id);
    
  private:
    Array <AttachmentPoint> attatchmentPoints;
    ArrayPtr <Node> nodeList;
    ArrayPtr <Node> layers;
    
    TextureCoordsNode* voidParent;
    
    unsigned int tileindices[10];
    
    GenericPool <Node,                    2,    2> nodePool;
    GenericPool <TextureNode,            10,    5> textureNodePool;
    GenericPool <IndicesNode,             2,    2> indicesNodePool;
    GenericPool <TextureCoordsNode,     150,   16> txcoordsNodePool;
    GenericPool <TileGraphNode,      120000, 1000> tileNodePool;
};

#endif