#include "ModelDatabase.h"
#include "../Managers/ManagersUtils.h"
#include "../Nodes/TransformGroup.h"
#include "../Factories/FactoryPBA.h"
#include "../Factories/WorldVisuals.h"
#include "../Kernel/Gateway.h"
#include "../Nodes/TransformGroupNode.h"
#include "../Controllers/ModelController.h"

ModelDatabase::ModelDatabase(const char* dbName) : NamedObject(dbName)
{
  rootGroup = new TransformGroup();
  dbtype = WorldObjectTypes::UNUSED;
}

bool ModelDatabase::initialize(const char* path)
{
  XMLStack xmlStack;
  XMLElement *root, *child, *childParam;
  String childName;
  const char *p;
  
  if (!(p = MediaPathManager::lookUpMediaPath(path)))
    return Logger::writeErrorLog(String("Could not locate ") + name + " definition file");
    
  if (!xmlStack.loadXMLFile(p))
    return Logger::writeErrorLog(String("Invalid XML file -> ") + path);
    
  if (!(root = xmlStack.getChildByName(name)))
    return Logger::writeErrorLog(String("Need a <") + name + "> tag in the XML file");
    
  for (unsigned int i = 0; i < root->getChildrenCount(); i++)
  {
    child = root->getChild(i);
    childName = child->getName();
    
    if (childName == "Objeto")
      if (childParam = child->getChildByName("name"))
        registeredModelNames.append(childParam->getValuec());
  }
  
  return true;
}

void ModelDatabase::compile()
{
  TransformGroup *proto, *group;
  WorldVisuals* worldVisuals;
  WorldObject* obj;
  PrototypeInfo* info;
  
  worldVisuals = Gateway::getWorldVisuals();
  
  for (unsigned int i = 0; i < worldVisuals->getObjectCount(); i++)
  {
    obj = worldVisuals->getObject(i);
    
    ///prototype will be returned only if the object's name matches a name
    ///that had been registered for this particular database.
    if (!(proto = getPrototype(obj->name)))
      continue;
      
    info = PrototypeManager::getPrototypeInfo(proto->getName());
    info->increaseUserCount();
    
    group = new TransformGroup(*proto);
    group->getTransform().setTranslations(obj->position.x, obj->position.y, obj->position.z);
    //group->getTransform().rotateY(90.0f * DEG2RAD);
    rootGroup->addChild(group);
  }
  
  PrototypeManager::flushAllPrototypes();
  rootGroup->updateBoundsDescriptor();
}

TransformGroup* ModelDatabase::getPrototype(const char* protoName)
{
  TransformGroup *proto = 0;
  PrototypeInfo* info;
  
  if (info = PrototypeManager::getPrototypeInfo(protoName))
    proto = info->getMedia();
  else
  {
    if (!(proto = buildPrototype(protoName)))
      return 0;
      
    info = new PrototypeInfo(protoName, proto);
    PrototypeManager::addPrototypeInfo(info);
    info->decreaseUserCount();
  }
  
  return proto;
}

TransformGroup* ModelDatabase::buildPrototype(const char* protoName)
{
  TransformGroup *proto = 0;
  
  if (registeredModelNames.contains(protoName))
  {
    proto = new TransformGroup();
    if (!loadPBA(proto, protoName))
    {
      deleteObject(proto);
      return 0;
    }
    proto->setName(protoName);
    proto->updateBoundsDescriptor();
  }
  
  return proto;
}

ModelController* ModelDatabase::instantiateModel(const char* modelName)
{
  TransformGroup* instance;
  TransformGroup* proto;;
  ModelController* controller;
  
  if (!(proto = getPrototype(modelName)))
    return 0;
    
  instance = new TransformGroup(*proto);
  controller = createController(instance);//new ModelController(instance);
  modelControllers.append(controller);
  
  rootGroup->addChild(instance);
  
  return controller;
}

ModelController* ModelDatabase::createController(TransformGroup* group)
{
  return new ModelController(group);
}

TransformGroup* ModelDatabase::getRootGroup()
{
  return rootGroup;
}

const Array <ICString>* ModelDatabase::getNamesList()
{
  return &registeredModelNames;
}

ModelController* ModelDatabase::getModelController(unsigned int index)
{
  return modelControllers(index);
}

void ModelDatabase::renderPrototypeImage(const char* name, const Transform3D& transform, const Tuple2i& size, int format)
{
  if (!strlen(name))
    return;
    
  Tuple3f center;
  Tuple4i viewport;
  Tuple3f minend, maxend;
  float xscale, yscale, maxlen;
  TransformGroup *prototypeParent, *prototypeChild, *prototype;
  
  if (!(prototype = getPrototype(name)))
  {
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    return;
  }
  
  prototypeParent = new TransformGroup();
  prototypeChild = new TransformGroup(*prototype);
  
  prototypeChild->setTransform(transform);
  prototypeParent->addChild(prototypeChild);
  prototypeParent->updateBoundsDescriptor();
  
  minend = prototypeChild->getBoundsDescriptor().getMinEndAABB();
  maxend = prototypeChild->getBoundsDescriptor().getMaxEndAABB();
  xscale = (maxend.x - minend.x);
  yscale = (maxend.y - minend.y);
  maxlen = xscale > yscale ? xscale : yscale;
  
  glClearColor(1,1,1,1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(-maxlen/2, maxlen/2, minend.y, minend.y + maxlen, 1000, -1000);
  
  glGetIntegerv(GL_VIEWPORT, viewport);
  glViewport(0, 0, size.x, size.y);
  
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  
  prototypeParent->render(format);
  
  deleteObject(prototypeParent);
  
  glClearColor(0,0,0,1);
  glViewport(0, 0, viewport.z, viewport.w);
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
}

unsigned int ModelDatabase::getModelControllerCount()
{
  return modelControllers.length();
}

void ModelDatabase::setType(int typeVal)
{
  dbtype = typeVal;
}

int ModelDatabase::getType(const char* objName)
{
  if (registeredModelNames.contains(objName))
    return dbtype;
    
  return -1;
}

void ModelDatabase::reset()
{
  rootGroup->destroy();
  modelControllers.clearAndDestroy();
}

ModelDatabase::~ModelDatabase()
{
  deleteObject(rootGroup);
  modelControllers.clearAndDestroy();
}