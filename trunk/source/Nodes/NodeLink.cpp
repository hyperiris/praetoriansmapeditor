#include "NodeLink.h"

NodeLink::NodeLink()
{
  parentNode = 0;
  childNode = 0;
  previousLink = this;
  nextLink = this;
}

void NodeLink::setParentNode(Node *node)
{
  parentNode = node;
}

void NodeLink::setChildNode(Node *node)
{
  childNode = node;
}

Node *NodeLink::getParentNode()
{
  return parentNode;
}

Node *NodeLink::getChildNode()
{
  return childNode;
}

void NodeLink::setPrevious(NodeLink *link)
{
  previousLink = link;
}

void NodeLink::setNext(NodeLink *link)
{
  nextLink = link;
}

NodeLink *NodeLink::getPrevious()
{
  return previousLink;
}

NodeLink *NodeLink::getNext()
{
  return nextLink;
}

bool NodeLink::isLinked()
{
  return (previousLink != this && nextLink != this);
}

void NodeLink::unLink()
{
  previousLink = this;
  nextLink = this;
}

NodeLink::~NodeLink()
{}