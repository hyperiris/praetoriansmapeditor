#ifndef MEDIAPATHMANAGER_H
#define MEDIAPATHMANAGER_H

#include "../Tools/Logger.h"
#include <vector>

class MediaPathManager
{
  public:
    static int          getPathCount();
    static const char*  getPath(unsigned int index);
    static bool         registerPath(XMLElement *MediaPathNode);
    static bool         registerPath(const char *path);
    static void         printAllPaths();
    static const char*  lookUpMediaPath(const char *path);
  private:
    static std::vector<String> pathStack;
    static String rpath;
};

#endif