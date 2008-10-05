#include "IOXMLObject.h"
#include "../Managers/MediaPathManager.h"
using namespace std;

IOXMLObject::IOXMLObject(const char *ionameArg) : NamedObject(ionameArg)
{
}

bool IOXMLObject::loadXMLFile(const char* path)
{
  if (!name.getLength())
    return Logger::writeErrorLog("Cannot load XML settings -> NULL element name");
    
  const char *verified = MediaPathManager::lookUpMediaPath(path);
  
  if (!verified)
    return Logger::writeErrorLog(String("Couldn't locate the <") +
                                 name + "> XML file at \""       +
                                 path + "\" even with a look up");
                                 
  XMLElement  *child  = NULL;
  XMLStack     xmlStack;
  
  if (!xmlStack.loadXMLFile(verified))
    return Logger::writeErrorLog(String("Invalid XML file -> ") + verified);
    
  if (!(child = xmlStack.getChildByName(name)))
    return Logger::writeErrorLog(String("Need a <") + name + "> tag in the XML file");
    
  return loadXMLSettings(child);
}

bool IOXMLObject::exportXMLSettings(const char* xmlPath)
{
  ofstream xmlFile(xmlPath, ios::app | ios::binary);
  bool result = exportXMLSettings(xmlFile);
  xmlFile.close();
  return result;
}

bool IOXMLObject::isSuitable(XMLElement *element)
{
  if (!element)
    return  Logger::writeErrorLog(String("NULL <") + name +  "> node");
    
  if (element->getName() != name)
    return Logger::writeErrorLog(String("Need a <") + name + "> tag in the XML Node");
    
  return true;
}


