#ifndef PAGEFILE
#define PAGEFILE

//#include "../Tools/Singleton.h"
#include "../Containers/ocarray.h"
#include "PageDescriptor.h"
#include <windows.h>

class PageFile// : public Singleton
{

  public:
    PageFile();
    ~PageFile();
    
  public:
    bool initialize(unsigned long megs);
    void* commit(unsigned long bytes);
    bool decommit(void* addr);
    bool reset(void* addr);
    void lock(void *addr, bool value);
    bool pageoutLRU();
    
    void reset();
    void destroy();
    
    unsigned long getUsedSize();
    unsigned long getTotalSize();
    unsigned long getPeakSize();
    
    void printProfile();
    
    bool ensureFreeMemory(unsigned long size);
    
  private:
    void reserve(unsigned long megs);
    void mapPages();
    unsigned long moveCursor();
    PageDescriptor* getRegion(unsigned long bytes);
    
  private:
    bool initialized;
    
    HANDLE pagefile;
    
    unsigned long allocmin;
    unsigned long pgsize;
    unsigned long filesize;
    unsigned long pfaddr;
    unsigned long peaksize;
    unsigned long usedsize;
    
    PageDescriptor* pCur;
    
    Array <PageDescriptor> descriptors;
};

#endif