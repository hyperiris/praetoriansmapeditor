#include "WindowEvent.h"

WindowEvent::WindowEvent(int xArg, int yArg, int widthArg, int heightArg)
{
  height = heightArg;
  width  = widthArg;
  x      = xArg;
  y      = yArg;
}

WindowEvent::WindowEvent(const WindowEvent& copy)
{
  this->operator =(copy);
}

WindowEvent &WindowEvent::operator= (const WindowEvent& copy)
{
  if (this != &copy)
  {
    height = copy.height;
    width  = copy.width;
    x      = copy.x;
    y      = copy.y;
  }
  return *this;
}

int WindowEvent::getHeight()
{
  return height;
}
int WindowEvent::getWidth()
{
  return width;
}
int WindowEvent::getX()
{
  return x;
}
int WindowEvent::getY()
{
  return y;
}