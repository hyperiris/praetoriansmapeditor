#ifndef MAINMENUSCENE
#define MAINMENUSCENE

#include "Scene.h"

class MainMenuScene : public Scene
{
  public:
    MainMenuScene(const char* name = "MainMenuScene");
    ~MainMenuScene();
    
  public:
    void beginScene();
    void endScene();
    void update(const FrameInfo &frameInfo);
    void actionPerformed(GUIEvent &evt);
    
  private:
    void drawFullScreenQuad(int width, int height);
};

#endif
