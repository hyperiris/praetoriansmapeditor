#include "NewmapScene.h"
#include "../Kernel/Gateway.h"
#include "../Image/ImageExt.h"
#include "../Tools/Timer.h"
#include "../Renderer/Renderer.h"
#include "../Managers/ManagersUtils.h"

NewmapScene::NewmapScene(const char* name) : Scene(name)
{
  currentItem = 0;
}

bool NewmapScene::initialize()
{
  Scene::initialize();
  buttonSurface = (GUIButton*) gui.getWidgetByCallbackString("PreviewSurface");
  messagePanel = (GUIPanel*) gui.getWidgetByCallbackString("PopupMessagePanel");
  messageLabel = (GUILabel*) gui.getWidgetByCallbackString("PopupMessageLabel");
  textbox = (GUITextBox*) gui.getWidgetByCallbackString("ProjectNameEditBox");
  
  loadTemplateNames();
  showMap(0);
  return true;
}

void NewmapScene::beginScene()
{
  Scene::beginScene();
  glClearColor(0,0,0,1);
  //mouseVisible = Gateway::getConfiguration().enableCursor;
}

void NewmapScene::endScene()
{
}

void NewmapScene::update(const FrameInfo &frameInfo)
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

bool NewmapScene::loadTemplateNames()
{
  XMLElement *root;
  XMLElement *child;
  XMLStack xmlStack;
  
  const char *verified = MediaPathManager::lookUpMediaPath("Plantillas.xml");
  
  if (!verified)
    return Logger::writeErrorLog("Couldn't locate the <Plantillas> XML file");
    
  if (!xmlStack.loadXMLFile(verified))
    return Logger::writeErrorLog(String("Invalid XML file -> ") + verified);
    
  if (root = xmlStack.getChildByName("Plantillas"))
    for (unsigned int i = 0; i < root->getChildrenCount(); i++)
    {
      child = root->getChild(i);
      
      MapDescriptor descriptor;
      descriptor.mapName = child->getValuec();
      descriptor.mapProjectName = descriptor.mapName;
      descriptor.camposition.set(0,10,0);
      descriptor.camfocus.set(8,0,8);
      descriptor.mapType = MapTypes::NEW_MAP;
      descriptor.loadXMLSettings(child);
      
      mapDescriptorList.append(descriptor);
    }
    
  return true;
}

void NewmapScene::actionPerformed(GUIEvent &evt)
{
  const String &callbackString  = evt.getCallbackString();
  GUIRectangle *sourceRectangle = evt.getEventSource();
  int           widgetType      = sourceRectangle->getWidgetType();
  
  if (widgetType == BUTTON)
  {
    GUIButton *button = (GUIButton*) sourceRectangle;
    
    if (button->isClicked())
    {
      if (callbackString == "PMOKButton")
      {
        messagePanel->setVisible(false);
        GUIPanel* pnl;
        pnl = (GUIPanel*) gui.getWidgetByCallbackString("NewmapPanel");
        pnl->setActive(true);
        pnl = (GUIPanel*) gui.getWidgetByCallbackString("OKCanelPanel");
        pnl->setActive(true);
        return;
      }
      
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
        handleAcceptingCallback();
        return;
      }
      
      sceneController.execute(callbackString);
    }
  }
}

bool NewmapScene::isProjectNameValid(const char* str)
{
  GUIPanel* pnl;
  
  if (!str)
  {
    messageLabel->getLabel()->setString("Missing project name");
    messagePanel->setVisible(true);
    pnl = (GUIPanel*) gui.getWidgetByCallbackString("NewmapPanel");
    pnl->setActive(false);
    pnl = (GUIPanel*) gui.getWidgetByCallbackString("OKCanelPanel");
    pnl->setActive(false);
    return false;
  }
  
  String projectname(str);
  projectname.trim();
  
  if (!projectname.getLength())
  {
    messageLabel->getLabel()->setString("Invalid project name");
    messagePanel->setVisible(true);
    pnl = (GUIPanel*) gui.getWidgetByCallbackString("NewmapPanel");
    pnl->setActive(false);
    pnl = (GUIPanel*) gui.getWidgetByCallbackString("OKCanelPanel");
    pnl->setActive(false);
    return false;
  }
  
  //if (projectname.containsDelimiters("\t\v!@#$%^&*()_-+={[}]:;\"'<,>.?/|\\~`"))
  if (projectname.containsDelimiters("\t\v@$%^&;\"<>?/\\"))
  {
    messageLabel->getLabel()->setString("Project name contains invalid characters");
    messagePanel->setVisible(true);
    pnl = (GUIPanel*) gui.getWidgetByCallbackString("NewmapPanel");
    pnl->setActive(false);
    pnl = (GUIPanel*) gui.getWidgetByCallbackString("OKCanelPanel");
    pnl->setActive(false);
    return false;
  }
  
  return true;
}

void NewmapScene::showMap(unsigned int index)
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
    
  if (textbox)
    textbox->setLabelString(mapDescriptorList(index).mapProjectName);
    
  gui.forceUpdate(true);
}

void NewmapScene::drawLoadingScreen(int randomNumber)
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

void NewmapScene::drawFullScreenQuad(int width, int height)
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

void NewmapScene::handleAcceptingCallback()
{
  if (textbox)
  {
    if (isProjectNameValid(textbox->getLabelString()))
    {
      if (!mapDescriptorList.isEmpty())
      {
        SoundManager::stopSong();
        
        int randomNum = clamp(int(getNextRandom() * 8.0f), 1, 8);
        drawLoadingScreen(randomNum);
        
        float loadLength, minLoadTime = 5.0f;
        __int64 start = Timer::getCurrentTime();
        
        MapDescriptor descriptor = mapDescriptorList(currentItem);
        descriptor.mapProjectName = textbox->getLabelString();
        descriptor.mapPastureType = ((GUIComboBox*)gui.getWidgetByCallbackString("PastureTypeList"))->getSelectedItem();
        if (Gateway::constructMap(descriptor))
          sceneController.execute("AcceptButton");
          
        loadLength = Timer::getElapsedTimeSeconds(start);
        if (loadLength < minLoadTime)
        {
          start = Timer::getCurrentTime();
          while (Timer::getElapsedTimeSeconds(start) < (minLoadTime - loadLength))
            {}
        }
      }
    }
  }
}

void NewmapScene::handleKeyEvent(KeyEvent evt, int extraInfo)
{
  Scene::handleKeyEvent(evt, extraInfo);
  
  int keyID = evt.getKeyID();
  
  switch (extraInfo)
  {
    case KEY_RELEASED:
      switch (keyID)
      {
        case KEY_ENTER:
          handleAcceptingCallback();
          break;
      }
  }
}

NewmapScene::~NewmapScene()
{}