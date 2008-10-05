#ifndef NATURELABSCENE
#define NATURELABSCENE

#include "Scene.h"
#include "../Databases/DatabaseResources.h"
#include "../Geometry/TexturedGrid.h"
#include "../Geometry/Axis.h"
#include "../Renderer/Renderer.h"

class NatureLabScene : public Scene
{
  public:
    NatureLabScene(const char* name = "NatureLabScene");
    ~NatureLabScene();
    
  public:
    bool initialize();
    void beginScene();
    void endScene();
    void update(const FrameInfo &frameInfo);
    
    void actionPerformed(GUIEvent &evt);
    void handleKeyEvent(KeyEvent evt, int extraInfo);
    
  private:
    GSElementInfo* getGSElementInfo(const char* name);
    void orbitCamera(float tick);
    void reset();
    
    void drawFullScreenImage(const GSElementInfo& info);
    void drawFullScreenQuad(int width, int height);
    
    void firstGroup();
    void lastGroup();
    void nextGroup();
    void previousGroup();
    
    void flushUnusedGroups();
    
    void updateLabels();
    void updateButtonTextures(unsigned int st, unsigned int en);
    void updateOffscreenTexture(Texture *targetTexture);
    
  private:
    AVLTreeT <String, GSElementInfo, 4> elementInfoAVLTree;
    Array <GSElementInfo> elementInfoList;
    
    Texture buttonTextures[14];
    Texture offscreenTexture;
    
    GSElementInfo currentInfo;
    
    TransformGroup* rootGroup;
    TransformGroup* currentGroup;
    
    Frustum frustum;
    Camera camera;
    
    GUILabel* currentSetLabel;
    GUILabel* modelNameLabel;
    GUILabel* modelTriLabel;
    GUILabel* modelWidthLabel;
    GUILabel* modelHeightLabel;
    GUIPanel* modelPropsPanel;
    
    TexturedGrid grid;
    Axis axis;
    
    float angle;
    float camDistance;
    
    int resolution;
    int startIndex;
    int endIndex;
    
    bool drawBounds;
    bool drawAxis;
    
    int capacity;
};

#endif
