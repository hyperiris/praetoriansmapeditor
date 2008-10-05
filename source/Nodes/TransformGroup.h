#ifndef TRANSFORMGROUP
#define TRANSFORMGROUP

#include "../Parsers/IOXMLObject.h"
#include "../Containers/ocarray.h"
#include "../Geometry/BoundsDescriptor.h"
#include "../Geometry/DistanceObject.h"
#include "Transform3D.h"

class Ray3D;
class Model;
class Frustum;
class TransformGroup;

struct RenderObjectOrderedList
{
  RenderObjectOrderedList(int i = 100) : groups(i), models(i) {}
  int textureID;
  ArrayPtr <TransformGroup> groups;
  ArrayPtr <Model> models;
};

class TransformGroup : public IOXMLObject
{
  public:
    TransformGroup();
    TransformGroup(const TransformGroup& copy);
    ~TransformGroup();
    
    void destroy();
    void destroyChildren();
    
    bool loadXMLSettings(XMLElement *element);
    bool exportXMLSettings(ofstream &xmlFile);
    
    void setParent(TransformGroup *group);
    bool addChild(TransformGroup *child);
    bool addChild(Model *child);
    
    unsigned int getGroupsCount();
    TransformGroup *getGroup(int index);
    TransformGroup *getGroup(const char* groupName);
    
    unsigned int getTriangleCount();
    
    unsigned int getModelsCount();
    Model *getModel(int index);
    
    int render(int format, Frustum *frustum = 0);
    void renderAABB(Frustum *frustum = 0);
    
    TransformGroup* pickClosestGroup(Ray3D* ray, Frustum *frustum);
    Model* pickClosestModel(Ray3D* ray, Frustum *frustum);
    
    void updateBoundsDescriptor();
    BoundsDescriptor &updateAndGetBoundsDescriptor();
    BoundsDescriptor &getBoundsDescriptor();
    
    void undoTransform();
    void doTransform();
    
    void setTransform(const Transform3D &t);
    Transform3D &getTransform();
    
    void setVisible(bool visArg);
    void setVisibleRecursive(bool visArg);
    
  private:
    void updateWorldTransform();
    void collectTransformGroups(TransformGroup *group, const Tuple3f& eyePos, Frustum *frustum = 0);
    
  private:
    static Array < DistanceObject<TransformGroup*> > groupsObjectList;
    static Array <RenderObjectOrderedList> opaqueObjectsList;
    static Array <RenderObjectOrderedList> transparentObjectsList;
    //static Array <RenderObjectOrderedList> shadowObjectsList;
    
    BoundsDescriptor boundsDescriptor;
    Transform3D transform;
    Matrix4f worldTransform;
    
    ArrayPtr <TransformGroup> groups;
    ArrayPtr <Model> models;
    
    bool applyWorldTransform;
    bool visible;
    
    TransformGroup *parent;
};

#endif