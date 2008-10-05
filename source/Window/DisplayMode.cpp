#include "DisplayMode.h"
#include "../Math/MathUtils.h"

DisplayMode::DisplayMode(int  x,
                         int  y,
                         int  width,
                         int  height,
                         int  colorDepth,
                         bool fullscreen,
                         bool centered) : IOXMLObject("DisplayMode")
{
  set(x, y, width, height, colorDepth, fullscreen, centered);
  refreshRate = 0;
}

DisplayMode::DisplayMode(const DisplayMode &copy) : IOXMLObject("DisplayMode")
{
  operator =(copy);
}

DisplayMode &DisplayMode::operator=(const DisplayMode &copy)
{
  setRefreshRate(copy.refreshRate);
  set(copy.x, copy.y, copy.width, copy.height,
      copy.colorDepth, copy.fullscreen, copy.centered);
  return *this;
}

bool DisplayMode::loadXMLSettings(XMLElement *element)
{
  if (!isSuitable(element))
    return false;
    
  XMLElement  *child     = NULL;
  
  for (size_t i = 0; i < element->getChildrenCount(); i++)
  {
    if (!(child = element->getChild(i)))
      continue;
      
    const String &childName = child->getName();
    
    if (childName == "fullscreen")
    {
      fullscreen = (child->getValue() == "true");
      continue;
    }
    
    if (childName == "centered")
    {
      centered = (child->getValue() == "true");
      continue;
    }
    
    if (childName == "height")
    {
      height  = child->getValuei();
      continue;
    }
    
    if (childName == "width")
    {
      width  = child->getValuei();
      continue;
    }
    
    if (childName == "colorDepth")
    {
      colorDepth = child->getValuei();
      continue;
    }
    
    if (childName == "x")
    {
      x = child->getValuei();
      continue;
    }
    
    if (childName == "y")
    {
      y = child->getValuei();
      continue;
    }
  }
  
  set(x, y, width, height, colorDepth, fullscreen, centered);
  return true;
}

bool DisplayMode::exportXMLSettings(ofstream &xmlFile)
{
  if (!xmlFile.is_open())
    return Logger::writeErrorLog("Cannot export DisplayMode to XML: file not ready.");
    
  xmlFile << "<DisplayMode  fullscreen = " << (fullscreen ? "true" : "false") << "\"\n"
  << "              colorDepth = " <<  colorDepth                     << "\"\n"
  << "              centered   = " << (centered   ? "true" : "false") << "\"\n"
  << "              height     = " <<  height                         << "\"\n"
  << "              width      = " <<  width                          << "\"\n"
  << "              x          = " <<  x                              << "\"\n"
  << "              y          = " <<  y                              << "\" /> \n";
  
  return true;
}

String DisplayMode::toString()
{
  return String("fullscreen: ")   + (fullscreen ? "true" : "false") +
         String(", colorDepth: ") + colorDepth +
         String(", centered: ")   + (centered ? "true" : "false") +
         String(", height: ")     + height +
         String(", width: ")      + width +
         String(", x: ")          + x +
         String(", y: ")          + y;
}

void DisplayMode::set(int x_,  int y_, int w, int h, int c, bool fs, bool cen)
{
  fullscreen = fs;
  colorDepth = clamp(c, 8, 32);
  centered   = fs ? false : cen;
  height     = clamp(h, 1, 2048);
  width      = clamp(w, 1, 2048);
  y          = fs ? 0 : y_;
  x          = fs ? 0 : x_;
}

bool DisplayMode::operator==(const DisplayMode &compare)
{
  return (fullscreen == compare.fullscreen) &&
         (colorDepth == compare.colorDepth) &&
         (centered   == compare.centered)   &&
         (height     == compare.height)     &&
         (width      == compare.width)      &&
         (y          == compare.y)          &&
         (x          == compare.x);
}

bool DisplayMode::operator!=(const DisplayMode &compare)
{
  return !this->operator ==(compare);
}

void DisplayMode::setColorDepth(int colorDepthArg)
{
  colorDepth  = colorDepthArg;
}
int  DisplayMode::getColorDepth()
{
  return colorDepth;
}

void DisplayMode::setHeight(int heightArg)
{
  height  = clamp(heightArg, 1, 2048);
}
int  DisplayMode::getHeight()
{
  return height;
}

void DisplayMode::setWidth(int widthArg)
{
  width  = clamp(widthArg, 1, 2048);
}
int  DisplayMode::getWidth()
{
  return width;
}

void DisplayMode::setPosition(int x_, int y_)
{
  x  = fullscreen ? 0 : x_;
  y  = fullscreen ? 0 : y_;
}

void DisplayMode::setRefreshRate(int refresh)
{
  refreshRate = clamp(refresh, 0, 160);
}

int  DisplayMode::getRefreshRate()
{
  return refreshRate;
}

void DisplayMode::setXPosition(int x_)
{
  x  = fullscreen ? 0 : x_;
}
void DisplayMode::setYPosition(int y_)
{
  y  = fullscreen ? 0 : y_;
}

int  DisplayMode::getXPosition()
{
  return x;
}
int  DisplayMode::getYPosition()
{
  return y;
}

void DisplayMode::setFullscreen(bool fs)
{
  fullscreen = fs;
  setCentered(centered);
}
bool DisplayMode::isFullscreen()
{
  return fullscreen;
}

void DisplayMode::setCentered(bool cen)
{
  centered   = fullscreen ? false : cen;
}
bool DisplayMode::isCentered()
{
  return centered;
}