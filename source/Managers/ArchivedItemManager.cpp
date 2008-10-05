#include "ArchivedItemManager.h"
#include "../Tools/Logger.h"
#include "../Math/MathUtils.h"

ArrayPtr <ArchiveInfo> ArchivedItemManager::infoList;

bool ArchivedItemManager::addItemInfo(ArchiveInfo* info)
{
  if (!info || !info->getMedia())
    return false;
    
  ArchivedItemManager::flushUnusedItems();
  
  infoList.append(info);
  
  return true;
}

ArchiveInfo* ArchivedItemManager::getItemInfo(unsigned int index)
{
  if (index >= infoList.length())
    return 0;
    
  return infoList(index);
}

ArchiveInfo* ArchivedItemManager::getItemInfo(const char* path)
{
  if (!strlen(path) || !infoList.length())
    return 0;
    
  for (unsigned int i = 0; i < infoList.length(); i++)
    if (infoList(i)->getMediaPath() == path)
      return infoList(i);
      
  return 0;
}

unsigned int ArchivedItemManager::getInfoCount()
{
  return infoList.length();
}

void ArchivedItemManager::flushUnusedItems()
{
  ArrayPtr <ArchiveInfo> validInfo;
  ArchiveInfo* info;
  
  for (unsigned int i = 0; i < infoList.length(); i++)
  {
    info = infoList(i);
    
    if (info->getUserCount() > 0)
      validInfo.append(info);
    else
    {
      //deleteArray(info->getMedia());
      deleteObject(info);
    }
  }
  
  infoList = validInfo;
}

void ArchivedItemManager::flushAllItems()
{
  ArchiveInfo* info;
  for (size_t i = 0; i < infoList.length(); i++)
  {
    info = infoList(i);
    //deleteArray(info->getMedia());
    deleteObject(info);
  }
  
  infoList.clear();
}
