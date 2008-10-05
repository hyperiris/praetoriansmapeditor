#ifndef PRAETORIANSMISSIONSCRIPT
#define PRAETORIANSMISSIONSCRIPT

#include "MissionScript.h"
#include "../Containers/ocavltree.h"

class PraetoriansMissionScript : public MissionScript
{
  public:
    PraetoriansMissionScript();
    ~PraetoriansMissionScript();
    
  public:
    bool exportData(const char* projectName);
    
  private:
    AVLTree <string> objectTypes;
};

#endif