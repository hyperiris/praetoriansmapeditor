#include "BlendAttributes.h"
#include "../Managers/MediaPathManager.h"

BlendAttributes::BlendAttributes(int sourceArg, int destinationArg) : IOXMLObject("BlendAttributes")
{
  destination = destinationArg;
  source      = sourceArg;
}

BlendAttributes::BlendAttributes(const BlendAttributes &copy) : IOXMLObject("BlendAttributes")
{
  destination = copy.destination;
  source      = copy.source;
}

BlendAttributes &BlendAttributes::operator = (const BlendAttributes &copy)
{
  destination = copy.destination;
  source      = copy.source;
  return *this;
}

bool  BlendAttributes::loadXMLSettings(XMLElement *element)
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
    
    if (childName == "destination")
      setDestination(getXMLDestinationFactor(child->getValue()));
      
    if (childName == "source")
      setSource(getXMLSourceFactor(child->getValue()));
  }
  
  return true;
}

bool  BlendAttributes::exportXMLSettings(ofstream &xmlFile)
{
  if (!xmlFile.is_open())
    return Logger::writeErrorLog("Cannot export BlendAttributes to XML: file not ready.");
    
  if (destination == -1.0f || source == -1.0f)
    return Logger::writeErrorLog("Cannot export BlendAttributes to XML: invalid attributes {-1.0}.");
    
  xmlFile << "  <BlendAttributes  destination = \"" <<((destination ==  GL_ONE_MINUS_SRC_COLOR) ? "ONE_MINUS_SRC_COLOR\"\n" :
      (destination ==  GL_ONE_MINUS_SRC_ALPHA) ? "ONE_MINUS_SRC_ALPHA\"\n" :
      (destination ==  GL_ONE_MINUS_DST_ALPHA) ? "ONE_MINUS_DST_ALPHA\"\n" :
      (destination ==  GL_SRC_COLOR)           ? "SRC_COLOR\"\n"           :
      (destination ==  GL_SRC_ALPHA)           ? "SRC_ALPHA\"\n"           :
      (destination ==  GL_DST_ALPHA)           ? "DST_ALPHA\"\n"           :
      (destination ==  GL_ZERO)                ? "ZERO\"\n"                :  "ONE\"\n")<<
      
  "                    source      = \"" <<((source ==  GL_ONE_MINUS_DST_COLOR) ? "ONE_MINUS_DST_COLOR\"/> \n" :
      (source ==  GL_ONE_MINUS_DST_ALPHA) ? "ONE_MINUS_DST_ALPHA\"/> \n" :
      (source ==  GL_ONE_MINUS_SRC_ALPHA) ? "ONE_MINUS_SRC_ALPHA\"/> \n" :
      (source ==  GL_SRC_ALPHA_SATURATE)  ? "SRC_ALPHA_SATURATE\" />\n"  :
      (source ==  GL_DST_ALPHA)           ? "DST_ALPHA\" />\n"           :
      (source ==  GL_SRC_ALPHA)           ? "SRC_ALPHA\" />\n"           :
      (source ==  GL_DST_COLOR)           ? "DST_COLOR\" />\n"           :
      (source ==  GL_ZERO)                ? "ZERO\" />\n"                :   "ONE\" />\n");
  return true;
}

void BlendAttributes::setAttributes(int source, int destination)
{
  setDestination(destination);
  setSource(source);
}

void  BlendAttributes::setSource(int sourceArg)
{
  switch (sourceArg)
  {
    case GL_ZERO:
    case GL_ONE:
    case GL_DST_COLOR:
    case GL_ONE_MINUS_DST_COLOR:
    case GL_SRC_ALPHA:
    case GL_ONE_MINUS_SRC_ALPHA:
    case GL_DST_ALPHA:
    case GL_ONE_MINUS_DST_ALPHA:
    case GL_SRC_ALPHA_SATURATE:
      source = sourceArg;
      return;
  }
  
  Logger::writeErrorLog("Invalid Blend Source, BlendAttributes::setSource(int source)");
  source = -1;
}

int   BlendAttributes::getSource()
{
  return source;
}

void  BlendAttributes::setDestination(int destinationArg)
{
  switch (destinationArg)
  {
    case GL_ZERO:
    case GL_ONE:
    case GL_SRC_COLOR:
    case GL_ONE_MINUS_SRC_COLOR:
    case GL_SRC_ALPHA:
    case GL_ONE_MINUS_SRC_ALPHA:
    case GL_DST_ALPHA:
    case GL_ONE_MINUS_DST_ALPHA:
      destination = destinationArg;
      return;
  }
  
  Logger::writeErrorLog("Invalid Blend Destination, BlendAttributes::setDestination(int destination)");
  destination = -1;
}

int BlendAttributes::getDestination()
{
  return destination;
}

void  BlendAttributes::apply()
{
  if (source != -1.0 && destination != -1)
  {
    glEnable(GL_BLEND);
    glBlendFunc(source, destination);
  }
}

void  BlendAttributes::stop()
{
  if (source != -1.0 && destination != -1)
    glDisable(GL_BLEND);
}

int   BlendAttributes::getXMLDestinationFactor(const String &value)
{
  if (value.getLength())
  {
    if (value == "ONE_MINUS_SRC_COLOR") return GL_ONE_MINUS_SRC_COLOR;
    if (value == "ONE_MINUS_SRC_ALPHA") return GL_ONE_MINUS_SRC_ALPHA;
    if (value == "ONE_MINUS_DST_ALPHA") return GL_ONE_MINUS_DST_ALPHA;
    if (value == "SRC_COLOR")           return GL_SRC_COLOR;
    if (value == "SRC_ALPHA")           return GL_SRC_ALPHA;
    if (value == "DST_ALPHA")           return GL_DST_ALPHA;
    if (value == "ZERO")                return GL_ZERO;
    if (value == "ONE")                 return GL_ONE;
  }
  return -1;
}

int   BlendAttributes::getXMLSourceFactor(const String &value)
{
  if (value.getLength())
  {
    if (value == "ONE_MINUS_DST_COLOR") return GL_ONE_MINUS_DST_COLOR;
    if (value == "ONE_MINUS_DST_ALPHA") return GL_ONE_MINUS_DST_ALPHA;
    if (value == "ONE_MINUS_SRC_ALPHA") return GL_ONE_MINUS_SRC_ALPHA;
    if (value == "SRC_ALPHA_SATURATE")  return GL_SRC_ALPHA_SATURATE;
    if (value == "DST_ALPHA")           return GL_DST_ALPHA;
    if (value == "SRC_ALPHA")           return GL_SRC_ALPHA;
    if (value == "DST_COLOR")           return GL_DST_COLOR;
    if (value == "ZERO")                return GL_ZERO;
    if (value == "ONE")                 return GL_ONE;
  }
  return -1;
}

bool BlendAttributes::isSet()
{
  return (source != -1 || destination != -1);
}