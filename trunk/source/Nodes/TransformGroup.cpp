#include "TransformGroup.h"
#include "../Factories/FactoryPBA.h"
#include "../Geometry/Model.h"
#include "../Geometry/Geometry.h"
#include "../Renderer/Renderer.h"
#include "../Containers/ocsort.h"
#include "../Math/Radix.h"
#include "../Geometry/Ray3D.h"
#include "../Math/Intersector.h"
#include "../Kernel/Gateway.h"

Array < DistanceObject<TransformGroup*> > TransformGroup::groupsObjectList;
Array <RenderObjectOrderedList> TransformGroup::opaqueObjectsList;
Array <RenderObjectOrderedList> TransformGroup::transparentObjectsList;

TransformGroup::TransformGroup() : IOXMLObject("Group")
{
  applyWorldTransform = false;
  parent = 0;
  visible = true;
}

TransformGroup::TransformGroup(const TransformGroup& copy) : IOXMLObject("Group")
{
  if (this != &copy)
  {
    destroy();
    
    Model* newmodel;
    TransformGroup* newgroup;
    size_t i;
    
    applyWorldTransform = copy.applyWorldTransform;
    parent = 0;
    visible = copy.visible;
    setName(copy.getName());
    
    for (i = 0; i < copy.models.length(); i++)
    {
      newmodel = new Model(*copy.models(i));
      addChild(newmodel);
    }
    
    for (i = 0; i < copy.groups.length(); i++)
    {
      newgroup = new TransformGroup(*copy.groups(i));
      addChild(newgroup);
    }
    
    transform = copy.transform;
    updateBoundsDescriptor();
  }
}

