#ifndef SCENEMANAGER
#define SCENEMANAGER

#include "../Events/WindowEvent.h"
#include "../Events/MouseEvent.h"
#include "../Events/KeyEvent.h"
#include "../Tools/FrameInfo.h"
#include "../Containers/ocavltree.h"

class Scene;
class Stage;

class SceneInfo
{
  public:
    SceneInfo(Scene *s = 0);
    SceneInfo(const SceneInfo &copy);
    SceneInfo &operator = (const SceneInfo &right);
    Scene* getScenePointer();
    
  private:
    Scene *scene;
};

class SceneManager
{
  public:
    static void       updateStage(bool winUpdate = false);
    static void       setStage(Stage *stage);
    static void       deactivateCurrentStage();
    
    static void       destroy();
    
    static bool       addScene(Scene *scene);
    static void       setScene(const char* name);
    static void       setScene(Scene* scene);
    static void       switchScene(const char* name);
    static void       transposeScene();
    static void       setTransposeScene(const char* name);
    static void       setTransposeScene(Scene* scene);
    static void       changeScene(const char* name);
    static void       updateScene(const FrameInfo &frameInfo);
    static SceneInfo* getSceneInfo(const char* name);
    
    static void       dispatchEvent(KeyEvent evt, int info);
    static void       dispatchEvent(MouseEvent& evt, int info);
    static void       dispatchEvent(WindowEvent evt, int info);
    
  private:
    static Scene *currentScene;
    static Scene *swapScene;
    static Stage *currentStage;
    static AVLTree <SceneInfo> infoList;
};

#endif
