#include "GUIMaterialSurface.h"

GUIMaterialSurface::GUIMaterialSurface(const char    *callback) :
    GUIAlphaElement(callback), GUIClippedRectangle()
{
  setBordersColor(0.0f, 0.75f, 0.0f);
  setDimensions(128, 128);
  widgetType = MATERIAL_SURFACE;
}

bool GUIMaterialSurface::loadXMLSettings(XMLElement *element)
{
  if (!element || element->getName() != "MaterialSurface")
    return Logger::writeErrorLog("Need a MaterialSurface node in the xml file");
    
  XMLElement *node = NULL, *child = NULL, *subChild = NULL;
  
  int textureIndex = 0;
  
  for (size_t i = 0; i < element->getChildrenCount(); i++)
  {
    node = element->getChild(i);
    const String &name = node->getName();
    
    if (name == "Appearance")
    {
      appearance.loadXMLSettings(node);
      continue;
    }
  }
  return GUIAlphaElement::loadXMLSettings(element) &&
         GUIClippedRectangle::loadXMLClippedRectangleInfo(element);
}

Appearance *GUIMaterialSurface::getAppearance()
{
  return &appearance;
}

void  GUIMaterialSurface::bindAppearance(bool enable)
{
  Material *materialPointer = appearance.getMaterial();
  Texture  *texturePointer;
  bool activeTexture = false;
  
  if (enable)
  {
    if (materialPointer)
      materialPointer->bindMaterial();
      
    glColor4fv(appearance.getColor());
    
    if (appearance.blendOn())
      appearance.getBlendAttributes().apply();
      
    if (appearance.alphaOn())
      appearance.getAlphaAttributes().apply();
      
    texturePointer = appearance.getTexture(0);
    if (texturePointer->getID())
    {
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      glTexCoordPointer(2, GL_FLOAT, 0, texCoords[0]);
      texturePointer->activate();
    }
  }
  else
  {
    texturePointer = appearance.getTexture(0);
    if (texturePointer->getID())
    {
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);
      texturePointer->deactivate();
    }
    
    if (appearance.blendOn())
      appearance.getBlendAttributes().stop();
      
    if (appearance.alphaOn())
      appearance.getAlphaAttributes().stop();
      
  }
}

void GUIMaterialSurface::render(float clockTick)
{
  if (!parent || !visible)
    return;
    
// if(update)
  {
    computeClippedBounds(getWindowBounds());
  }
  
  bindAppearance(true);
  glVertexPointer(2, GL_INT, 0, vertices[0]);
  glEnableClientState(GL_VERTEX_ARRAY);
  glDrawArrays(GL_TRIANGLE_FAN, 0, 7);
  glDisableClientState(GL_VERTEX_ARRAY);
  bindAppearance(false);
  
  if (drawBounds)
  {
    glColor3fv(bordersColor);
    glVertexPointer(2, GL_INT, 0, vertices[0]);
    glEnableClientState(GL_VERTEX_ARRAY);
    glDrawArrays(GL_LINE_STRIP, 0, 7);
    glDisableClientState(GL_VERTEX_ARRAY);
  }
  glColor3f(1,1,1);
}