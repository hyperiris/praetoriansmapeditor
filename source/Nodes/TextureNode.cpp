#include "TextureNode.h"
#include "../Visitors/Visitor.h"

TextureNode::TextureNode()
{
  textureID = 65536;
}

void TextureNode::accept(Visitor* visitor)
{
  visitor->visit(this);
}

void TextureNode::reset()
{
  textureID = 65536;
  Node::reset();
}

void TextureNode::setID(unsigned int id)
{
  textureID = id;
}

unsigned int TextureNode::getID()
{
  return textureID;
}

TextureNode::~TextureNode()
{
}