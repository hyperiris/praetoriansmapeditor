#ifndef MATLIGHT_H
#define MATLIGHT_H

#include "../Math/Tuple4.h"

class MatLight
{
  public:
    void setSpecular(const Tuple4f &v);
    void setDiffuse(const Tuple4f &v);
    void setAmbient(const Tuple4f &v);
    
    void setSpecular(const float r, const float g,
                     const float b, const float a);
    void setDiffuse(const float r, const float g,
                    const float b, const float a);
    void setAmbient(const float r, const float g,
                    const float b, const float a);
                    
    const Tuple4f &getSpecular() const
    {
      return specular;
    }
    const Tuple4f &getDiffuse() const
    {
      return diffuse;
    }
    const Tuple4f &getAmbient() const
    {
      return ambient;
    }
    
  protected:
    Tuple4f  specular,
    diffuse,
    ambient;
};
#endif