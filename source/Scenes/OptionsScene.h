#ifndef OPTIONSSCENE
#define OPTIONSSCENE

#include "Scene.h"

class OptionsScene : public Scene
{
  public:
    OptionsScene(const char* name = "OptionsScene");
    ~OptionsScene();
    
  public:
    bool initialize();
    void beginScene();
    void actionPerformed(GUIEvent &evt);
    
  private:
    float camDist;
};

#endif
