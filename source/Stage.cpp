#include "Stage.h"
#include "Scenes/MainMenuScene.h"
#include "Scenes/OptionsScene.h"
#include "Scenes/LoadmapScene.h"
#include "Scenes/NatureLabScene.h"
#include "Scenes/CharacterLabScene.h"
#include "Scenes/CritterLabScene.h"
#include "Scenes/StructureLabScene.h"
#include "Scenes/VillageLabScene.h"
#include "Scenes/BrushLabScene.h"
#include "Scenes/EditorScene.h"
#include "Scenes/NewmapScene.h"
#include "Managers/ManagersUtils.h"
#include "FileSystem/FileSystem.h"
#include "Databases/VillageDatabase.h"
#include "Nodes/TransformGroup.h"
#include "Kernel/Gateway.h"
#include "Image/ImageExt.h"

#include "fmod/inc/fmod.hpp"
#include "fmod/inc/fmod_errors.h"

Stage::Stage(HINSTANCE instance) : IOXMLObject("Stage")
{
  Logger::enableHTML();
  Logger::initialize();
  MediaPathManager::registerPath("ImperialStudios/");
  window.setAppInstance(instance);
  window.setInputEventListener(this);
  activeState = true;
}

bool Stage::initialize()
{
  if (!IOXMLObject::loadXMLFile(String(name) + ".xml"))
    return Logger::writeErrorLog(String("Could not initialize ") + name);
    
  srand((unsigned)time(0));
  
  Logger::writeInfoLog(String("Renderer: ") + (char*)glGetString(GL_RENDERER));
  Logger::writeInfoLog(String("Version:  ") + (char*)glGetString(GL_VERSION));
  Logger::writeInfoLog(String("Vendor:   ") + (char*)glGetString(GL_VENDOR));
  
  bool succArray[14] = {false};
  
  succArray[0] = FileSystem::initialize();
  succArray[1] = Gateway::initialize();
  succArray[2] = SoundManager::initialize();
  
  MainMenuScene* mainMenuScene = new MainMenuScene();
  OptionsScene* optionsScene = new OptionsScene();
  LoadmapScene* loadmapScene = new LoadmapScene();
  NatureLabScene* natureLabScene = new NatureLabScene();
  CharacterLabScene* characterLabScene = new CharacterLabScene();
  CritterLabScene* critterLabScene = new CritterLabScene();
  StructureLabScene* structureLabScene = new StructureLabScene();
  VillageLabScene* villageLabScene = new VillageLabScene();
  EditorScene* editorScene = new EditorScene();
  NewmapScene* newmapScene = new NewmapScene();
  BrushLabScene* brushLabScene = new BrushLabScene();
  
  editorScene->setViewAngle(45.0f);
  natureLabScene->setViewAngle(90.0f);
  characterLabScene->setViewAngle(90.0f);
  structureLabScene->setViewAngle(90.0f);
  villageLabScene->setViewAngle(90.0f);
  
  //if (window.isFullscreen())
  //{
  //  ImageExt im;
  //  im.load("FlechaMenu.tga");
  //  cursor.load2DImage(im);
  //  mainMenuScene->setDefaultCursorTexture(&cursor);
  //  optionsScene->setDefaultCursorTexture(&cursor);
  //  loadmapScene->setDefaultCursorTexture(&cursor);
  //  natureLabScene->setDefaultCursorTexture(&cursor);
  //  characterLabScene->setDefaultCursorTexture(&cursor);
  //  critterLabScene->setDefaultCursorTexture(&cursor);
  //  structureLabScene->setDefaultCursorTexture(&cursor);
  //  villageLabScene->setDefaultCursorTexture(&cursor);
  //  editorScene->setDefaultCursorTexture(&cursor);
  //  newmapScene->setDefaultCursorTexture(&cursor);
  //  ShowCursor(-1);
  //  Gateway::getConfiguration().enableCursor = false;
  //}
  
  succArray[3] = SceneManager::addScene(mainMenuScene);
  succArray[4] = SceneManager::addScene(optionsScene);
  succArray[5] = SceneManager::addScene(loadmapScene);
  succArray[6] = SceneManager::addScene(newmapScene);
  succArray[7] = SceneManager::addScene(natureLabScene);
  succArray[8] = SceneManager::addScene(characterLabScene);
  succArray[9] = SceneManager::addScene(critterLabScene);
  succArray[10] = SceneManager::addScene(structureLabScene);
  succArray[11] = SceneManager::addScene(villageLabScene);
  succArray[12] = SceneManager::addScene(brushLabScene);
  succArray[13] = SceneManager::addScene(editorScene);
  SceneManager::setStage(this);
  SceneManager::setScene(mainMenuScene);
  
  //succArray[10] = mainMenuScene->initialize();
  //succArray[11] = loadmapScene->initialize();
  //succArray[12] = newmapScene->initialize();
  //succArray[13] = optionsScene->initialize();
  
  if (!succArray[0]  || !succArray[1] || !succArray[2]  || !succArray[3] ||
      !succArray[4]  || !succArray[5] || !succArray[6]  || !succArray[7] ||
      !succArray[8]  || !succArray[9] || !succArray[10] || !succArray[11]||
      !succArray[12] || !succArray[13])
    return false;
    
  window.setVisible(true);
  
  Logger::flush();
  
  return true;
}

