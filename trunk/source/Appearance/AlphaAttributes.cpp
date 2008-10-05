#include "AlphaAttributes.h"
#include "../Managers/MediaPathManager.h"
#include "../Math/MathUtils.h"

AlphaAttributes::AlphaAttributes(int functionArg, float referenceArg) : IOXMLObject("AlphaAttributes")
{
  reference = referenceArg;
  function  = functionArg;
}

AlphaAttributes::AlphaAttributes(const AlphaAttributes &copy) : IOXMLObject("AlphaAttributes")
{
  reference = copy.reference;
  function  = copy.function;
}

AlphaAttributes &AlphaAttributes::operator = (const AlphaAttributes &copy)
{
  reference = copy.reference;
  function  = copy.function;
  return *this;
}

bool  AlphaAttributes::loadXMLSettings(XMLElement *element)
{
  if (!isSuitable(element))
    return false;
    
  XMLElement  *subChild  = NULL,
                           *child     = NULL;
  String       path;
  
  if (child = element->getChildByName("description"))
    return IOXMLObject::loadXMLFile(child->getValuec());
    
  for (size_t i = 0; i < element->getChildrenCount(); i++)
  {
    if (!(child = element->getChild(i)))
      continue;
      
    const String &childName = child->getName();
    
    if (childName == "function")
      setFunction(getXMLAlphaFunc(child->getValue()));
      
    if (childName == "reference")
      setReference(child->getValuef());
  }
  
  return true;
}

bool  AlphaAttributes::exportXMLSettings(ofstream &xmlFile)
{
  if (!xmlFile.is_open())
    return Logger::writeErrorLog("Cannot export AlphaAttributes to XML: file not ready.");
    
  if (function == -1.0f || reference == -1.0f)
    return Logger::writeErrorLog("Cannot export AlphaAttributes to XML: invalid attributes {-1.0}.");
    
  xmlFile << "  <AlphaAttributes  reference = \"" << reference << "\n"
  "                    function  = \"" <<((function ==  GL_NEVER) ? "NEVER\"/> \n"    :
                                          (function ==  GL_LESS) ? "LESS\"/> \n"     :
                                          (function ==  GL_EQUAL) ? "EQUAL\"/> \n"    :
                                          (function ==  GL_NOTEQUAL) ? "NOTEQUAL\" />\n" :
                                          (function ==  GL_GREATER) ? "GREATER\" />\n"  :
                                          (function ==  GL_LEQUAL) ? "LEQUAL\" />\n"   :
                                          (function ==  GL_GEQUAL) ? "GEQUAL\" />\n"   :  "ALWAYS\" />\n");
  return true;
}

void AlphaAttributes::setAttributes(int function, float reference)
{
  setReference(reference);
  setFunction(function);
}

void  AlphaAttributes::setFunction(int functionArg)
{
  switch (functionArg)
  {
    case GL_NEVER:
    case GL_LESS:
    case GL_EQUAL:
    case GL_LEQUAL:
    case GL_GREATER:
    case GL_NOTEQUAL:
    case GL_GEQUAL:
    case GL_ALWAYS:
      function = functionArg;
      return;
  }
  
  Logger::writeErrorLog("Invalid Alpha function, AlphaAttributes::setFunction(int function)");
  function = -1;
}

int   AlphaAttributes::getFunction()
{
  return function;
}

void  AlphaAttributes::setReference(float referenceArg)
{
  reference = clamp(referenceArg, 0.0f, 1.0f);
  if (reference != referenceArg)
    Logger::writeErrorLog("Alpha reference clamped to fit in [0.0, 1.0]");
}

float AlphaAttributes::getReference()
{
  return reference;
}

void  AlphaAttributes::apply()
{
  if (reference != -1.0 && function != -1)
  {
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(function, reference);
  }
}

void  AlphaAttributes::stop()
{
  if (reference != -1.0 && function != -1)
    glDisable(GL_ALPHA_TEST);
}

int   AlphaAttributes::getXMLAlphaFunc(const String &value)
{
  if (value.getLength())
  {
    if (value == "NOTEQUAL") return GL_NOTEQUAL;
    if (value == "GREATER")  return GL_GREATER;
    if (value == "GEQUAL")   return GL_GEQUAL;
    if (value == "LEQUAL")   return GL_LEQUAL;
    if (value == "ALWAYS")   return GL_ALWAYS;
    if (value == "NEVER")    return GL_NEVER;
    if (value == "EQUAL")    return GL_EQUAL;
    if (value == "LESS")     return GL_LESS;
  }
  return -1;
}

bool AlphaAttributes::isSet()
{
  return (function != -1 || reference != -1.0f);
}