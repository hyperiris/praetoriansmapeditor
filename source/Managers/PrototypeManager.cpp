#include "ManagersUtils.h"
#include "../Nodes/TransformGroup.h"

ArrayPtr <PrototypeInfo> PrototypeManager::prototypeCollection;

bool PrototypeManager::addPrototypeInfo(PrototypeInfo *prototypeInfo)
{
  if (!prototypeInfo || !prototypeInfo->getMedia())
    return Logger::writeErrorLog("Cannot add NULL PrototypeInfo");
    
  PrototypeManager::flushUnusedPrototypes();
  prototypeCollection.append(prototypeInfo);
  
  return true;
}

PrototypeInfo *PrototypeManager::getPrototypeInfo(const char* prototypePath)
{
  if (!prototypePath || prototypeCollection.isEmpty())
    return 0;
    
  for (size_t i = 0; i < prototypeCollection.length(); i++)
    if (prototypeCollection(i)->getMediaPath() == prototypePath)
      return prototypeCollection(i);
      
  return 0;
}

PrototypeInfo *PrototypeManager::getPrototypeInfo(const String& prototypePath)
{
  if (!prototypePath.getLength() || prototypeCollection.isEmpty())
    return 0;
    
  for (size_t i = 0; i < prototypeCollection.length(); i++)
    if (prototypeCollection(i)->getMediaPath() == prototypePath)
      return prototypeCollection(i);
      
  return 0;
}

PrototypeInfo *PrototypeManager::getPrototypeInfo(unsigned int index)
{
  return (index >= prototypeCollection.length()) ? 0 : prototypeCollection(index);
}

void PrototypeManager::flushUnusedPrototypes()
{
  ArrayPtr <PrototypeInfo> validPrototypes;
  
  for (size_t i = 0; i < prototypeCollection.length(); i++)
    if (prototypeCollection(i)->getUserCount() > 0)
      validPrototypes.append(prototypeCollection(i));
    else
    {
      deleteObject(prototypeCollection(i)->getMedia());
      deleteObject(prototypeCollection(i));
    }
    
  prototypeCollection = validPrototypes;
}

void PrototypeManager::flushAllPrototypes()
{
  size_t i;
  
  flushUnusedPrototypes();
  
  for (i = 0; i < prototypeCollection.length(); i++)
  {
    deleteObject(prototypeCollection(i)->getMedia());
    deleteObject(prototypeCollection(i));
  }
  
  prototypeCollection.clear();
  
  if (i) Logger::writeInfoLog(String("Flushed ") + int(i) + String(" active prototypes"));
}

void PrototypeManager::printPrototypeInfo()
{
  if (prototypeCollection.isEmpty())
    cout << "This Manager contains no prototypes." << endl;
  else
  {
    cout << "Prototype list: " << endl;
    for (size_t i = 0; i < prototypeCollection.length(); i++)
      cout <<  "<users = \"" << prototypeCollection(i)->getUserCount() << "\" "
           <<  " path = \"" << prototypeCollection(i)->getMediaPath() << "\" />" << endl;
  }
}
