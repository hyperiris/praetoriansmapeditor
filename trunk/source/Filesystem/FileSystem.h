#ifndef FILESYSTEM
#define FILESYSTEM

#include "../Memory/GenericPool.h"
#include "../Containers/ocarray.h"
#include "../Containers/ocstring.h"

class Archive;
class ArchivedFile;

class FileSystem
{
  public:
    static bool initialize();
    static void reset();
    static void destroy();
    
    static bool registerArchive(const char* path);
    static ArchivedFile* checkOut(const char* name);
    static void checkIn(ArchivedFile* file);
    static bool fileExist(const char* name);
    
    static void printAllPaths();
    static void printAllEntries();
    static void printAllRegisteredNames();
    
  private:
    static bool checkOut(const char* name, ArchivedFile* file);
    static bool checkForRepeat(const char* path);
    
  private:
    static Array <OCString> registeredNames;
    static ArrayPtr <Archive> archives;
    static GenericPool <ArchivedFile, 20, 10> openedFiles;
};

#endif
