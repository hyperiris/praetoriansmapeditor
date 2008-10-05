#ifndef INDICESNODE
#define INDICESNODE

#include "Node.h"

class IndicesNode : public Node
{
  public:
    void accept(Visitor* visitor);
    
    void setIndices(unsigned int *idx);
    unsigned int* getIndices();
    
  private:
    unsigned int *indices;
};

#endif