int TransformGroup::render(int format, Frustum *frustum)
{
  int tc;
  int txid;
  int order;
  int apptype;
  float coeff;
  size_t i;
  size_t j;
  size_t index;
  Model* mdl;
  TransformGroup *currentGroup;
  RenderObjectOrderedList* pRO;
  Tuple3f groupCenter;
  Tuple3f eyePosition;
  const unsigned int* sortedIdx;
  
  order = (format & FRONT_TO_BACK) ? FRONT_TO_BACK : (format & BACK_TO_FRONT) ? BACK_TO_FRONT : 0;
  tc = 0;
  sortedIdx = 0;
  
  if (!visible)
    return 0;
    
  if (order)
  {
    eyePosition = Renderer::getCameraPosition();
    coeff = (order == FRONT_TO_BACK) ? 1.0f : -1.0f;
    
    groupsObjectList.clearFast();
    collectTransformGroups(this, eyePosition, frustum);
    
    for (i = 0; i < groupsObjectList.length(); i++)
    {
      currentGroup = groupsObjectList(i).getObject();
      currentGroup->updateBoundsDescriptor();
      groupCenter = currentGroup->boundsDescriptor.getCenterOBB();
      groupsObjectList(i).setDistance(coeff * groupCenter.getDistance(eyePosition));
    }
    
    //OCInsertionSort(groupsObjectList, 0, groupsObjectList.length());
    OCQuickSort(groupsObjectList, 0, groupsObjectList.length());
    //RadixSort radixSorter;
    //sortedIdx = radixSorter.Sort((float*)groupsObjectList.data(), groupsObjectList.length()).GetIndices();
    //RadixSort3 radixSorter;
    //sortedIdx = radixSorter.Sort((float*)groupsObjectList.data(), groupsObjectList.length()).GetRanks();
    
    if (format & NOT_SORTED_BY_TEXTURE)
    {
      for (i = 0; i < groupsObjectList.length(); i++)
      {
        if (sortedIdx)
          currentGroup = groupsObjectList(sortedIdx[i]).getObject();
        else
          currentGroup = groupsObjectList(i).getObject();
        if (!currentGroup->visible)
          continue;
          
        currentGroup->doTransform();
        for (index = 0; index < currentGroup->models.length(); index++)
          tc += currentGroup->models(index)->draw(format);
        currentGroup->undoTransform();
      }
    }
    else if (format & SORTED_BY_TEXTURE)
    {
      opaqueObjectsList.clear();
      transparentObjectsList.clear();
      
      for (i = 0; i < groupsObjectList.length(); i++)
      {
        if (sortedIdx)
          currentGroup = groupsObjectList(sortedIdx[i]).getObject();
        else
          currentGroup = groupsObjectList(i).getObject();
        if (!currentGroup->visible)
          continue;
          
        for (index = 0; index < currentGroup->models.length(); index++)
        {
          mdl = currentGroup->models(index);
          txid = mdl->getAppearance()->getTexture(0)->getID();
          apptype = mdl->getAppearance()->getType();
          
          ///collect non-blended
          if (apptype == MATERIAL_NONE || apptype == MATERIAL_ALPHATEST)
          {
            for (j = 0; j < opaqueObjectsList.length(); j++)
            {
              if (opaqueObjectsList(j).textureID == txid)
              {
                opaqueObjectsList(j).groups.append(currentGroup);
                opaqueObjectsList(j).models.append(mdl);
                break;
              }
              else
              {
                RenderObjectOrderedList rool;
                rool.textureID = txid;
                rool.groups.append(currentGroup);
                rool.models.append(mdl);
                opaqueObjectsList.append(rool);
                break;
              }
            }
            
            if (opaqueObjectsList.isEmpty())
            {
              RenderObjectOrderedList rool;
              rool.textureID = txid;
              rool.groups.append(currentGroup);
              rool.models.append(mdl);
              opaqueObjectsList.append(rool);
            }
          }
          ///collect blended
          else if (apptype == MATERIAL_ALPHA)
          {
            if (transparentObjectsList.isEmpty())
            {
              RenderObjectOrderedList rool;
              rool.groups.append(currentGroup);
              rool.models.append(mdl);
              transparentObjectsList.append(rool);
            }
            else
            {
              if (order == BACK_TO_FRONT)
              {
                transparentObjectsList(0).groups.append(currentGroup);
                transparentObjectsList(0).models.append(mdl);
              }
              else
              {
                transparentObjectsList(0).groups.prepend(currentGroup);
                transparentObjectsList(0).models.prepend(mdl);
              }
            }
          }
          ///collect blended
          /*//
          else if (apptype == MATERIAL_ALPHASOMBRA)
          {
            if (shadowObjectsList.isEmpty())
            {
              RenderObjectOrderedList rool;
              rool.groups.append(currentGroup);
              rool.models.append(mdl);
              shadowObjectsList.append(rool);
            }
            else
            {
              if (order == BACK_TO_FRONT)
              {
                shadowObjectsList(0).groups.prepend(currentGroup);
                shadowObjectsList(0).models.prepend(mdl);
              }
              else
              {
                shadowObjectsList(0).groups.append(currentGroup);
                shadowObjectsList(0).models.append(mdl);
              }
            }
          }
          //*/
        }
      }
      
      ///render opaque
      for (i = 0; i < opaqueObjectsList.length(); i++)
      {
        pRO = &opaqueObjectsList(i);
        for (j = 0; j < pRO->groups.length(); j++)
        {
          currentGroup = pRO->groups(j);
          currentGroup->doTransform();
          tc += pRO->models(j)->draw(format);
          currentGroup->undoTransform();
        }
      }
      
      ///render transparent
      for (i = 0; i < transparentObjectsList.length(); i++)
      {
        pRO = &transparentObjectsList(i);
        for (j = 0; j < pRO->groups.length(); j++)
        {
          currentGroup = pRO->groups(j);
          currentGroup->doTransform();
          tc += pRO->models(j)->draw(format);
          currentGroup->undoTransform();
        }
      }
      
      /*/render shadows
      for (i = 0; i < shadowObjectsList.length(); i++)
      {
        pRO = &shadowObjectsList(i);
        for (j = 0; j < pRO->groups.length(); j++)
        {
          currentGroup = pRO->groups(j);
          currentGroup->doTransform();
          tc += pRO->models(j)->draw(format);
          currentGroup->undoTransform();
        }
      }
      //*/
    }
  }
  else
  {
    updateBoundsDescriptor();
    
    if (frustum)
      if (!frustum->AABBInFrustum(boundsDescriptor))
        return 0;
        
    doTransform();
    for (i = 0; i < models.length(); i++)
      tc += models(i)->draw(format);
    undoTransform();
    
    for (i = 0; i < groups.length(); i++)
      tc += groups(i)->render(format, frustum);
  }
  
  return tc;
}

void TransformGroup::renderAABB(Frustum *frustum)
{
  if (updateAndGetBoundsDescriptor().isInitialized())
  {
    if (frustum)
      if (!frustum->AABBInFrustum(boundsDescriptor))
        return;
        
    boundsDescriptor.render(BoundsDescriptor::WIRE | BoundsDescriptor::AABB);
  }
  
  for (size_t g = 0; g < groups.length(); g++)
    groups(g)->renderAABB(frustum);
}

unsigned int TransformGroup::getTriangleCount()
{
  unsigned int tc = 0;
  unsigned int i;
  
  for (i = 0; i < models.length(); i++)
    tc += models(i)->getGeometry()->getTriangleCount();
    
  for (i = 0; i < groups.length(); i++)
    tc += groups(i)->getTriangleCount();
    
  return tc;
}

