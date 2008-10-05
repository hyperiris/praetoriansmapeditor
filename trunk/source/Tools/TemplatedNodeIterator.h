#ifndef TEMPLATEDNODEITERATOR
#define TEMPLATEDNODEITERATOR

#include "../Nodes/Node.h"

template <class T>
class TemplatedNodeIterator
{
  public:
    TemplatedNodeIterator() : pNode(0)
    {}
    
    TemplatedNodeIterator(T* node)
    {
      pNode = node;
    }
    
    TemplatedNodeIterator& operator ++ ()
    {
      pNode = pNode->getNextSibling();
      return *this;
    }
    
    TemplatedNodeIterator operator ++ (int)
    {
      TemplatedNodeIterator temp = *this;
      pNode = (T*)pNode->getNextSibling();
      return temp;
    }
    
    void setNode(T* node)
    {
      pNode = node;
    }
    
    bool end()
    {
      return (pNode == 0);
    }
    
    T* current()
    {
      return pNode;
    }
    
  protected:
    T* pNode;
};

#endif
