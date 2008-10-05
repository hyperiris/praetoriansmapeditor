#ifndef SCENE
#define SCENE

#include "../GUI/GUIUtils.h"
#include "../Parsers/IOXMLObject.h"
#include "../Events/GUIEventListener.h"
#include "../Events/WindowEvent.h"
#include "../Events/MouseEvent.h"
#include "../Events/KeyEvent.h"
#include "../Controllers/SceneController.h"
#include "../Tools/FrameInfo.h"

class Scene : public GUIEventListener, public IOXMLObject
{
  public:
    Scene(const char* name = "DefaultScene");
    virtual ~Scene();
    
  public:
    bool loadXMLSettings(XMLElement *element);
    bool exportXMLSettings(ofstream &xmlFile);
    void actionPerformed(GUIEvent &evt);
    
  public:
    virtual bool initialize();
    virtual void destroy();
    
    virtual void update(const FrameInfo &frameInfo);
    virtual void beginScene();
    virtual void pauseScene();
    virtual void endScene();
    
    virtual void handleWindowEvent(WindowEvent evt, int extraInfo);
    virtual void handleMouseEvent(MouseEvent& evt, int extraInfo);
    virtual void handleKeyEvent(KeyEvent evt, int extraInfo);
    
    bool isInitialized();
    
    void setDefaultCursorTexture(Texture *texture);
    
    void setViewAngle(float angle);
    
  protected:
    void drawCursor();
    void setPerspective(WindowEvent evt, bool wEvt = false);
    
  protected:
    GUIFrame gui;
    bool initialized;
    bool showMouse;
    SceneController sceneController;
    Texture *cursor;
    Tuple2i cursorCoords;
    bool mouseVisible;
    float viewAngle;
};

#endif
