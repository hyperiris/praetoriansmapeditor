#include "Sprite.h"

Sprite::Sprite()  : IOXMLObject("Sprite")
{
  size  = 1.0f;
  timer = 0.0f;
  range = 10.0f;
}

Sprite::Sprite(const Sprite &copy) : IOXMLObject("Sprite")
{
  this->operator =(copy);
}

Sprite &Sprite::operator =(const Sprite &copy)
{
  if (this != &copy)
  {
    position = copy.position;
    texture  = copy.texture;
    range    = copy.range;
    color    = copy.color;
    timer    = copy.timer;
    size     = copy.size;
  }
  return *this;
}


void Sprite::updateTimer(float tick)
{
  timer  += tick;
  timer  -= (timer >= path.getCycleLength()) ? path.getCycleLength() : 0.0f;
  
  Tuple3f newPosition = path.getLocationAtTime(timer);
  if (newPosition.getLengthSquared())
    position.set(newPosition);
}


void Sprite::render()
{
  static  Matrix4f matrixBuffer;
  
  glDepthMask(GL_FALSE);
  glColor4fv(color);
  
  texture.activate();
  
  glPushMatrix();
  
  //if(!GL_ARB_point_sprite_supported)
  {
    glGetFloatv(GL_MODELVIEW_MATRIX, matrixBuffer);
    //Improve locality
    matrixBuffer.setTranspose();
    
    glBegin(GL_TRIANGLE_STRIP);
    
    glTexCoord2f(0.0f,0.0f);
    glVertex3f(position.x + (-matrixBuffer[0] - matrixBuffer[4])*size,
               position.y + (-matrixBuffer[1] - matrixBuffer[5])*size,
               position.z + (-matrixBuffer[2] - matrixBuffer[6])*size);
               
    glTexCoord2f(1.0f,0.0f);
    glVertex3f(position.x + (matrixBuffer[0] - matrixBuffer[4])*size,
               position.y + (matrixBuffer[1] - matrixBuffer[5])*size,
               position.z + (matrixBuffer[2] - matrixBuffer[6])*size);
               
    glTexCoord2f(0.0f,1.0f);
    glVertex3f(position.x + (-matrixBuffer[0] + matrixBuffer[4])*size,
               position.y + (-matrixBuffer[1] + matrixBuffer[5])*size,
               position.z + (-matrixBuffer[2] + matrixBuffer[6])*size);
               
    glTexCoord2f(1.0f,1.0f);
    glVertex3f(position.x + (matrixBuffer[0] + matrixBuffer[4])*size,
               position.y + (matrixBuffer[1] + matrixBuffer[5])*size,
               position.z + (matrixBuffer[2] + matrixBuffer[6])*size);
    glEnd();
  }
  /*else{
    glPointSize(size*2);
    glEnable(GL_POINT_SPRITE_ARB);
    glTexEnvf(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE);
   glBegin(GL_POINTS);
  glVertex3fv(position);
  glEnd();
    glDisable(GL_POINT_SPRITE_ARB);
  }*/
  glPopMatrix();
  texture.deactivate();
  glColor3f(1,1,1);
  glDepthMask(GL_TRUE);
}