bool Stage::loadXMLSettings(XMLElement *element)
{
  if (!isSuitable(element))
    return false;
    
  XMLElement *child, *attrib;
  bool success = false;
  
  if (child = element->getChildByName("Export"))
    if (attrib = child->getChildByName("path"))
      Gateway::setExportPath(attrib->getValuec());
      
  if (child = element->getChildByName("MediaSource"))
    for (size_t i = 0; i < child->getChildrenCount(); i++)
      MediaPathManager::registerPath(child->getChild(i));
      
  if (child = element->getChildByName("Benchmark"))
    benchmark.loadXMLSettings(child);
    
  if (child = element->getChildByName("Window"))
    success = window.loadXMLSettings(child);
    
  if (child = element->getChildByName("MenuMusic"))
  {
    if (attrib = child->getChildByName("path"))
      Gateway::setMenuMusicPath(attrib->getValuec());
      
    if (attrib = child->getChildByName("enabled"))
      Gateway::enableMenuMusic(attrib->getValue() == "true");
  }
  
  return success;
}

bool Stage::run()
{
  float interval = benchmark.getFrameInterval();
  
  benchmark.markFrameStart();
  FrameInfo frameInfo(interval, benchmark.getFPS(), window.getWidth(), window.getHeight());
  Gateway::update();
  SceneManager::updateScene(frameInfo);
  window.update(interval);
  benchmark.markFrameEnd();
  
  activeState = benchmark.running();
  
  return activeState;
}

void Stage::setActive(bool state)
{
  activeState = state;
}

bool Stage::isActive()
{
  return activeState;
}

void Stage::forceUpdate(bool winUpdate)
{
  SceneManager::dispatchEvent(WindowEvent(0, 0, window.getWidth(), window.getHeight()), 1);
  
  if (winUpdate)
    window.update();
}

void Stage::mouseDoubleClicked(MouseEvent& evt)
{
  SceneManager::dispatchEvent(evt, DOUBLE_CLICKED);
}

void Stage::mouseScrolled(MouseEvent& evt)
{
}

void Stage::mouseReleased(MouseEvent& evt)
{
  SceneManager::dispatchEvent(evt, RELEASED);
}

void Stage::mouseClicked(MouseEvent& evt)
{
  SceneManager::dispatchEvent(evt, CLICKED);
}

void Stage::mouseMoved(MouseEvent& evt)
{
  SceneManager::dispatchEvent(evt, MOVED);
}

void Stage::mouseDragged(MouseEvent& evt)
{
  SceneManager::dispatchEvent(evt, DRAGGED);
}

void Stage::mouseExit(MouseEvent& evt)
{
  SceneManager::dispatchEvent(evt, 0);
}

void Stage::keyReleased(KeyEvent evt)
{
  SceneManager::dispatchEvent(evt, KEY_RELEASED);
  
  switch (evt.getKeyID())
  {
    case KEY_F8:
      window.scheduleSnapshot();
      break;
  }
}

void Stage::keyPressed(KeyEvent evt)
{
  SceneManager::dispatchEvent(evt, KEY_PRESSED);
}

void Stage::windowPositionChanged(WindowEvent evt)
{
  SceneManager::dispatchEvent(evt, 1);
}

void Stage::windowSizeChanged(WindowEvent evt)
{
  SceneManager::dispatchEvent(evt, 1);
}

void Stage::windowMaximized(WindowEvent evt)
{
  SceneManager::dispatchEvent(evt, 1);
}

void Stage::windowMinimized(WindowEvent evt)
{
  SceneManager::dispatchEvent(evt, 1);
}

void Stage::windowClosing(WindowEvent evt)
{
}

bool Stage::exportXMLSettings(ofstream &xmlFile)
{
  return true;
}

void Stage::destroy()
{
  if (benchmark.isEnabled())
    benchmark.exportResults();
    
  SceneManager::destroy();
  FileSystem::destroy();
  Gateway::destroy();
  TexturesManager::flushAllTextures();
  //GeometryManager::flushAllGeometries();
  PrototypeManager::flushAllPrototypes();
  SoundManager::destroy();
  
  window.destroy();
}

Stage::~Stage()
{
  destroy();
  Logger::finalize();
}
