#ifndef TILEGRAPHRENDERER
#define TILEGRAPHRENDERER

#include "Visitor.h"
#include "../Math/Tuple4.h"

class TileGraphRenderer : public Visitor
{
  public:
    TileGraphRenderer();
    ~TileGraphRenderer();
    
  public:
    void visit(Node* node);
    void visit(TextureNode* node);
    void visit(IndicesNode* node);
    void visit(TextureCoordsNode* node);
    void visit(TileGraphNode* node);
    
    void enableBlend(bool enable);
    void enableTexture(bool enable);
    
    void enableWireMode(bool enable);
    
    void reset();
    
  private:
    void transformTMatrix(unsigned short flags);
    void unTransformTMatrix();
    
  private:
    unsigned int *pIndices;
    Tuple2f* pCoords;
    bool blend;
    bool texture;
    bool wireMode;
};

#endif