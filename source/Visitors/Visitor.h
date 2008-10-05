#ifndef VISITOR
#define VISITOR

class Node;
class TextureNode;
class IndicesNode;
//class VerticesNode;
//class FacegroupNode;
//class AppearanceNode;
class TextureCoordsNode;
//class TransformGroupNode;
class SpatialIndexNode;
class SpatialIndexCell;
class SpatialIndexBaseNode;
//class StaticModelNode;
class TileGraphNode;
class TransformGroupNode;
//class JointNode;

class Visitor
{
  public:
    Visitor() {}
    virtual ~Visitor() {}
    
  public:
    virtual void visit(Node* node) {}
    virtual void visit(TextureNode* node) {}
    virtual void visit(IndicesNode* node) {}
    //virtual void visit(VerticesNode* node) {}
    //virtual void visit(FacegroupNode* node) {}
    //virtual void visit(AppearanceNode* node) {}
    virtual void visit(TextureCoordsNode* node) {}
    //virtual void visit(TransformGroupNode* node) {}
    
    virtual void visit(SpatialIndexBaseNode* node) {}
    virtual void visit(SpatialIndexNode* node) {}
    virtual void visit(SpatialIndexCell* node) {}
    //virtual void visit(StaticModelNode* node) {}
    
    virtual void visit(TileGraphNode* node) {}
    virtual void visit(TransformGroupNode* node) {}
    //virtual void visit(JointNode* node) {}
    virtual void reset() {}
};

#endif