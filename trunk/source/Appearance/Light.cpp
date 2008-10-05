#include "Light.h"
#include "../Tools/Logger.h"

static int  lightCount = 0;

Light::Light() : MatLight()
{
  initialize();
}

Light::Light(const Light &light) : MatLight()
{
  if (this != &light)
    initialize();
    
  this->operator =(light);
}

void Light::initialize()
{
  lightID     =    0;
  enabled     = true;
  
  switch (lightCount)
  {
    case 0:
      lightID = GL_LIGHT0;
      break;
    case 1:
      lightID = GL_LIGHT1;
      break;
    case 2:
      lightID = GL_LIGHT2;
      break;
    case 3:
      lightID = GL_LIGHT3;
      break;
    case 4:
      lightID = GL_LIGHT4;
      break;
    case 5:
      lightID = GL_LIGHT5;
      break;
    case 6:
      lightID = GL_LIGHT6;
      break;
    case 7:
      lightID = GL_LIGHT7;
      break;
    default:
      Logger::writeErrorLog(String("Can't use more than 8 lights"));
  }
  
  if (lightID)
    lightCount++;
}
void Light::bind()
{
  if (!enabled && lightID)
    return;
    
  glEnable(lightID);
  
  glLightf(lightID, GL_QUADRATIC_ATTENUATION, attenuation.z);
  glLightf(lightID, GL_CONSTANT_ATTENUATION , attenuation.x);
  glLightf(lightID, GL_LINEAR_ATTENUATION   , attenuation.y);
  
  glLightfv(lightID, GL_POSITION, position);
  glLightfv(lightID, GL_AMBIENT,  getAmbient());
  glLightfv(lightID, GL_DIFFUSE,  getDiffuse());
  glLightfv(lightID, GL_SPECULAR, getSpecular());
}

void Light::stop()
{
  if (!enabled && lightID)
    return;
  glDisable(lightID);
}

Light &Light::operator = (const Light& light)
{
  if (this != &light)
  {
    setAttenuation(light.attenuation);
    setEnabled(light.enabled);
    setPosition(light.position);
    setSpecular(light.specular);
    setDiffuse(light.diffuse);
    setAmbient(light.ambient);
    setLightID(light.lightID);
  }
  return *this;
}

void Light::setAttenuation(const Tuple4f &att)
{
  attenuation = att;
}

void Light::setAttenuation(const float x, const float y,
                           const float z, const float w)
{
  attenuation.set(x,y,z,w);
}

void Light::setPosition(const Tuple4f  &pos)
{
  position = pos;
}

void Light::setPosition(const float x, const float y,
                        const float z, const float w)
{
  position.set(x,y,z,w);
}

void Light::setLightID(int id)
{
  lightID = id;
}

void Light::setEnabled(bool en)
{
  enabled = en;
}