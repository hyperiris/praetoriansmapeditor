#include "LoadmapScene.h"
#include "../Kernel/Gateway.h"
#include "../Image/ImageExt.h"
#include "../Tools/Timer.h"
#include "../Renderer/Renderer.h"
#include "../Managers/ManagersUtils.h"

LoadmapScene::LoadmapScene(const char* name) : Scene(name)
{
  currentItem = 0;
}

bool LoadmapScene::initialize()
{
  Scene::initialize();
  buttonSurface = (GUIButton*) gui.getWidgetByCallbackString("PreviewSurface");
  loadTemplateNames();
  showMap(0);
  return true;
}

void LoadmapScene::beginScene()
{
  Scene::beginScene();
  glClearColor(0,0,0,1);
  //mouseVisible = Gateway::getConfiguration().enableCursor;
}

void LoadmapScene::endScene()
{
}

void LoadmapScene::update(const FrameInfo &frameInfo)
{
  const FrameInfo* info = &frameInfo;
  
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  Renderer::enter2DMode(info->m_Width, info->m_Height);
  glEnable(GL_TEXTURE_2D);
  Gateway::bindBackground();
  glColor3f(1,1,1);
  drawFullScreenQuad(info->m_Width, info->m_Height);
  glDisable(GL_TEXTURE_2D);
  gui.render(info->m_Interval);
  drawCursor();
  Renderer::exit2DMode();
}

bool LoadmapScene::loadTemplateNames()
{
  XMLElement *root;
  XMLElement *child;
  XMLStack xmlStack;
  
  const char *verified = MediaPathManager::lookUpMediaPath("Mapas.xml");
  
  if (!verified)
    return Logger::writeErrorLog("Could not locate the <Mapas> XML file");
    
  if (!xmlStack.loadXMLFile(verified))
    return Logger::writeErrorLog(String("Invalid XML file -> ") + verified);
    
  if (root = xmlStack.getChildByName("Mapas"))
    for (unsigned int i = 0; i < root->getChildrenCount(); i++)
    {
      child = root->getChild(i);
      
      MapDescriptor descriptor;
      descriptor.mapName = child->getValuec();
      descriptor.camposition.set(0,10,0);
      descriptor.camfocus.set(8,0,8);
      descriptor.mapType = MapTypes::LOAD_MAP;
      descriptor.loadXMLSettings(child);
      
      mapDescriptorList.append(descriptor);
    }
    
  return true;
}

void LoadmapScene::actionPerformed(GUIEvent &evt)
{
  const String &callbackString  = evt.getCallbackString();
  GUIRectangle *sourceRectangle = evt.getEventSource();
  int           widgetType      = sourceRectangle->getWidgetType();
  
  if (widgetType == BUTTON)
  {
    GUIButton *button = (GUIButton*) sourceRectangle;
    
    if (button->isClicked())
    {
      if (callbackString == "PreviewMapLeft")
      {
        currentItem = clamp(--currentItem, 0, (int)mapDescriptorList.length() - 1);
        showMap(currentItem);
        return;
      }
      
      if (callbackString == "PreviewMapRight")
      {
        currentItem = clamp(++currentItem, 0, (int)mapDescriptorList.length() - 1);
        showMap(currentItem);
        return;
      }
      
      if (callbackString == "AcceptButton")
      {
        if (!mapDescriptorList.isEmpty())
        {
          SoundManager::stopSong();
          
          int randomNum = clamp(int(getNextRandom() * 8.0f), 1, 8);
          drawLoadingScreen(randomNum);
          
          float loadLength, minLoadTime = 5.0f;
          __int64 start = Timer::getCurrentTime();
          
          if (Gateway::constructMap(mapDescriptorList(currentItem)))
            sceneController.execute(callbackString);
            
          loadLength = Timer::getElapsedTimeSeconds(start);
          if (loadLength < minLoadTime)
          {
            start = Timer::getCurrentTime();
            while (Timer::getElapsedTimeSeconds(start) < (minLoadTime - loadLength))
              {}
          }
        }
        
        return;
      }
      
      sceneController.execute(callbackString);
    }
  }
}

void LoadmapScene::showMap(unsigned int index)
{
  GUILabel* label;
  ImageExt image;
  Texture texture;
  
  label = (GUILabel*) gui.getWidgetByCallbackString("PreviewMapName");
  if (label)
    label->setLabelString(mapDescriptorList(index).mapName);
    
  label = (GUILabel*) gui.getWidgetByCallbackString("MapSize");
  Tuple2i area = mapDescriptorList(index).mapDimensions;
  if (label)
    label->setLabelString(String("Size: ") + area.x + String("x") + area.y);
    
  label = (GUILabel*) gui.getWidgetByCallbackString("MapMode");
  if (label)
    label->setLabelString(String("Mode: ") + mapDescriptorList(index).mapMode);
    
  image.load(mapDescriptorList(index).mapMinimap);
  texture.load2DImage(image);
  
  if (buttonSurface)
    buttonSurface->setTexture(texture);
    
  gui.forceUpdate(true);
}

void LoadmapScene::drawLoadingScreen(int randomNumber)
{
  ImageExt image;
  Texture screentexture;
  Tuple4i viewport;
  String texPath = String("carga_0") + randomNumber + String(".dds");
  
  image.load(texPath);
  screentexture.load2DImage(image);
  glGetIntegerv(GL_VIEWPORT, viewport);
  
  Renderer::enter2DMode(viewport.z, viewport.w);
  screentexture.activate();
  glColor3f(1,1,1);
  drawFullScreenQuad(viewport.z, viewport.w);
  screentexture.deactivate();
  Renderer::exit2DMode();
  
  SceneManager::updateStage(true);
}

void LoadmapScene::drawFullScreenQuad(int width, int height)
{
  glBegin(GL_TRIANGLE_STRIP);
  glTexCoord2f(0.0f, 1.0f);
  glVertex2i(0, 0);
  glTexCoord2f(0.0f, 0.0f);
  glVertex2i(0, height);
  glTexCoord2f(1.0f, 1.0f);
  glVertex2i(width, 0);
  glTexCoord2f(1.0f, 0.0f);
  glVertex2i(width, height);
  glEnd();
}

LoadmapScene::~LoadmapScene()
{}