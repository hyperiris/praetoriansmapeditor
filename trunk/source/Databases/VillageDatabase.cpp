#include "VillageDatabase.h"
#include "../Nodes/TransformGroup.h"
#include "../Managers/ManagersUtils.h"
#include "../Parsers/XMLParser.h"
#include "../Controllers/VillageModelController.h"

VillageDatabase::VillageDatabase(const char* dbName) : ModelDatabase(dbName)
{
  xmlStack = new XMLStack();
}

bool VillageDatabase::initialize(const char* path)
{
  XMLElement *child, *root, *subchild, *childAttr;
  
  if (!xmlStack->loadXMLFile(MediaPathManager::lookUpMediaPath(path)))
    return false;
    
  if (!(root = xmlStack->getChildByName(name)))
    return Logger::writeErrorLog(String("Need a <") + name + "> tag in the XML file");
    
  for (size_t i = 0; i < root->getChildrenCount(); i++)
  {
    child = root->getChild(i);
    if (child->getName() == "Village")
    {
      if (!(subchild = child->getChild(0)))
        continue;
        
      childAttr = subchild->getChildByName("name");
      if (subchild->getName() == "Group" && childAttr)
        if (!villageElementsAVLTree.contains(childAttr->getValuec()))
        {
          villageElementsAVLTree.insertKeyAndValue(childAttr->getValuec(), child);
          registeredModelNames.append(childAttr->getValuec());
        }
    }
  }
  
  return !villageElementsAVLTree.isEmpty();
}

TransformGroup* VillageDatabase::buildPrototype(const char* name)
{
  XMLElement *root;
  XMLElement *villageGroup;
  TransformGroup* villageTG = 0;
  
  root = xmlStack->getChild(0);
  
  AVLTreeIterator <XMLElement*> iter(villageElementsAVLTree);
  while (iter.next())
  {
    villageGroup = iter.value()->getChild(0);
    if (villageGroup->getChildByName("name")->getValue() == name)
    {
      villageTG = new TransformGroup();
      if (!villageTG->loadXMLSettings(villageGroup))
      {
        deleteObject(villageTG);
        return 0;
      }
    }
  }
  
  return villageTG;
}

ModelController* VillageDatabase::createController(TransformGroup* group)
{
  return new VillageModelController(group);
}

VillageDatabase::~VillageDatabase()
{
  deleteObject(xmlStack);
}