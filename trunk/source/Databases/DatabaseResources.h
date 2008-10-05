#ifndef DATABASERESOURCES
#define DATABASERESOURCES

#include "../Appearance/Texture.h"

class TransformGroup;

struct GSElementInfo
{
  GSElementInfo()
  {
    coords.set(0);
    index = 0;
    activeGroup = 0;
    population = 400;
    maxpopulation = 400;
    windFactor = 0.79f;
  }
  
  GSElementInfo(const GSElementInfo& copy)
  {
    if (this != &copy)
      this->operator = (copy);
  }
  
  GSElementInfo &operator = (const GSElementInfo& right)
  {
    if (this != &right)
    {
      texture.destroy();
      
      if (right.texture.getID())
        texture = right.texture;
        
      name    = right.name;
      coords  = right.coords;
      index   = right.index;
      activeGroup = right.activeGroup;
      population = right.population;
      maxpopulation = right.maxpopulation;
      windFactor = right.windFactor;
    }
    return *this;
  }
  
  void reset()
  {
    name.clear();
    coords.set(0);
    texture.destroy();
    index = 0;
    activeGroup = 0;
    population = 0;
    maxpopulation = 0;
    windFactor = 0.79f;
  }
  
  String name;
  Tuple4f coords;
  Texture texture;
  unsigned int index;
  TransformGroup* activeGroup;
  unsigned int population;
  unsigned int maxpopulation;
  float windFactor;
};

class TSElementInfo
{
  public:
    TSElementInfo()
    {
      texture = 0;
      pageIndex = 0;
      coordsIndex = 0;
    }
    
    TSElementInfo(const TSElementInfo& copy)
    {
      if (this != &copy)
        this->operator = (copy);
    }
    
    TSElementInfo &operator = (const TSElementInfo& right)
    {
      if (this != &right)
      {
        texture = right.texture;
        pageIndex = right.pageIndex;
        coordsIndex = right.coordsIndex;
        coords = right.coords;
      }
      return *this;
    }
    
    void reset()
    {
      texture = 0;
      pageIndex = 0;
      coordsIndex = 0;
      coords.set(0);
    }
    
    Texture *texture;
    unsigned int pageIndex;
    unsigned int coordsIndex;
    Tuple4f coords;
};

#endif