#ifndef GLSPRITE_H
#define GLSPRITE_H

#include "../Math/MathUtils.h"
#include "../Appearance/Texture.h"
#include "../Tools/Spline.h"

class Sprite  : public IOXMLObject
{
  private:
    Tuple3f   position;
    Tuple4f   color;
    Texture   texture;
    Spline    path;
    
    float     size,
    timer,
    range;
    
  public:
    Sprite();
    Sprite(const Sprite &copy);
    Sprite &operator =(const Sprite &copy);
    
    void updateTimer(float tick);
    void render();
    void render(float angle);
    
    virtual bool exportXMLSettings(ofstream &xmlFile);
    virtual bool loadXMLSettings(XMLElement *spriteNode);
    
    void setTexture(Texture &tex);
    void setTexture(const char * textureName);
    
    void setColor(float r, float g, float b, float a);
    void setColor(const Tuple4f &clr);
    
    void setSize(float s);
    void setRange(float range);
    
    void setPosition(float x, float y, float z);
    void setPosition(const Tuple3f &pos);
    
    const Tuple3f  &getPosition() const ;
    const Texture  &getTexture()  const ;
    const Tuple4f  &getColor()    const ;
    const Spline   &getSpline()   const ;
    float           getRange();
    float           getSize();
};
#endif