#ifndef GEOMETRYMANAGER_H
#define GEOMETRYMANAGER_H

#include "../Tools/MediaInfo.h"
#include "../Containers/ocarray.h"

class Geometry;

typedef MediaInfo <Geometry*> GeometryInfo;

class GeometryManager
{
  public:
    static bool         addGeometryInfo(GeometryInfo *geometryInfo);
    static GeometryInfo *getGeometryInfo(const char* geometryPath);
    static GeometryInfo *getGeometryInfo(const String& geometryPath);
    static GeometryInfo *getGeometryInfo(unsigned int index);
    
    static void flushUnusedGeometry();
    static void printGeometryInfo();
    static void flushAllGeometries();
    
  private:
    static ArrayPtr <GeometryInfo> geometryCollection;
};

#endif