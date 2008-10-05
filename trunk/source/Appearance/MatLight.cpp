#include "MatLight.h"
#include "../Tools/Logger.h"

void MatLight::setDiffuse(const Tuple4f &t)
{
  diffuse = t;
}

void MatLight::setDiffuse(const float r, const float g,
                          const float b, const float a)
{
  diffuse.set(r, g, b, a);
}

void MatLight::setSpecular(const Tuple4f &t)
{
  specular = t;
}

void MatLight::setSpecular(const float r, const float g,
                           const float b, const float a)
{
  specular.set(r, g, b, a);
}

void MatLight::setAmbient(const Tuple4f &t)
{
  ambient = t;
}

void MatLight::setAmbient(const float r, const float g,
                          const float b, const float a)
{
  ambient.set(r, g, b, a);
}