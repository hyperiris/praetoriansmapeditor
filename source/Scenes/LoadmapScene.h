#ifndef LOADMAPSCENE
#define LOADMAPSCENE

#include "Scene.h"
#include "../Factories/FactoryUtils.h"
#include "../Containers/ocarray.h"

class LoadmapScene : public Scene
{
  public:
    LoadmapScene(const char* name = "LoadmapScene");
    ~LoadmapScene();
    
  public:
    bool initialize();
    void beginScene();
    void endScene();
    void update(const FrameInfo &frameInfo);
    void actionPerformed(GUIEvent &evt);
    
  private:
    bool loadTemplateNames();
    void showMap(unsigned int index);
    void drawFullScreenQuad(int width, int height);
    void drawLoadingScreen(int randomNumber);
    
  private:
    GUIButton* buttonSurface;
    Array <MapDescriptor> mapDescriptorList;
    int currentItem;
};

#endif
