#include "ArchiveMapping.h"

ArchiveMapping::ArchiveMapping()
{
  SYSTEM_INFO info;
  GetSystemInfo(&info);
  granularity = info.dwAllocationGranularity;
  flen = 0;
  hf = NULL;
  hfm = NULL;
}

bool ArchiveMapping::createMapping(const char* path)
{
  hf = CreateFile(path, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
  
  if (INVALID_HANDLE_VALUE == hf)
    return false;
    
  hfm = CreateFileMapping(hf, 0, PAGE_READONLY, 0, 0, 0);
  
  if (NULL == hfm)
  {
    CloseHandle(hf);
    return false;
  }
  
  flen = GetFileSize(hf, 0);
  
  return true;
}

bool ArchiveMapping::createView(const char* name, unsigned int start, unsigned int bytes, ArchivedFile *file)
{
  if (start > flen || bytes > flen || start == bytes || bytes == 0 || !name)
    return false;
    
  unsigned int base;
  
  LPVOID ptr;
  
  if (openViews.contains(name))// <-- allow only one for now
    return false;
    
  base = (start / granularity) * granularity;
  
  ptr = MapViewOfFile(hfm, FILE_MAP_READ, 0, base, (start - base) + bytes);
  
  if (ptr == NULL)
    return false;
    
  file->setName(name);
  file->data = (unsigned char*) ptr + (start - base);
  file->size = bytes;
  
  openViews.insertKeyAndValue(name, ptr);
  
  return true;
}

void ArchiveMapping::unmapView(ArchivedFile* file)
{
  if (!file->data)
    return;
    
  if (openViews.contains(file->getName()))
  {
    UnmapViewOfFile(openViews[file->getName()]);
    openViews.remove(file->getName());
    file->reset();
  }
}

unsigned int ArchiveMapping::getFileSize()
{
  return flen;
}

void ArchiveMapping::close()
{
  AVLTreeTIterator <String, LPVOID, 4> iter(openViews);
  
  while (iter.next())
    UnmapViewOfFile(iter.value());
    
  CloseHandle(hf);
  CloseHandle(hfm);
  
  openViews.clear();
}

ArchiveMapping::~ArchiveMapping()
{
  close();
}
