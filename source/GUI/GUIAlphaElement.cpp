#include "GUIAlphaElement.h"

GUIAlphaElement::GUIAlphaElement(const char *cbs) :  GUIRectangle(cbs)
{
  setColor(225.0f, 225.0f, 225.0f);
  alphaFadeScale =     1.0f;
  minAlpha       =     0.6f;
  color.w        = minAlpha;
}

bool GUIAlphaElement::loadXMLSettings(XMLElement *element)
{
  if (!element)
    return false;
    
  XMLElement *child    = NULL;
  
  if (child = element->getChildByName("alphaFadeScale"))
    setAlphaFadeScale(child->getValuef());
    
  if (child = element->getChildByName("minAlpha"))
    setMinAlpha(child->getValuef());
    
  if (child = element->getChildByName("Text"))
    label.loadXMLSettings(child);
    
  if (child = element->getChildByName("Color"))
    XMLElement::loadRX_GY_BZ_AWf(*child, color);
    
  setColor(color.x, color.y, color.z);
  return   GUIRectangle::loadXMLSettings(element);
}

void  GUIAlphaElement::setColor(const Tuple3f& color)
{
  setColor(color.x, color.y, color.z);
}

void GUIAlphaElement::setColor(float r, float g, float b)
{
  color.set(clamp(r, 0.0f, 255.0f),
            clamp(g, 0.0f, 255.0f),
            clamp(b, 0.0f, 255.0f),
            clamp(color.w, 0.0f, 1.0f));
            
  color.x /= (color.x > 1.0) ? 255.0f : 1.0f;
  color.y /= (color.y > 1.0) ? 255.0f : 1.0f;
  color.z /= (color.z > 1.0) ? 255.0f : 1.0f;
}

const Tuple4f &GUIAlphaElement::getColor()
{
  return color;
}

void  GUIAlphaElement::setLabelString(const char *labelArg)
{
  if (!labelArg)
    return;
    
  label = labelArg;
}

void  GUIAlphaElement::setLabelString(const String &labelArg)
{
  label = labelArg;
}

GUIText * GUIAlphaElement::getLabel()
{
  return &label;
}

const char *GUIAlphaElement::getLabelString()
{
  return label.getString().getBytes();
}

void  GUIAlphaElement::setAlpha(float alphaArg)
{
  color.w = clamp(alphaArg, minAlpha, 1.0f);
}

float GUIAlphaElement::getAlpha()
{
  return color.w;
}

void  GUIAlphaElement::setAlphaFadeScale(float duration)
{
  alphaFadeScale = clamp(duration, 0.0f, 10.0f);
}

float GUIAlphaElement::getAlphaFadeScale()
{
  return alphaFadeScale;
}

void  GUIAlphaElement::setMinAlpha(float minAlphaArg)
{
  minAlpha = clamp(minAlphaArg, 0.2f, 1.0f);
}

float GUIAlphaElement::getMinAlpha()
{
  return minAlpha;
}

void GUIAlphaElement::modifyCurrentAlpha(float clockTick)
{
  if (!mouseOver && !pressed)
    setAlpha(color.w - clockTick * alphaFadeScale);
  else
    setAlpha(color.w + clockTick * alphaFadeScale);
}