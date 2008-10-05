#include "PageFile.h"
#include "../Tools/Logger.h"

PageFile::PageFile()
{
  initialized = false;
  peaksize = 0;
  usedsize = 0;
}

bool PageFile::initialize(unsigned long megs)
{
  if (initialized)
    return true;
    
  unsigned long pagecount;
  
  SYSTEM_INFO sysInfo;
  
  GetSystemInfo(&sysInfo);
  
  pgsize = sysInfo.dwAllocationGranularity;
  //pgsize = 16 * 1024;
  
  allocmin = pgsize / 2;
  
  filesize = megs * 1024 * 1024;
  
  pagecount = filesize / pgsize;
  
  if (filesize % pgsize != 0)
    filesize = pgsize * pagecount;
    
  pfaddr = (unsigned long) VirtualAlloc(NULL, filesize, MEM_RESERVE, PAGE_NOACCESS);
  
  if (NULL == pfaddr)
    return Logger::writeErrorLog(String("Couldnt reserve ") + int(filesize) + " bytes of memory");
    
  for (unsigned long i = 0; i < pagecount; i++)
    descriptors.append(PageDescriptor());
    
  if (!initialized)
    pCur = &descriptors(0);
    
  mapPages();
  
  initialized = true;
  
  return true;
}

void PageFile::reserve(unsigned long megs)
{
}

void* PageFile::commit(unsigned long bytes)
{
  if (!initialized)
    return 0;
    
  PageDescriptor* p;
  DWORD mem;
  
  //if (bytes < allocmin)
  //  bytes = allocmin;
  
  p = getRegion(bytes);
  
  if (!p)
  {
    Logger::writeErrorLog(String("Bad memory request: ") + int(bytes));
    return 0;
  }
  
  mem = (DWORD) VirtualAlloc((void*)p->addr, p->size, MEM_COMMIT, PAGE_READWRITE);
  
  peaksize = peaksize < p->size ? p->size : peaksize;
  usedsize += p->size;
  
  return (void*) mem;
}

bool PageFile::decommit(void* addr)
{
  if (!initialized)
    return false;
    
  if ((DWORD)addr < pfaddr || (DWORD)addr >= pfaddr + (descriptors.length() * pgsize))
    return Logger::writeErrorLog("Unable to decommit memory block");
    
  PageDescriptor* p = &descriptors(((DWORD)addr - pfaddr) / pgsize);
  
  VirtualFree((void*)p->addr, p->size, MEM_DECOMMIT);
  
  usedsize -= p->size;
  p->size = 0;
  
  pCur = p;
  
  return true;
}

bool PageFile::reset(void* addr)
{
  if (!initialized)
    return false;
    
  if ((DWORD)addr < pfaddr || (DWORD)addr >= pfaddr + (descriptors.length() * pgsize))
    return false;
    
  PageDescriptor* p = &descriptors(((DWORD)addr - pfaddr) / pgsize);
  
  //addr = (void*) VirtualAllocEx(NULL, (void*)p->addr, p->size, MEM_RESET | MEM_COMMIT, PAGE_READWRITE);
  VirtualFree((void*)p->addr, p->size, MEM_DECOMMIT);
  addr = (void*) VirtualAlloc((void*)p->addr, p->size, MEM_COMMIT, PAGE_READWRITE);
  
  return true;
}

bool PageFile::pageoutLRU()
{
  return false;
}

PageDescriptor* PageFile::getRegion(unsigned long bytes)
{
  PageDescriptor* pMark = 0;
  
  for (unsigned int i = 0; i < descriptors.length(); i++)
  {
    //if (!pCur)
    //  pCur = &descriptors[0];
    
    if (bytes <= pCur->cap && !pCur->locked)
      if (pCur->withinRange(bytes))
      {
        pMark = pCur;
        pCur->size = bytes;
        
        moveCursor();
        
        break;
      }
      
    i += moveCursor();
  }
  
  return pMark;
}

unsigned long PageFile::moveCursor()
{
  unsigned long pageskip;
  unsigned long pageindex;
  
  pageindex = (pCur->addr - pfaddr) / pgsize;
  
  pageskip = (pCur->size == 0) ? 1 : (pCur->size % pgsize == 0) ? (pCur->size / pgsize) : (pCur->size / pgsize) + 1;
  
  pCur = &descriptors((pageindex + pageskip) % descriptors.length());
  
  return pageskip;
}

void PageFile::mapPages()
{
  PageDescriptor* iter;
  PageDescriptor* last = &descriptors(0);
  
  for (unsigned int i = 0; i < descriptors.length(); i++)
  {
    iter = &descriptors(i);
    iter->addr = pfaddr + (i * pgsize);
    iter->cap  = filesize - (i * pgsize);
    iter->base = i == 0 ? true : false;
    
    last->next = iter;
    last = iter;
  }
}

void PageFile::lock(void *addr, bool value)
{
  PageDescriptor* p;
  unsigned long pagecnt;
  
  if (!initialized)
    return;
    
  if ((DWORD)addr < pfaddr || (DWORD)addr >= pfaddr + (descriptors.length() * pgsize))
    return;
    
  p = &descriptors(((DWORD)addr - pfaddr) / pgsize);
  pagecnt = (p->size == 0) ? 1 : (p->size % pgsize == 0) ? (p->size / pgsize) : (p->size / pgsize) + 1;
  
  //if(!pages[pageIndex].committed)
  //{
  // if(!PageFault(ptr))
  //  return;
  //}
  
  for (unsigned int i = 0; i < pagecnt; i++)
  {
    p->locked = value;
    p = p->next;
  }
}

bool PageFile::ensureFreeMemory(unsigned long size)
{
  return true;
}

unsigned long PageFile::getUsedSize()
{
  return usedsize;
}

unsigned long PageFile::getTotalSize()
{
  return filesize;
}

unsigned long PageFile::getPeakSize()
{
  return peaksize;
}

void PageFile::printProfile()
{
  Logger::writeInfoLog(String("Total PF size: ") + int(filesize));
  Logger::writeInfoLog(String("Peak size: ") + int(peaksize));
  Logger::writeInfoLog(String("Available size: ") + int(filesize - peaksize));
}

void PageFile::reset()
{
  peaksize = 0;
  usedsize = 0;
  pCur = &descriptors(0);
}

void PageFile::destroy()
{
  //printProfile();
  
  if (initialized)
    VirtualFree((void*)pfaddr, 0, MEM_RELEASE);
    
  descriptors.clear();
  
  peaksize = 0;
  
  initialized = false;
}

PageFile::~PageFile()
{
  destroy();
}
