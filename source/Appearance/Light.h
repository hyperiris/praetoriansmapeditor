#ifndef GLLIGHT_H
#define GLLIGHT_H
#include "MatLight.h"

class Light : public MatLight
{
  public:
    Light();
    Light(const Light &);
    Light &operator =(const Light &);
    
    const Tuple4f &getAttenuation() const
    {
      return attenuation;
    }
    const Tuple4f &getPosition()    const
    {
      return position;
    }
    const Tuple4f &getColor()       const
    {
      return color;
    }
    const int      getLightID()     const
    {
      return lightID;
    }
    
    void     initialize();
    void     bind();
    void     stop();
    
    void     setAttenuation(const Tuple4f  &v);
    void     setAttenuation(const float x, const float y,
                            const float z, const float w);
                            
    void     setEnabled(bool);
    
    void     setPosition(const Tuple4f &v);
    void     setPosition(const float x, const float y,
                         const float z, const float w);
                         
    void     setLightID(int);
    bool     isEnabled()
    {
      return enabled;
    }
  private:
    Tuple4f  attenuation,
    position,
    color;
    bool     enabled;
    int      lightID;
    
};
#endif
