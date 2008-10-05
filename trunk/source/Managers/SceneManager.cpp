#include "SceneManager.h"
#include "../Stage.h"
#include "../Scenes/Scene.h"

AVLTree <SceneInfo> SceneManager::infoList;
Scene* SceneManager::currentScene  = 0;
Stage* SceneManager::currentStage  = 0;
Scene* SceneManager::swapScene     = 0;

/**
 * SceneInfo
 */
SceneInfo::SceneInfo(Scene *s) : scene(s)
{}

SceneInfo::SceneInfo(const SceneInfo &copy)
{
  this->operator = (copy);
}

SceneInfo &SceneInfo::operator = (const SceneInfo & right)
{
  if (this != &right)
    scene = right.scene;
    
  return *this;
}

Scene* SceneInfo::getScenePointer()
{
  return scene;
}

/**
 * SceneManager
 */
void SceneManager::updateStage(bool winUpdate)
{
  currentStage->forceUpdate(winUpdate);
}

void SceneManager::setStage(Stage *stage)
{
  currentStage = stage;
}

void SceneManager::deactivateCurrentStage()
{
  currentStage->setActive(false);
}

void SceneManager::setScene(const char* name)
{
  SceneInfo* info = SceneManager::getSceneInfo(name);
  
  if (!info)
    return;
    
  currentScene = info->getScenePointer();
  currentScene->beginScene();
}

void SceneManager::setScene(Scene* scene)
{
  SceneInfo* info = SceneManager::getSceneInfo(scene->getName());
  
  if (!info)
    return;
    
  currentScene = scene;
  currentScene->beginScene();
}

void SceneManager::switchScene(const char* name)
{
  currentScene->pauseScene();
  setScene(name);
  currentStage->forceUpdate();
}

void SceneManager::transposeScene()
{
  if (!swapScene)
    return;
    
  Scene* prev = currentScene;
  currentScene->pauseScene();
  setScene(swapScene);
  setTransposeScene(prev);
  currentStage->forceUpdate();
}

void SceneManager::setTransposeScene(const char* name)
{
  SceneInfo* info = SceneManager::getSceneInfo(name);
  if (!info)
    return;
    
  swapScene = info->getScenePointer();
}

void SceneManager::setTransposeScene(Scene* scene)
{
  SceneInfo* info = SceneManager::getSceneInfo(scene->getName());
  
  if (!info)
    return;
    
  swapScene = scene;
}

void SceneManager::changeScene(const char* name)
{
  currentScene->endScene();
  setScene(name);
  currentStage->forceUpdate();
}

void SceneManager::updateScene(const FrameInfo &frameInfo)
{
  currentScene->update(frameInfo);
}

bool SceneManager::addScene(Scene *scene)
{
  if (!scene)
    return false;
    
  string key(scene->getName());
  
  if (infoList.contains(key))
    return false;
    
  infoList.insertKeyAndValue(key, SceneInfo(scene));
  
  return true;
}

SceneInfo* SceneManager::getSceneInfo(const char* name)
{
  if (!infoList.contains(name))
    return 0;
    
  return &infoList[name];
}

void SceneManager::dispatchEvent(KeyEvent evt, int info)
{
  if (!currentScene)
    return;
    
  currentScene->handleKeyEvent(evt, info);
}

void SceneManager::dispatchEvent(MouseEvent& evt, int info)
{
  if (!currentScene)
    return;
    
  currentScene->handleMouseEvent(evt, info);
}

void SceneManager::dispatchEvent(WindowEvent evt, int info)
{
  if (!currentScene)
    return;
    
  currentScene->handleWindowEvent(evt, info);
}

void SceneManager::destroy()
{
  AVLTreeIterator <SceneInfo> iter(infoList);
  
  while (iter.next())
    delete iter.value().getScenePointer();
    
  currentScene = 0;
  
  infoList.clear();
}
