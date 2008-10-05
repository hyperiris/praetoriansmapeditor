#ifndef EDITORSCENE
#define EDITORSCENE

#include "Scene.h"
#include "../Geometry/Grid.h"
#include "../Geometry/Axis.h"
#include "../Geometry/Cylinder.h"
#include "../Geometry/Ray3D.h"
#include "../Renderer/Renderer.h"
#include "../Databases/DatabaseResources.h"
#include "../Containers/occircularbuffer.h"

class Water;

namespace CursorTypes
{
  enum Enum
  {
    POINTER,
    PAINT,
    FILL,
    MARKER,
    PASTEL,
    RAISE,
    LOWER,
    ERASE,
    RESTORE,
    BURN,
    HEAL,
    TILE_ROTATE,
    TILE_MIRROR,
    TILE_ERASE,
    TILE_LOGIC,
    TILE_FLAG,
    NATURE,
    VILLAGE,
    STRUCTURE,
    CHARACTER,
    CRITTER,
    GRASS
  };
}

class EditorScene : public Scene
{
  public:
    EditorScene(const char* name = "EditorScene");
    ~EditorScene();
    
  public:
    bool initialize();
    void beginScene();
    void pauseScene();
    void endScene();
    void reset();
    void update(const FrameInfo &frameInfo);
    
    void actionPerformed(GUIEvent &evt);
    void handleMouseEvent(MouseEvent& evt, int extraInfo);
    void handleKeyEvent(KeyEvent evt, int extraInfo);
    
  private:
    void updateMinimap();
    void updateOffscreenTexture(Texture* targetTexture);
    void updateMemoryMonitor(float frameInterval);
    
    void drawFullScreenQuad(int width, int height);
    void drawFullScreenQuadInverse(int width, int height);
    void drawCollisionMark();
    void setFlag(unsigned int flag);
    void setLogicFlag(unsigned int flag);
    
    void resetConfiguration();
    void resetTileTextureHandles();
    void resetVisitors();
    void resetCamera();
    void resetFlags();
    void resetGUI();
    void resetMemoryStats();
    
    void addPastureToCombobox(const String& pastureName);
    
    void enableGUI(bool enable);
    
    TSElementInfo* getTSElementInfo(const char* name);
    
    void firstTileSet();
    void lastTileSet();
    void previousTileSet();
    void nextTileSet();
    
  private:
    Frustum frustum;
    Camera  camera;
    
    GUILabel *fpsCounter;
    GUISlider* fogDensity;
    GUIMaterialSurface* autoMap;
    GUIMaterialSurface* memstatsSurface;
    GUIPanel* toolsConfigPanel;
    GUILabel* tilesetLabel;
    GUIPanel* minimapPanel;
    GUIPanel* tilesetPanel;
    GUIPanel* brushPanel;
    GUIPanel* okcancelPanel;
    GUIPanel* labPanel;
    GUIPanel* memStatsPanel;
    
    TransformGroup* natureGroup;
    TransformGroup* structureGroup;
    TransformGroup* characterGroup;
    TransformGroup* critterGroup;
    TransformGroup* villageGroup;
    TransformGroup* waterGroup;
    
    Water* currentWater;
    
    Texture minimap;
    Texture finalmap;
    Tuple4i scrcoords;
    Texture memstats;
    
    //Texture cursors[22];
    
    Grid grid;
    Axis axis;
    Ray3D ray;
    Cylinder cylinder;
    
    bool pickEnabled;
    bool mouseLocked;
    
    bool drawBase;
    bool drawTop;
    bool blendEnabled;
    bool debugView;
    
    bool drawNature;
    bool drawStructures;
    bool drawVillages;
    bool drawCharacters;
    bool drawCritters;
    bool drawPasture;
    bool drawWater;
    
    bool drawCollMark;
    bool drawLogic;
    bool drawFlags;
    bool drawGrid;
    
    bool paused;
    
    Tuple4f fogColor;
    
    float tickCounter;
    
    AVLTreeT <String, TSElementInfo, 4> elementInfoAVLTree;
    Array <unsigned int> textureHandles;
    TSElementInfo currentTileInfo;
    Texture offscreenTexture;
    Texture currentPageTexture;
    int currentIndex;
    
    CircularBuffer <float> pagefileMemoryMonitor;
    CircularBuffer <float> terrainMemoryMonitor;
    CircularBuffer <float> geometryMemoryMonitor;
};

#endif