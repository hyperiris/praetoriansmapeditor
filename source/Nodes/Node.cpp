#include "Node.h"
#include "../Visitors/Visitor.h"

Node::Node()
{
  parentLink.setChildNode(this);
  childLink.setParentNode(this);
}

void Node::accept(Visitor* visitor)
{
  visitor->visit(this);
}

void Node::attach(Node *node)
{
  NodeLink *pLink = &node->childLink;
  parentLink.setParentNode(node);
  parentLink.setPrevious(pLink->getPrevious());
  parentLink.setNext(pLink);
  pLink->getPrevious()->setNext(&parentLink);
  pLink->setPrevious(&parentLink);
}

void Node::detach()
{
  parentLink.getPrevious()->setNext(parentLink.getNext());
  parentLink.getNext()->setPrevious(parentLink.getPrevious());
  parentLink.setPrevious(&parentLink);
  parentLink.setNext(&parentLink);
}

void Node::hide()
{
  if (parentLink.getParentNode())
    if (parentLink.isLinked())
      detach();
}

void Node::reveal()
{
  if (parentLink.getParentNode())
    if (!parentLink.isLinked())
    {
      NodeLink *lastChild = parentLink.getParentNode()->childLink.getPrevious();
      NodeLink *nullChild = &parentLink.getParentNode()->childLink;
      parentLink.setPrevious(lastChild);
      parentLink.setNext(nullChild);
      nullChild->setPrevious(&parentLink);
      lastChild->setNext(&parentLink);
    }
}

Node* Node::getParentNode()
{
  return parentLink.getParentNode();
}

Node* Node::getPreviousSibling()
{
  return parentLink.getPrevious()->getChildNode();
}

Node* Node::getNextSibling()
{
  return parentLink.getNext()->getChildNode();
}

Node* Node::getFirstChild()
{
  return childLink.getNext()->getChildNode();
}

Node* Node::getLastChild()
{
  return childLink.getPrevious()->getChildNode();
}

void Node::reset()
{
  detach();
  parentLink.setParentNode(0);
  parentLink.unLink();
  childLink.unLink();
}

Node::~Node()
{}