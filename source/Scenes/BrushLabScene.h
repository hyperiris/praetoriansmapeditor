#ifndef BRUSHLABSCENE
#define BRUSHLABSCENE

#include "Scene.h"
#include "../Databases/DatabaseResources.h"
#include "../Factories/Tile.h"

class BrushLabScene : public Scene
{
  public:
    BrushLabScene(const char* name = "BrushLabScene");
    ~BrushLabScene();
    
  public:
    bool initialize();
    void beginScene();
    void endScene();
    void update(const FrameInfo &frameInfo);
    void actionPerformed(GUIEvent &evt);
    
  private:
    void updateOffscreenTexture(Texture* targetTexture);
    TSElementInfo* getTSElementInfo(const char* name);
    void drawFullScreenQuadInverse(int width, int height);
    
    void createBrushMatrix(unsigned int width, unsigned int height);
    
    void firstTileSet();
    void lastTileSet();
    void previousTileSet();
    void nextTileSet();
    
  private:
    GUIPanel* tilesetPanel;
    GUILabel* tilesetLabel;
    
    int currentIndex;
    
    TSElementInfo currentTileInfo;
    
    ///this is the 4x4 matrix of button elements stored with their associated callbackStrings
    AVLTreeT <String, TSElementInfo, 4> elementInfoAVLTree;
    
    Array <unsigned int> textureHandles;
    
    Array <String> matrixElementCBStrings;
    
    Array <Tile> matrixTileList;
    
    ///this is the buffer we draw to. all buttons are connected to it
    Texture offscreenTexture;
    
    Texture currentPageTexture;
};

#endif
