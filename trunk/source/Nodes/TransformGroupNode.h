#ifndef TRANSFORMGROUPNODE
#define TRANSFORMGROUPNODE

#include "Node.h"

class ModelController;

class TransformGroupNode : public Node
{
  public:
    TransformGroupNode();
    ~TransformGroupNode();
    
  public:
    void accept(Visitor* visitor);
    
  public:
    void setController(ModelController *controller);
    ModelController* getController();
    
  private:
    ModelController* modelController;
};

#endif