#ifndef ARCHIVE
#define ARCHIVE

#include "ArchiveMapping.h"
#include "../Tools/ICString.h"

#pragma pack(2)

struct ZipDirectory
{
  unsigned int zipdirsize;
  unsigned int zipdirdata;
  unsigned short zipentrcnt;
  unsigned short zipentrtot;
};

struct ZipEntry
{
  enum
  {
    FILE = 0x00000020,
    DIRECTORY = 0x00000010
  };
  
  unsigned int zipsize;
  unsigned int zipuncmp;
  unsigned int zipattr;
  unsigned int zipdata;
  unsigned short zipcomp;
  unsigned short zipfnln;
  unsigned short zipxtraln;
  ICString zipname;
};

#pragma pack()

class Archive
{
  public:
    Archive(const char* name = "");
    ~Archive();
    
  public:
    bool open(const char* path);
    void close();
    
    bool unpackEntry(ZipEntry* entry, unsigned char* out);
    bool findEntry(const char* name, ZipEntry* entry);
    bool containsEntry(const char* name);
    
    void printAllPaths();
    void printAllEntries();
    
  private:
    unsigned int verifyZip(unsigned int sig);
    void decryptulong(unsigned int& v);
    void decryptuchar(unsigned char& v);
    void decryptushort(unsigned short& v);
    
  private:
    Array <ICString> directories;
    AVLTreeT  <ICString, ZipEntry, 4> entries;
    
    unsigned int state;
    
    ArchiveMapping archiveMap;
    
    String archivename;
};

#endif
