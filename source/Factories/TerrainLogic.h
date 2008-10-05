#ifndef TERRAINLOGIC
#define TERRAINLOGIC

class TerrainLogic
{
  public:
    TerrainLogic() {}
    virtual ~TerrainLogic() {}
    
  public:
    virtual bool load(const char* path) = 0;
    virtual bool exportData(const char* projectName) = 0;
    
    virtual bool createLogicMap(unsigned int width, unsigned int height) = 0;
    
    virtual unsigned int getFlags(unsigned int index) = 0;
    virtual unsigned int getFlagCount() = 0;
    virtual void setFlags(unsigned int index, unsigned int flags) = 0;
    virtual void reset() = 0;
    
};

#endif