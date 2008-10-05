#ifndef ARCHIVEDFILE
#define ARCHIVEDFILE

#include "../Tools/NamedObject.h"

enum SeekMethod
{
  SEEKA,
  SEEKD,
  SEEKU
};

class ArchivedFile : public NamedObject
{
  public:
    ArchivedFile(const char* n = "\0", unsigned char* b = 0, unsigned int s = 0);
    
  public:
    unsigned int read(void* dest, unsigned int bytes);
    void seek(unsigned int bytes, unsigned int from = SEEKA);
    unsigned int tell();
    void reset();
    
  private:
    void memcopy(void *dst, void *src, unsigned int bytes);
    void memcopyIntel(void* dest, void* src, unsigned int bytes);
    
  public:
    unsigned int index;
    unsigned int size;
    unsigned char *data;
    
  private:
    ArchivedFile& operator = (const ArchivedFile& f) {}
};

#endif
