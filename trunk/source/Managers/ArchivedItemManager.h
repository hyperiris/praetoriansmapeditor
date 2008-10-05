#ifndef ARCHIVEDITEMMANAGER
#define ARCHIVEDITEMMANAGER

#include "../Tools/MediaInfo.h"
#include "../Containers/ocarray.h"

typedef MediaInfo <unsigned char*> ArchiveInfo;

class ArchivedItemManager
{
    static ArrayPtr <ArchiveInfo> infoList;
    
  public:
    static bool         addItemInfo(ArchiveInfo* info);
    static ArchiveInfo* getItemInfo(const char* path);
    static ArchiveInfo* getItemInfo(unsigned int index);
    static unsigned int getInfoCount();
    
    static void         flushUnusedItems();
    static void         flushAllItems();
};

#endif
