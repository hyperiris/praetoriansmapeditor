#ifndef NODEITERATOR
#define NODEITERATOR

class Node;

class NodeIterator
{
  public:
    NodeIterator();
    NodeIterator(Node* node);
    
  public:
    NodeIterator& operator ++ ();
    NodeIterator operator ++ (int);
    
    NodeIterator& operator -- ();
    NodeIterator operator -- (int);
    
    bool operator == (NodeIterator& right);
    bool operator != (NodeIterator& right);
    bool end();
    
    Node* current();
    
  protected:
    Node *pNode;
};

#endif
