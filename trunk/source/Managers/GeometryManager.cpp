#include "ManagersUtils.h"
#include "../Geometry/Geometry.h"

ArrayPtr <GeometryInfo> GeometryManager::geometryCollection;

bool GeometryManager::addGeometryInfo(GeometryInfo *geometryInfo)
{
  if (!geometryInfo || !geometryInfo->getMedia())
    return Logger::writeErrorLog("Cannot add NULL GeometryInfo");
    
  GeometryManager::flushUnusedGeometry();
  geometryCollection.append(geometryInfo);
  
  return true;
}

GeometryInfo *GeometryManager::getGeometryInfo(const char* geometryPath)
{
  if (!geometryPath || geometryCollection.isEmpty())
    return 0;
    
  for (size_t i = 0; i < geometryCollection.length(); i++)
    if (geometryCollection(i)->getMediaPath() == geometryPath)
      return geometryCollection(i);
      
  return 0;
}

GeometryInfo *GeometryManager::getGeometryInfo(const String& geometryPath)
{
  if (!geometryPath.getLength() || geometryCollection.isEmpty())
    return 0;
    
  for (size_t i = 0; i < geometryCollection.length(); i++)
    if (geometryCollection(i)->getMediaPath() == geometryPath)
      return geometryCollection(i);
      
  return 0;
}

GeometryInfo *GeometryManager::getGeometryInfo(unsigned int index)
{
  return (index >= geometryCollection.length()) ? 0 : geometryCollection(index);
}

void GeometryManager::flushUnusedGeometry()
{
  ArrayPtr <GeometryInfo> validGeometry;
  
  for (size_t i = 0; i < geometryCollection.length(); i++)
    if (geometryCollection(i)->getUserCount() > 0)
      validGeometry.append(geometryCollection(i));
    else
    {
      deleteObject(geometryCollection(i)->getMedia());
      deleteObject(geometryCollection(i));
    }
    
  geometryCollection = validGeometry;
}

void GeometryManager::flushAllGeometries()
{
  size_t i;
  
  flushUnusedGeometry();
  
  for (i = 0; i < geometryCollection.length(); i++)
  {
    deleteObject(geometryCollection(i)->getMedia());
    deleteObject(geometryCollection(i));
  }
  
  geometryCollection.clear();
  
  if (i) Logger::writeInfoLog(String("Flushed ") + int(i) + String(" active geometries"));
}

void GeometryManager::printGeometryInfo()
{
  if (geometryCollection.isEmpty())
    cout << "This Manager contains no geometries." << endl;
  else
  {
    cout << "Geometry list: " << endl;
    for (size_t i = 0; i < geometryCollection.length(); i++)
      cout <<  "<users = \"" << geometryCollection(i)->getUserCount() << "\" "
           <<  " path = \"" << geometryCollection(i)->getMediaPath() << "\" />" << endl;
  }
}
