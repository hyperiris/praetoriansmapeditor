#include "Command.h"
#include "../Parsers/XMLParser.h"

bool Command::loadXMLSettings(XMLElement *element)
{
  XMLElement *subChild = 0;
  
  if (subChild = element->getChildByName("source"))
    setSource(subChild->getValuec());
    
  if (subChild = element->getChildByName("target"))
    setTarget(subChild->getValuec());
    
  return true;
}

void Command::setSource(const char* s)
{
  source = s;
}

void Command::setTarget(const char* t)
{
  target = t;
}

const char* Command::getSource()
{
  return source;
}

const char* Command::getTarget()
{
  return target;
}

void Command::execute()
{
}
