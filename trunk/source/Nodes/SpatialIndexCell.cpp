#include "SpatialIndexCell.h"
#include "../Visitors/Visitor.h"

SpatialIndexCell::SpatialIndexCell()
{}

void SpatialIndexCell::accept(Visitor* visitor)
{
  visitor->visit(this);
}

SpatialIndexCell::~SpatialIndexCell()
{}