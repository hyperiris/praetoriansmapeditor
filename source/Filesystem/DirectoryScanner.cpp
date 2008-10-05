#include "DirectoryScanner.h"

DirectoryScanner::DirectoryScanner() : hf(INVALID_HANDLE_VALUE)
{}

DirectoryScanner::DirectoryScanner(const OCString& dir) : hf(INVALID_HANDLE_VALUE)
{
  set(dir);
}

void DirectoryScanner::set(const OCString& dir)
{
  unsigned int attrs = GetFileAttributes(dir.c_str());
  
  if (!(attrs & 0x00000010))
    return;
    
  OCString fullPath(dir);
  
  if (fullPath.length() > 0 && fullPath[fullPath.length()-1] != '/')
    fullPath += '/';
    
  hf = FindFirstFile((fullPath + "*").c_str(), &currEntry);
}

DirectoryItem DirectoryScanner::currentItem()
{
  DirectoryItem item;
  unsigned int attrs = currEntry.dwFileAttributes;
  item.name = currEntry.cFileName;
  item.type = (0x00000020 & attrs) ? DIR_FILE : (0x00000010 & attrs) ? DIR_FOLDER : DIR_NOTUSED;
  return item;
}

bool DirectoryScanner::next()
{
  if (FindNextFile(hf, &currEntry) == 0)
    return false;
    
  return true;
}

DirectoryScanner::~DirectoryScanner()
{
  if (hf != INVALID_HANDLE_VALUE)
    FindClose(hf);
}
