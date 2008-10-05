#ifndef TERRAINBRUSH
#define TERRAINBRUSH

#include "Visitor.h"
#include "../Math/Intersector.h"
#include "../Containers/ocavltreet.h"
#include "../Tools/String.h"

class BoundsDescriptor;
class TransformGroup;
class BrushStrategy;
class Water;

class TerrainBrush : public Visitor
{
  public:
    TerrainBrush();
    ~TerrainBrush();
    
  public:
    void visit(SpatialIndexBaseNode* node);
    void visit(SpatialIndexNode* node);
    void visit(SpatialIndexCell* node);
    
    void setRay(Ray3D* r);
    
    void setPaintIndex(int index);
    void setPaintLayer(int layer);
    void setMeadowName(const char* name);
    void setLogic(unsigned int flags);
    
    void setStrength(float s);
    void setRadius(float r);
    void setOpacity(float o);
    
    void setNatureTransformGroup(TransformGroup* group);
    void setCharacterTransformGroup(TransformGroup* group);
    void setCritterTransformGroup(TransformGroup* group);
    void setStructureTransformGroup(TransformGroup* group);
    void setVillageTransformGroup(TransformGroup* group);
    
    void setWaterModel(Water* water);
    
    void setMode(int m);
    unsigned int getMode();
    
    void setType(int m);
    unsigned int getType();
    
    void setLayer(int m);
    int getLayer();
    
    void enable(bool e);
    void enableLogicFlag(bool enable);
    void enableConstant(bool enable);
    void enableGuides(bool enable);
    
    Tuple4f getCollisionPoint();
    void saveCollisionPoint(bool save);
    void drawCollisionMark();
    
    void update(float tick);
    
    void reset();
    
  private:
    bool intersect(BoundsDescriptor *bounds);
    void editTiles();
    void editGrass();
    void editWater();
    void editModel();
    
  private:
    bool showGuides;
    bool enabled;
    bool constant;
    
    bool saveCollPoint;
    
    int txcoordsIndex;
    unsigned int level;
    String meadowname;
    
    unsigned int logicflag;
    
    int mode;
    int type;
    int layer;
    
    float radius;
    float strength;
    float time;
    float opacity;
    
    Tuple3f cpoint;
    Intersector intersector;
    Ray3D* ray;
    
    TransformGroup* natureRoot;
    TransformGroup* characterRoot;
    TransformGroup* critterRoot;
    TransformGroup* structureRoot;
    TransformGroup* villageRoot;
    
    Water* waterModel;
    
    AVLTreeT <float, Tuple3f, 4> pointAVLTree;
    Array <BrushStrategy*> strategies;
    BrushStrategy* strategy;
};

#endif