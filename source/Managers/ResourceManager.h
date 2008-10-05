#ifndef RESOURCEMANAGER
#define RESOURCEMANAGER

#include "../Memory/PageFile.h"
#include "../Memory/BlockMemory.h"

class ResourceManager
{
  public:
    static void* aquire1K(unsigned int bytes);
    static void release1K(void* addr);
    
  private:
    static PageFile pageFile;
    static BlockMemory memory1K;
};

#endif