bool TransformGroup::loadXMLSettings(XMLElement *element)
{
  if (!isSuitable(element))
    return false;
    
  XMLElement *child, *attr;
  String childName;
  
  destroy();
  
  for (size_t i = 0; i < element->getChildrenCount(); i++)
  {
    child = element->getChild(i);
    childName = child->getName();
    
    if (childName == "visible")
    {
      setVisible(child->getValue() == "true");
      continue;
    }
    
    if (childName == "name")
    {
      name = child->getValue();
      continue;
    }
    
    if (childName == "Transform")
    {
      transform.loadXMLSettings(child);
      continue;
    }
    
    if (childName == "Shape")
    {
      if (attr = child->getChildByName("model"))
        loadPBA(this, attr->getValue());
      continue;
    }
    
    if (childName == "Group")
    {
      TransformGroup *group = new TransformGroup();
      if (!group->loadXMLSettings(child))
      {
        deleteObject(group);
      }
      else
        addChild(group);
      continue;
    }
  }
  
  return true;
}

bool TransformGroup::exportXMLSettings(ofstream &xmlFile)
{
  return true;
}

void TransformGroup::setParent(TransformGroup *group)
{
  parent = group;
}

bool TransformGroup::addChild(TransformGroup *child)
{
  groups.append(child);
  child->setParent(this);
  transform.overrideUpdatedFlag(true);
  updateWorldTransform();
  return true;
}

bool TransformGroup::addChild(Model *child)
{
  models.append(child);
  child->setParent(this);
  return true;
}

unsigned int TransformGroup::getGroupsCount()
{
  return groups.length();
}

TransformGroup *TransformGroup::getGroup(int index)
{
  return groups(index);
}

TransformGroup *TransformGroup::getGroup(const char* groupName)
{
  TransformGroup *target = 0;
  
  if (!groupName)
    return 0;
    
  if (name == groupName)
    return this;
    
  for (size_t i = 0; i < groups.length(); i++)
    if (target = groups(i)->getGroup(groupName))
      break;
      
  return target;
}

unsigned int TransformGroup::getModelsCount()
{
  return models.length();
}

Model *TransformGroup::getModel(int index)
{
  return models(index);
}

BoundsDescriptor &TransformGroup::updateAndGetBoundsDescriptor()
{
  updateBoundsDescriptor();
  return getBoundsDescriptor();
}

BoundsDescriptor &TransformGroup::getBoundsDescriptor()
{
  return boundsDescriptor;
}

void TransformGroup::doTransform()
{
  if (applyWorldTransform)
  {
    glPushMatrix();
    glMultMatrixf(worldTransform);
  }
}

void TransformGroup::undoTransform()
{
  if (applyWorldTransform)
    glPopMatrix();
}

void TransformGroup::setTransform(const Transform3D &t)
{
  transform = t;
  transform.overrideUpdatedFlag(true);
  updateWorldTransform();
}

Transform3D &TransformGroup::getTransform()
{
  return transform;
}

void TransformGroup::updateWorldTransform()
{
  if (transform.updatedFlagOn())
  {
    if (parent)
    {
      worldTransform  = parent->worldTransform;
      worldTransform *= transform.getMatrix4f();
    }
    else
      worldTransform = transform.getMatrix4f();
      
    for (size_t g = 0; g < groups.length(); g++)
    {
      groups(g)->transform.overrideUpdatedFlag(true);
      groups(g)->updateWorldTransform();
    }
    
    transform.overrideUpdatedFlag(false);
    applyWorldTransform = (worldTransform != IDENTITY);
  }
}

void TransformGroup::updateBoundsDescriptor()
{
  size_t i;
  
  updateWorldTransform();
  boundsDescriptor.reset();
  
  for (i = 0; i < models.length(); i++)
  {
    if (!boundsDescriptor.isInitialized())
      boundsDescriptor  = models(i)->getGeometry()->getBoundsDescriptor();
    else
      boundsDescriptor += models(i)->getGeometry()->getBoundsDescriptor();
  }
  
  if (boundsDescriptor.isInitialized())
    boundsDescriptor *= worldTransform;
    
  for (i = 0; i < groups.length(); i++)
    if (!boundsDescriptor.isInitialized())
      boundsDescriptor  = groups(i)->updateAndGetBoundsDescriptor();
    else
      boundsDescriptor += groups(i)->updateAndGetBoundsDescriptor();
}

