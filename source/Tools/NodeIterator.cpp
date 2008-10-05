#include "NodeIterator.h"
#include "../Nodes/Node.h"

NodeIterator::NodeIterator() : pNode(0)
{
}

NodeIterator::NodeIterator(Node* node)
{
  pNode = node;
}

NodeIterator& NodeIterator::operator ++ ()
{
  pNode = pNode->getNextSibling();
  return *this;
}

NodeIterator NodeIterator::operator ++ (int)
{
  NodeIterator temp = *this;
  pNode = pNode->getNextSibling();
  return temp;
}

NodeIterator& NodeIterator::operator -- ()
{
  pNode = pNode->getPreviousSibling();
  return *this;
}

NodeIterator NodeIterator::operator -- (int)
{
  NodeIterator temp = *this;
  pNode = pNode->getPreviousSibling();
  return temp;
}

bool NodeIterator::operator == (NodeIterator& right)
{
  return (pNode == right.pNode);
}

bool NodeIterator::operator != (NodeIterator& right)
{
  return !this->operator == (right);
}

bool NodeIterator::end()
{
  return (pNode == 0);
}

Node* NodeIterator::current()
{
  return pNode;
}
