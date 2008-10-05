#include "GUITexCoordDescriptor.h"
#include "../Tools/Logger.h"

Tuple2i textureGUIDimensions(256,256);

void GUITexCoordDescriptor::setTextureWidth(int width)
{
  textureGUIDimensions.x = clamp(width, 8, 1024);
}

void GUITexCoordDescriptor::setTextureHeight(int height)
{
  textureGUIDimensions.y = clamp(height, 8, 1024);
}

GUITexCoordDescriptor::GUITexCoordDescriptor(int widgetTypeArg)
{
  texCoords.set(0.0f,0.0f,1.0f,1.0f);
  widgetType = widgetTypeArg;
}

GUITexCoordDescriptor::GUITexCoordDescriptor(const GUITexCoordDescriptor &copy)
{
  this->operator=(copy);
}

GUITexCoordDescriptor &GUITexCoordDescriptor::operator =(const GUITexCoordDescriptor &copy)
{

  if (this != &copy)
  {
    texCoords  = copy.texCoords;
    widgetType = copy.widgetType;
  }
  return *this;
}

void GUITexCoordDescriptor::setType(int type)
{
  switch (type)
  {
    case SLIDER:
    case BUTTON:
    case CHECK_BOX:
    case CHECK_BOX_MARK:
    case RADIO_BUTTON:
    case CHECK_RB_MARK:
      widgetType = type;
      break;
    default:
      widgetType = UNKNOWN;
  }
}

void GUITexCoordDescriptor::setType(const String &type)
{
  if (!type.getLength())
  {
    Logger::writeErrorLog("NULL GUITexCoordDescriptor type");
    return;
  }
  
  if (type == "MATERIAL_SURFACE")
  {
    widgetType = MATERIAL_SURFACE;
    return;
  }
  if (type == "CHECK_BOX_MARK")
  {
    widgetType = CHECK_BOX_MARK;
    return;
  }
  if (type == "CHECK_RB_MARK")
  {
    widgetType = CHECK_RB_MARK;
    return;
  }
  if (type == "RADIO_BUTTON")
  {
    widgetType = RADIO_BUTTON;
    return;
  }
  if (type == "SEPARATOR")
  {
    widgetType = SEPARATOR;
    return;
  }
  if (type == "CHECK_BOX")
  {
    widgetType = CHECK_BOX;
    return;
  }
  if (type == "TEXT_AREA")
  {
    widgetType = TEXT_AREA;
    return;
  }
  if (type == "BUTTON")
  {
    widgetType = BUTTON;
    return;
  }
  if (type == "SLIDER")
  {
    widgetType = SLIDER;
    return;
  }
  if (type == "PANEL")
  {
    widgetType = PANEL;
    return;
  }
  if (type == "LABEL")
  {
    widgetType = LABEL;
    return;
  }
  
  Logger::writeErrorLog(String("Unknow GUITexCoordDescriptor type -> ") + type);
  widgetType = UNKNOWN;
}

int GUITexCoordDescriptor::getType()
{
  return widgetType;
}

void GUITexCoordDescriptor::loadXMLSettings(XMLElement *element)
{
  if (!element || element->getName() != "TexCoordsDesc")
    return;
    
  XMLElement *child = NULL;
  
  string name;
  int    xStart        = 0,
                         yStart        = 0,
                                         xEnd          = 0,
                                                         yEnd          = 0;
                                                         
  if (child = element->getChildByName("xStart"))
    xStart = child->getValuei();
    
  if (child = element->getChildByName("yStart"))
    yStart = child->getValuei();
    
  if (child = element->getChildByName("xEnd"))
    xEnd = child->getValuei();
    
  if (child = element->getChildByName("yEnd"))
    yEnd = child->getValuei();
    
  if (child = element->getChildByName("type"))
    if (widgetType = child->getValuei());
    else
      setType(child->getValue());
      
      
  setTexCoords(xStart/float(textureGUIDimensions.x), yStart/float(textureGUIDimensions.y),
               xEnd  /float(textureGUIDimensions.x), yEnd  /float(textureGUIDimensions.y));
  setType(widgetType);
}

int GUITexCoordDescriptor::getTextureWidth()
{
  return textureGUIDimensions.x;
}

int GUITexCoordDescriptor::getTextureHeight()
{
  return textureGUIDimensions.y;
}

void GUITexCoordDescriptor::setTexCoords(float x, float y, float z, float w)
{
  texCoords.set(clamp(x,0.0f, 1.0f), clamp(y,0.0f, 1.0f),
                clamp(z,0.0f, 1.0f), clamp(w,0.0f, 1.0f));
}

void GUITexCoordDescriptor::setTexCoords(const Tuple4f& texCoordsArg)
{
  texCoords =  texCoordsArg;
};

const Tuple4f &GUITexCoordDescriptor::getTexCoords()
{
  return texCoords;
}
