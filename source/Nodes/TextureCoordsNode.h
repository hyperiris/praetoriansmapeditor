#ifndef TEXTURECOORDSNODE
#define TEXTURECOORDSNODE

#include "Node.h"
#include "../Math/Tuple2.h"

class TextureCoordsNode : public Node
{
  public:
    TextureCoordsNode();
    ~TextureCoordsNode();
    
    void accept(Visitor* visitor);
    
    void setCoords(Tuple2f *coords);
    Tuple2f* getCoords();
    
  private:
    Tuple2f* txCoords;
};

#endif