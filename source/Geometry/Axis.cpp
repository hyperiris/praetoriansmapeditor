#include "Axis.h"

#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>

Axis::Axis(void)
{
  yOffset.set(0.0f, 0.01f, 0.0f);
}

void Axis::draw()
{
  glLineWidth(2);
  glBegin(GL_LINES);
  
  glColor3f(1, 0, 0);
  glVertex3f(yOffset.x, yOffset.y, yOffset.z);
  glVertex3f(yOffset.x, yOffset.y + 1.0f, yOffset.z);
  
  glColor3f(0, 1, 0);
  glVertex3f(yOffset.x, yOffset.y, yOffset.z);
  glVertex3f(yOffset.x, yOffset.y, yOffset.z + 1.0f);
  
  glColor3f(0, 0, 1);
  glVertex3f(yOffset.x, yOffset.y, yOffset.z);
  glVertex3f(yOffset.x + 1.0f, yOffset.y, yOffset.z);
  
  glEnd();
  glLineWidth(1);
}

void Axis::setOffset(const Tuple3f &offset)
{
  yOffset = offset;
}

Axis::~Axis(void)
{
}
