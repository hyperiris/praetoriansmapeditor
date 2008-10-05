#include "Ray3D.h"

Ray3D::Ray3D()
{}

Ray3D::Ray3D(const Tuple3f &Po, const Tuple3f &Pd)
{
  origin = Po;
  setDestination(Pd);
}

void Ray3D::set(const Tuple3f &Po, const Tuple3f &Pd)
{
  setOrigin(Po);
  setDestination(Pd);
}

void Ray3D::setOrigin(const Tuple3f &Po)
{
  origin = Po;
}

void Ray3D::setDestination(const Tuple3f &Pd)
{
  destination = Pd;
}

void Ray3D::normalizeDirection()
{
  destination.normalize();
}

const Tuple3f &Ray3D::getOrigin()
{
  return origin;
}

Tuple3f Ray3D::getDirection()
{
  Tuple3f direction(destination);
  return direction.normalize();
}

const Tuple3f &Ray3D::getDestination()
{
  return destination;
}

float Ray3D::getLength(void)
{
  return (origin -destination).getLength();
}

Ray3D::~Ray3D()
{}