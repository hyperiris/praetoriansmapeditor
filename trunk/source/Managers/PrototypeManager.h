#ifndef PROTOTYPEMANAGER
#define PROTOTYPEMANAGER

#include "../Tools/MediaInfo.h"
#include "../Containers/ocarray.h"

class TransformGroup;

typedef MediaInfo <TransformGroup*> PrototypeInfo;

class PrototypeManager
{
  public:
    static bool addPrototypeInfo(PrototypeInfo *prototypeInfo);
    static PrototypeInfo *getPrototypeInfo(const char* prototypePath);
    static PrototypeInfo *getPrototypeInfo(const String& prototypePath);
    static PrototypeInfo *getPrototypeInfo(unsigned int index);
    
    static void printPrototypeInfo();
    static void flushAllPrototypes();
    static void flushUnusedPrototypes();
    
  private:
    static ArrayPtr <PrototypeInfo> prototypeCollection;
};

#endif