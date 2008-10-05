#ifndef NODE
#define NODE

#include "NodeLink.h"

class Visitor;

class Node
{
  public:
    Node();
    virtual ~Node();
    
  public:
    virtual void accept(Visitor* visitor);
    virtual void reset();
    
    void detach();
    void attach(Node *node);
    
    void hide();
    void reveal();
    
    Node* getParentNode();
    Node* getPreviousSibling();
    Node* getNextSibling();
    Node* getFirstChild();
    Node* getLastChild();
    
  protected:
    NodeLink parentLink;
    NodeLink childLink;
};

#endif
