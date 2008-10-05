#ifndef NEWMAPSCENE
#define NEWMAPSCENE

#include "Scene.h"
#include "../Factories/FactoryUtils.h"
#include "../Containers/ocarray.h"

class NewmapScene : public Scene
{
  public:
    NewmapScene(const char* name = "NewmapScene");
    ~NewmapScene();
    
  public:
    bool initialize();
    void beginScene();
    void endScene();
    void update(const FrameInfo &frameInfo);
    void actionPerformed(GUIEvent &evt);
    void handleKeyEvent(KeyEvent evt, int extraInfo);
    
  private:
    bool loadTemplateNames();
    void showMap(unsigned int index);
    void drawFullScreenQuad(int width, int height);
    void drawLoadingScreen(int randomNumber);
    bool isProjectNameValid(const char* str);
    void handleAcceptingCallback();
    
  private:
    GUIButton* buttonSurface;
    GUIPanel* messagePanel;
    GUILabel* messageLabel;
    GUITextBox* textbox;
    
    Array <MapDescriptor> mapDescriptorList;
    int currentItem;
};

#endif