void Sprite::render(float angle)
{
  static  Matrix4f matrixBuffer;
  float   newSize = 0.8f*size;
  
  glDepthMask(GL_FALSE);
  glColor4fv(color*0.7f);
  texture.activate();
  
  //if(!GL_ARB_point_sprite_supported)
  {
    glGetFloatv(GL_MODELVIEW_MATRIX, matrixBuffer);
    //Improve locality
    matrixBuffer.setTranspose();
    
    glMatrixMode(GL_TEXTURE);
    glTranslatef(0.5,0.5,0.0);
    glRotatef(angle,0,0,1);
    glTranslatef(-0.5,-0.5,0.0);
    
    glBegin(GL_TRIANGLE_STRIP);
    
    glTexCoord2f(0.0f,0.0f);
    glVertex3f(position.x + (-matrixBuffer[0] - matrixBuffer[4])*size,
               position.y + (-matrixBuffer[1] - matrixBuffer[5])*size,
               position.z + (-matrixBuffer[2] - matrixBuffer[6])*size);
               
    glTexCoord2f(1.0f,0.0f);
    glVertex3f(position.x + (matrixBuffer[0] - matrixBuffer[4])*size,
               position.y + (matrixBuffer[1] - matrixBuffer[5])*size,
               position.z + (matrixBuffer[2] - matrixBuffer[6])*size);
               
    glTexCoord2f(0.0f,1.0f);
    glVertex3f(position.x + (-matrixBuffer[0] + matrixBuffer[4])*size,
               position.y + (-matrixBuffer[1] + matrixBuffer[5])*size,
               position.z + (-matrixBuffer[2] + matrixBuffer[6])*size);
               
    glTexCoord2f(1.0f,1.0f);
    glVertex3f(position.x + (matrixBuffer[0] + matrixBuffer[4])*size,
               position.y + (matrixBuffer[1] + matrixBuffer[5])*size,
               position.z + (matrixBuffer[2] + matrixBuffer[6])*size);
               
    glEnd();
    glLoadIdentity();
    
    glColor4fv(color*0.5f + Tuple3f(0.1f,0.1f,0.1f));
    
    glTranslatef(0.5,0.5,0.0);
    glRotatef(-angle,0,0,1);
    glTranslatef(-0.5,-0.5,0.0);
    
    glBegin(GL_TRIANGLE_STRIP);
    
    glTexCoord2f(0.0f,0.0f);
    glVertex3f(position.x + (-matrixBuffer[0] - matrixBuffer[4])*newSize,
               position.y + (-matrixBuffer[1] - matrixBuffer[5])*newSize,
               position.z + (-matrixBuffer[2] - matrixBuffer[6])*newSize);
               
    glTexCoord2f(1.0f,0.0f);
    glVertex3f(position.x + (matrixBuffer[0] - matrixBuffer[4])*newSize,
               position.y + (matrixBuffer[1] - matrixBuffer[5])*newSize,
               position.z + (matrixBuffer[2] - matrixBuffer[6])*newSize);
               
    glTexCoord2f(0.0f,1.0f);
    glVertex3f(position.x + (-matrixBuffer[0] + matrixBuffer[4])*newSize,
               position.y + (-matrixBuffer[1] + matrixBuffer[5])*newSize,
               position.z + (-matrixBuffer[2] + matrixBuffer[6])*newSize);
               
    glTexCoord2f(1.0f,1.0f);
    glVertex3f(position.x + (matrixBuffer[0] + matrixBuffer[4])*newSize,
               position.y + (matrixBuffer[1] + matrixBuffer[5])*newSize,
               position.z + (matrixBuffer[2] + matrixBuffer[6])*newSize);
               
    glEnd();
    glLoadIdentity();
    
    glMatrixMode(GL_MODELVIEW);
    
  }
  /*else{
    glPointSize(size*2);
    glEnable(GL_POINT_SPRITE_ARB);
    glTexEnvf(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE);
   glBegin(GL_POINTS);
  glVertex3fv(position);
  glEnd();
    glDisable(GL_POINT_SPRITE_ARB);
  }*/
  texture.deactivate();
  glColor3f(1,1,1);
  glDepthMask(GL_TRUE);
}

bool Sprite::loadXMLSettings(XMLElement *sprite)
{
  XMLElement *child1 = NULL,
                       *child2 = NULL;
  Tuple4f    temporary;
  
  if (!isSuitable(sprite))
    return false;
    
  if (child1 = sprite->getChildByName("description"))
    return IOXMLObject::loadXMLFile(child1->getValuec());
    
  for (size_t i = 0; i < sprite->getChildrenCount(); i++)
  {
    if (!(child1 = sprite->getChild(i)))
      continue;
      
    const String &token =  child1->getName();
    
    if (token == "Spline")
      path.loadXMLOrbit(child1);
      
    if (token == "size")
      size = child1->getValuef();
      
    if (token == "range")
      range = child1->getValuef();
      
    if (token == "Position" || token == "Center")
      XMLElement::loadRX_GY_BZf(*child1, position);
      
      
    if (token == "Texture")
      texture.loadXMLSettings(child1);
      
    if (token == "Color")
      XMLElement::loadRX_GY_BZ_AWf(*child1, color);
      
  }
  
  return false;
}

bool     Sprite::exportXMLSettings(ofstream &xmlFile)
{
  return true;
}

void     Sprite::setRange(float rangeArg)
{
  range = rangeArg;
}
float    Sprite::getRange()
{
  return range;
}

void     Sprite::setTexture(const char * textureName)
{
  texture.load2D(textureName);
}
void     Sprite::setTexture(Texture &tex)
{
  texture = tex;
}
const    Texture &Sprite::getTexture() const
{
  return texture;
}

void     Sprite::setColor(float r, float g, float b, float a)
{
  color.set(r,g,b,a);
}
void     Sprite::setColor(const Tuple4f &clr)
{
  color = clr;
}
const    Tuple4f &Sprite::getColor() const
{
  return color;
}

void     Sprite::setSize(float s)
{
  size = (s <= 0.0) ? size : s;
}
float    Sprite::getSize()
{
  return size;
}

void           Sprite::setPosition(float x, float y, float z)
{
  position.set(x,y,z);
}
void           Sprite::setPosition(const Tuple3f &pos)
{
  position = pos;
}
const Tuple3f &Sprite::getPosition() const
{
  return position;
}

const Spline  &Sprite::getSpline() const
{
  return path;
}
