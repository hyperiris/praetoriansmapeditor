#ifndef TEXTURENODE
#define TEXTURENODE

#include "Node.h"

class TextureNode : public Node
{
  public:
    TextureNode();
    ~TextureNode();
    
    void accept(Visitor* visitor);
    
    void reset();
    void setID(unsigned int id);
    unsigned int getID();
    
  private:
    unsigned int textureID;
};

#endif