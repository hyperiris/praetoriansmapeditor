#ifndef WORLDVISUALS
#define WORLDVISUALS

struct WorldObject;

class WorldVisuals
{
  public:
    WorldVisuals() {}
    virtual ~WorldVisuals() {}
    
  public:
    virtual bool load(const char* path) = 0;
    virtual bool exportData(const char* projectName) = 0;
    virtual void reset() = 0;
    virtual void addObject(const WorldObject& object) = 0;
    virtual void destroyObjects(unsigned int type) = 0;
    virtual unsigned int getObjectCount() = 0;
    virtual WorldObject* getObject(unsigned int index) = 0;
};

#endif