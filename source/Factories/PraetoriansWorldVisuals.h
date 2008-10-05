#ifndef PRAETORIANSWORLDVISUALS
#define PRAETORIANSWORLDVISUALS

#include "WorldVisuals.h"
#include "FactoryUtils.h"
#include "../Containers/ocarray.h"

class PraetoriansWorldVisuals : public WorldVisuals
{
  public:
    PraetoriansWorldVisuals();
    ~PraetoriansWorldVisuals();
    
  public:
    bool load(const char* path);
    bool exportData(const char* projectName);
    void reset();
    
    void addObject(const WorldObject& object);
    void destroyObjects(unsigned int type);
    
    unsigned int getObjectCount();
    WorldObject* getObject(unsigned int index);
    
  private:
    bool loadPackedMedia(const char* path);
    bool loadUnpackedMedia(const char* path);
    
  private:
    Array <WorldObject> objects;
};

#endif