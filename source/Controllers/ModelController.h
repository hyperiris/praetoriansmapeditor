#ifndef MODELCONTROLLER
#define MODELCONTROLLER

#include "../Nodes/TransformGroupNode.h"
#include "../Math/Tuple3.h"

class TransformGroup;

class ModelController
{
  public:
    ModelController();
    ModelController(TransformGroup* group);
    virtual ~ModelController();
    
  public:
    TransformGroup* getTransformGroup();
    TransformGroupNode* getTGNode();
    
    void translateModel(float x, float y, float z);
    
    Tuple3f getPosition();
    
  private:
    TransformGroup* controlledGroup;
    TransformGroupNode tgNode;
};

#endif