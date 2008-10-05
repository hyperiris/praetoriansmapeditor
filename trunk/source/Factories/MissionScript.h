#ifndef MISSIONSCRIPT
#define MISSIONSCRIPT

class MissionScript
{
  public:
    virtual ~MissionScript() {}
    
  public:
    virtual bool exportData(const char* projectName) = 0;
};

#endif