#ifndef ARCHIVEMAPPING
#define ARCHIVEMAPPING

#include <windows.h>
#include "ArchivedFile.h"
#include "../Containers/ocavltreet.h"

class ArchiveMapping
{
  public:
    ArchiveMapping();
    ~ArchiveMapping();
    
  public:
    bool createMapping(const char* path);
    void close();
    
    bool createView(const char* name, unsigned int start, unsigned int bytes, ArchivedFile *file);
    void unmapView(ArchivedFile* file);
    
    unsigned int getFileSize();
    
  private:
    HANDLE hf;
    HANDLE hfm;
    
    AVLTreeT <String, LPVOID, 4> openViews;
    
    unsigned int flen;
    unsigned int granularity;
};

#endif
