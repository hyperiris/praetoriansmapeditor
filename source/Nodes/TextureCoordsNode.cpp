#include "TextureCoordsNode.h"
#include "../Visitors/Visitor.h"

TextureCoordsNode::TextureCoordsNode()
{
}

void TextureCoordsNode::accept(Visitor* visitor)
{
  visitor->visit(this);
}

void TextureCoordsNode::setCoords(Tuple2f *coords)
{
  txCoords = coords;
}

Tuple2f* TextureCoordsNode::getCoords()
{
  return txCoords;
}

TextureCoordsNode::~TextureCoordsNode()
{
}