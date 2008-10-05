#include "Cylinder.h"
#include "../Math/MathUtils.h"
#include <windows.h>
#include <gl/glu.h>
#include <gl/gl.h>

Cylinder::Cylinder()
{
  radius = 5.0f;
  height = 10.0f;
  sides = 20;
  topVertsColor.set(1);
  bottomVertsColor.set(1);
}

void Cylinder::set(float fRadius, float fHeight, unsigned int nSides)
{
  radius = fRadius;
  height = fHeight;
  sides = nSides;
}

void Cylinder::setTopVerticesColor(float r, float g, float b, float a)
{
  topVertsColor.set(r,g,b,a);
}

void Cylinder::setBottomVerticesColor(float r, float g, float b, float a)
{
  bottomVertsColor.set(r,g,b,a);
}

void Cylinder::setRadius(float r)
{
  radius = r;
}

void Cylinder::setHeight(float h)
{
  height = h;
}

void Cylinder::setNumSides(unsigned int n)
{
  sides = n;
}

void Cylinder::draw()
{
  float inc = TWO_PI / sides;
  float theta = 0.0f;
  Tuple2f p;
  
  glBegin(GL_TRIANGLE_STRIP);
  for (unsigned int i = 0; i <= sides; i++)
  {
    p.x = radius * fastCos(theta);
    p.y = radius * fastSin(theta);
    glColor4fv(bottomVertsColor);
    glVertex3f(p.x, 0, p.y);
    glColor4fv(topVertsColor);
    glVertex3f(p.x, height, p.y);
    theta += inc;
  }
  glEnd();
}

Cylinder::~Cylinder()
{
}