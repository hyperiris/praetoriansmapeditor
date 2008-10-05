#ifndef TILEGRAPHNODE
#define TILEGRAPHNODE

#include "Node.h"

class TileController;

class TileGraphNode : public Node
{
  public:
    TileGraphNode();
    ~TileGraphNode();
    
    void accept(Visitor* visitor);
    
    void setController(TileController *contr);
    TileController* getController();
    
  private:
    TileController* controller;
};

#endif