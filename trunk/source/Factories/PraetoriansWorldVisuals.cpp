#include "PraetoriansWorldVisuals.h"
#include "../FileSystem/FileSystem.h"
#include "../FileSystem/ArchivedFile.h"
#include "../Managers/ManagersUtils.h"
#include "../Kernel/Gateway.h"

PraetoriansWorldVisuals::PraetoriansWorldVisuals() : objects(1000)
{
}

bool PraetoriansWorldVisuals::load(const char* path)
{
  ///unpacked files have higher priority
  const char* p = MediaPathManager::lookUpMediaPath(path);
  if (p)
    return loadUnpackedMedia(p);
    
  if (FileSystem::fileExist(path))
    return loadPackedMedia(path);
    
  return false;
}

bool PraetoriansWorldVisuals::loadPackedMedia(const char* path)
{
  ArchivedFile* file;
  unsigned int objectcount;
  char objectname[64];
  int objtype;
  
  if (!(file = FileSystem::checkOut(path)))
    return Logger::writeErrorLog(String("Could not load ") + path);
    
  file->seek(4, SEEKD);
  file->read(&objectcount, 4);
  
  for (unsigned int i = 0; i < objectcount; i++)
  {
    WorldObject object;
    
    file->read(&object.flags, 4);
    file->read(objectname, 64);
    file->read(&object.position.z, 4);
    file->read(&object.position.y, 4);
    file->read(&object.position.x, 4);
    file->read(&object.orientation, 4);
    file->read(&object.windFactor, 4);
    object.name = objectname;
    
    Gateway::findType(objectname, objtype);
    object.type = objtype;
    
    objects.append(object);
  }
  
  FileSystem::checkIn(file);
  
  return true;
}

bool PraetoriansWorldVisuals::loadUnpackedMedia(const char* path)
{
  unsigned int objectcount;
  char objectname[64];
  int objtype;
  
  ifstream in(path, ios_base::binary);
  if (!in.is_open())
    return Logger::writeErrorLog(String("Could not load -> ") + path);
    
  in.seekg(4, ios_base::cur);
  in.read((char*)&objectcount, 4);
  
  for (unsigned int i = 0; i < objectcount; i++)
  {
    WorldObject object;
    
    in.read((char*)&object.flags, 4);
    in.read(objectname, 64);
    in.read((char*)&object.position.z, 4);
    in.read((char*)&object.position.y, 4);
    in.read((char*)&object.position.x, 4);
    in.read((char*)&object.orientation, 4);
    in.read((char*)&object.windFactor, 4);
    object.name = objectname;
    
    Gateway::findType(objectname, objtype);
    object.type = objtype;
    
    objects.append(object);
  }
  
  in.close();
  
  return true;
}

bool PraetoriansWorldVisuals::exportData(const char* projectName)
{
  String path;
  char signature[4] = {'M','O','B',0x01};
  char objname[64];
  unsigned int objCount;
  unsigned int objtype;
  WorldObject* object;
  
  path = Gateway::getExportPath();
  objCount = objects.length();
  
  ofstream out((path + "Mapas/" + projectName + ".MOB").getBytes(), ios_base::binary);
  
  if (!out.is_open())
    return Logger::writeErrorLog("Could not export terrain objects");
    
  out.write(signature, 4);
  out.write((char*)&objCount, 4);
  
  for (unsigned int i = 0; i < objCount; i++)
  {
    object = &objects(i);
    
    objtype = object->type;
    
    if ((objtype != WorldObjectTypes::NATURE) && (objtype != WorldObjectTypes::STRUCTURE))
      continue;
      
    out.write((char*)&object->flags, 4);
    
    memset(objname, 0, 64);
    memcpy(objname, object->name.getBytes(), object->name.getLength());
    out.write((char*)objname, 64);
    
    out.write((char*)&object->position.z, 4);
    out.write((char*)&object->position.y, 4);
    out.write((char*)&object->position.x, 4);
    out.write((char*)&object->orientation, 4);
    out.write((char*)&object->windFactor, 4);
  }
  
  out.close();
  
  return true;
}

void PraetoriansWorldVisuals::addObject(const WorldObject& object)
{
  objects.append(object);
}

void PraetoriansWorldVisuals::destroyObjects(unsigned int type)
{
  Array <WorldObject> validObjects(objects.capacity());
  for (unsigned int i = 0; i < objects.length(); i++)
    if (objects(i).type != type)
      validObjects.append(objects(i));
      
  objects = validObjects;
}

unsigned int PraetoriansWorldVisuals::getObjectCount()
{
  return objects.length();
}

WorldObject* PraetoriansWorldVisuals::getObject(unsigned int index)
{
  return &objects(index);
}

void PraetoriansWorldVisuals::reset()
{
  objects.clear();
}

PraetoriansWorldVisuals::~PraetoriansWorldVisuals()
{
}