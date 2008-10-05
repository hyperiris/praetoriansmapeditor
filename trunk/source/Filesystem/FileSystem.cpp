#include "FileSystem.h"
#include "DirectoryScanner.h"
#include "Archive.h"
#include "../Managers/ArchivedItemManager.h"
#include "../Managers/MediaPathManager.h"
#include "../Kernel/Gateway.h"

Array <OCString> FileSystem::registeredNames;
ArrayPtr <Archive> FileSystem::archives;
GenericPool <ArchivedFile, 20, 10> FileSystem::openedFiles;

bool FileSystem::initialize()
{
  DirectoryScanner    dir;
  DirectoryItem       item;
  const char*         path;
  OCString            ext;
  OCString::size_type idx;
  HKEY                hkey;
  DWORD               buffersize;
  char*               regval;
  
  if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Pyro Studios\\Praetorians Game\\Paths", 0, KEY_READ, &hkey))
  {
    RegQueryValueEx(hkey, "Raiz", 0, 0, 0, &buffersize);
    regval = new char[buffersize];
    RegQueryValueEx(hkey, "Raiz", 0, 0, (LPBYTE)regval, &buffersize);
    String strkey = String(regval) + "DATA\\";
    strkey.convertChars('\\', '/');
    MediaPathManager::registerPath(strkey);
    deleteArray(regval);
  }
  
  for (int i = 0; i < MediaPathManager::getPathCount(); i++)
  {
    path = MediaPathManager::getPath(i);
    dir.set(path);
    
    while (dir.next())
    {
      item = dir.currentItem();
      
      if (DIR_FILE == item.type)
      {
        idx = item.name.rfind(".");
        
        if (idx == OCString::npos)
          continue;
          
        ext = item.name.substr(idx + 1);
        
        if (ext == "PAK" || ext == "pak" || ext == "ZIP" || ext == "zip" || ext == "PAT" || ext == "pat")
          if (!FileSystem::registerArchive(OCString(path + item.name).c_str()))
            Logger::writeWarningLog(String("Could not register archive -> ") + item.name.c_str());
      }
    }
  }
  
  return true;
}

bool FileSystem::registerArchive(const char* path)
{
  if (!strlen(path))
    return false;
    
  OCString tpath(path);
  
  tpath = tpath.substr(tpath.rfind('/') + 1);
  
  if (FileSystem::checkForRepeat(tpath.c_str()))
    return true;
    
  Archive* archive = new Archive(tpath.substr(0, tpath.rfind('.')).c_str());
  
  if (!archive->open(path))
  {
    deleteObject(archive);
    return false;
  }
  
  registeredNames.append(tpath);
  archives.append(archive);
  
  return true;
}

ArchivedFile* FileSystem::checkOut(const char* name)
{
  ArchivedFile* p = openedFiles.allocate();
  
  if (!checkOut(name, p))
  {
    openedFiles.recycle(p);
    return 0;
  }
  
  return p;
}

bool FileSystem::checkOut(const char* name, ArchivedFile* file)
{
  ZipEntry entry;
  unsigned char* buf = 0;
  ArchiveInfo* info;
  
  file->reset();
  
  if (!strlen(name) || !file || archives.length() == 0)
    return Logger::writeWarningLog(String("FileSystem::checkOut - > ") + name);
    
  for (unsigned int i = 0; i < archives.length(); i++)
    if (archives(i)->findEntry(name, &entry))
    {
      info = ArchivedItemManager::getItemInfo(entry.zipname.c_str());
      
      if (info)
      {
        info->increaseUserCount();
        buf = info->getMedia();
      }
      else
      {
        //buf = new unsigned char[entry.zipuncmp];
        buf = (unsigned char*) Gateway::aquireFilebufferMemory(entry.zipuncmp);
        
        if (!archives(i)->unpackEntry(&entry, buf))
        {
          //deleteArray(buf);
          Gateway::releaseFilebufferMemory(buf);
          return Logger::writeErrorLog(String("Could not unpack file -> ") + name);
        }
        
        ArchivedItemManager::addItemInfo(new ArchiveInfo(entry.zipname.c_str(), buf));
      }
      
      file->size = entry.zipuncmp;/// <-- ?
      file->setName(entry.zipname.c_str());
      file->data = buf;
      
      return true;
    }
    
  return false;
}

bool FileSystem::fileExist(const char* name)
{
  if (!strlen(name) || archives.length() == 0)
    return false;
    
  for (unsigned int i = 0; i < archives.length(); i++)
    if (archives(i)->containsEntry(name))
      return true;
      
  return false;
}

void FileSystem::checkIn(ArchivedFile* file)
{
  if (!file || !file->data)
    return;
    
  ArchiveInfo* info = ArchivedItemManager::getItemInfo(file->getName());
  
  if (info)
  {
    info->decreaseUserCount();
    if (info->getUserCount() == 0)
      Gateway::releaseFilebufferMemory(info->getMedia());
    ArchivedItemManager::flushUnusedItems();
    file->reset();
  }
  
  openedFiles.recycle(file);
}

bool FileSystem::checkForRepeat(const char* path)
{
  return registeredNames.contains(path);
}

void FileSystem::printAllRegisteredNames()
{
  for (unsigned int i = 0; i < registeredNames.length(); i++)
    Logger::writeInfoLog(String("Registered  archive -> ") + registeredNames(i).c_str());
}

void FileSystem::printAllPaths()
{
  for (unsigned int i = 0; i < archives.length(); i++)
    archives(i)->printAllPaths();
}

void FileSystem::printAllEntries()
{
  for (unsigned int i = 0; i < archives.length(); i++)
    archives(i)->printAllEntries();
}

void FileSystem::reset()
{
  //ArchivedItemManager::flushAllItems();
  ArchiveInfo* info;
  for (unsigned int i = 0; i < ArchivedItemManager::getInfoCount(); i++)
  {
    info = ArchivedItemManager::getItemInfo(i);
    if (info->getUserCount() == 0)
      Gateway::releaseFilebufferMemory(info->getMedia());
  }
  ArchivedItemManager::flushUnusedItems();
}

void FileSystem::destroy()
{
  //for (size_t i = 0; i < archives.length(); i++)
  //  archives(i)->close();
  ArchiveInfo* info;
  for (unsigned int i = 0; i < ArchivedItemManager::getInfoCount(); i++)
  {
    info = ArchivedItemManager::getItemInfo(i);
    if (info->getUserCount() == 0)
      Gateway::releaseFilebufferMemory(info->getMedia());
  }
  
  ArchivedItemManager::flushAllItems();
  
  archives.clearAndDestroy();
  registeredNames.clear();
}