void TransformGroup::collectTransformGroups(TransformGroup *parent, const Tuple3f& eyePos, Frustum *frustum)
{
  size_t g;
  const ArrayPtr <TransformGroup> &children = parent->groups;
  
  if (parent && !frustum)
  {
    for (g = 0; g < children.length(); g++)
    {
      if (!children(g)->visible)
        continue;
      groupsObjectList.append(DistanceObject<TransformGroup*>(children(g)));
      collectTransformGroups(children(g), eyePos);
    }
    return;
  }
  
  float camRadius = Gateway::getConfiguration().camRadius;
  
  if (parent && frustum)
    for (g = 0; g < children.length(); g++)
      if (frustum->AABBInFrustum(children(g)->getBoundsDescriptor()) && children(g)->visible)
      {
        if (children(g)->getBoundsDescriptor().getCenterOBB().getDistance(eyePos) < camRadius)
          groupsObjectList.append(DistanceObject<TransformGroup*>(children(g)));
        collectTransformGroups(children(g), eyePos, frustum);
      }
}

void TransformGroup::setVisible(bool visArg)
{
  visible = visArg;
}

void TransformGroup::setVisibleRecursive(bool visArg)
{
  visible = visArg;
  
  for (size_t i = 0; i < groups.length(); i++)
    groups(i)->setVisibleRecursive(visArg);
}


TransformGroup* TransformGroup::pickClosestGroup(Ray3D* ray, Frustum *frustum)
{
  TransformGroup *currentGroup;
  Intersector intersector;
  
  Tuple3f ro;
  Tuple3f rd;
  Tuple3f d;
  Tuple3f a;
  Tuple3f e;
  Tuple3f c;
  Tuple4f packet;
  
  Matrix4f matrix;
  
  groupsObjectList.clearFast();
  collectTransformGroups(this, Renderer::getCameraPosition(), frustum);
  
  ArrayPtr < DistanceObject<TransformGroup*> > pickedGroupsList(groupsObjectList.length());
  
  ro = ray->getOrigin();
  rd = ray->getDestination();
  
  for (size_t i = 0; i < groupsObjectList.length(); i++)
  {
    currentGroup = groupsObjectList(i).getObject();
    
    d = (rd - ro) * 0.5f;
    a.set(fabsf(d.x), fabsf(d.y), fabsf(d.z));
    
    e = currentGroup->boundsDescriptor.getExtents();
    c = ro + d - currentGroup->boundsDescriptor.getCenterOBB();
    
    if (fabsf(c.x) > e.x + a.x || fabsf(c.y) > e.y + a.y || fabsf(c.z) > e.z + a.z)
      continue;
      
    if (fabsf(d.y * c.z - d.z * c.y) > (e.y * a.z + e.z * a.y) ||
        fabsf(d.z * c.x - d.x * c.z) > (e.z * a.x + e.x * a.z) ||
        fabsf(d.x * c.y - d.y * c.x) > (e.x * a.y + e.y * a.x))
      continue;
      
      
    for (size_t j = 0; j < currentGroup->models.length(); j++)
    {
      if (currentGroup->models(j)->getAppearance()->getType() == MATERIAL_ALPHASOMBRA)
        continue;
        
      matrix = currentGroup->worldTransform;
      matrix.setInverse();
      Ray3D newray(ro*matrix, rd*matrix);
      packet = intersector.intersectTriangles(&newray,
                                              currentGroup->models(j)->getGeometry()->getVertices(),
                                              currentGroup->models(j)->getGeometry()->getIndices(),
                                              currentGroup->models(j)->getGeometry()->getTriangleCount());
                                              
      if (packet.w == 1)
      {
        groupsObjectList(i).setDistance(ro.getDistance(Tuple3f(packet.x, packet.y, packet.z)*matrix));
        pickedGroupsList.append(&groupsObjectList(i));
        //break;
      }
    }
  }
  
  if (pickedGroupsList.isEmpty())
    return 0;
    
  OCQuickSort(pickedGroupsList, 0, pickedGroupsList.length());
  
  return pickedGroupsList(0)->getObject();
}

Model* TransformGroup::pickClosestModel(Ray3D* ray, Frustum *frustum)
{
  return 0;
}

void TransformGroup::destroyChildren()
{
  models.clearAndDestroy();
  groups.clearAndDestroy();
  boundsDescriptor.reset();
  transform.setIdentity();
  worldTransform.setIdentity();
}

void TransformGroup::destroy()
{
  models.clearAndDestroy();
  groups.clearAndDestroy();
  visible = true;
  parent = 0;
}

TransformGroup::~TransformGroup()
{
  destroy();
}