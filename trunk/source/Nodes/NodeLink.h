#ifndef NODELINK
#define NODELINK

class Node;

class NodeLink
{
  public:
    NodeLink();
    ~NodeLink();
    
  public:
    void setParentNode(Node *node);
    void setChildNode(Node *node);
    Node* getParentNode();
    Node* getChildNode();
    
    void setPrevious(NodeLink *link);
    void setNext(NodeLink *link);
    NodeLink* getPrevious();
    NodeLink* getNext();
    
    void unLink();
    
    bool isLinked();
    
  private:
    Node *parentNode;
    Node *childNode;
    
    NodeLink *previousLink;
    NodeLink *nextLink;
};

#endif
