#ifndef DIRECTORYSCANNER
#define DIRECTORYSCANNER

#include <windows.h>
#include <winbase.h>
#include "../Containers/ocport.h"
#include "../Containers/ocstring.h"

enum OSDIR_FILE_TYPE
{
  DIR_FILE,
  DIR_FOLDER,
  DIR_NOTUSED
};

struct DirectoryItem
{
  OCString name;
  unsigned int type;
};

typedef DWORD OSDIR_ENTRY;

class DirectoryScanner
{

  public:
    DirectoryScanner();
    DirectoryScanner(const OCString& dir);
    ~DirectoryScanner();
    
  public:
    void set(const OCString& dir);
    DirectoryItem currentItem();
    bool next();
    
  private:
    HANDLE hf;
    WIN32_FIND_DATA currEntry;
};

#endif
