#include "VillageLabScene.h"
//#include "../Databases/ModelDatabase.h"
#include "../Databases/VillageDatabase.h"
#include "../Nodes/TransformGroup.h"
#include "../Geometry/Geometry.h"
#include "../Kernel/Gateway.h"
#include "../Geometry/Model.h"
#include "../Managers/ManagersUtils.h"

VillageLabScene::VillageLabScene(const char* name) : Scene(name)
{
  angle = 0.0f;
  resolution = 128;
  startIndex = 0;
  endIndex = 0;
  currentGroup = 0;
  rootGroup = 0;
  camDistance = 8.0f;
  drawBounds = false;
  drawAxis = false;
  capacity = 0;
}

bool VillageLabScene::initialize()
{
  Scene::initialize();
  
  glDisable(GL_CULL_FACE);
  
  rootGroup = new TransformGroup();
  
  offscreenTexture.create2DShell("VillageOffscreenTexture", resolution, resolution, GL_RGBA8, GL_RGBA, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
  currentInfo.texture.create2DShell("VillageCurrentTexture", 256, 256, GL_RGBA8, GL_RGBA, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
  
  GUIButton* btn;
  GUIPanel* btnPanel = (GUIPanel*) gui.getWidgetByCallbackString("GeosetPanel");
  
  if (btnPanel)
    for (int i = 0; i < 14; i++)
    {
      GSElementInfo info;
      String btnName(String("GSBTN_") + i);
      
      buttonTextures[i].create2DShell(String("VL") + btnName, resolution, resolution, GL_RGB8, GL_RGB, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
      
      btn = (GUIButton*) gui.getWidgetByCallbackString(btnName);
      btn->setTexture(buttonTextures[i]);
      
      info.index = i;
      info.texture = buttonTextures[i];
      elementInfoAVLTree.insertKeyAndValue(btnName, info);
    }
    
    
  GSElementInfo elementInfo;
  const Array <ICString>* namelist = Gateway::getVillageDatabase()->getNamesList();
  for (size_t i = 0; i < namelist->length(); i++)
  {
    elementInfo.index = i;
    elementInfo.name = namelist->at(i).c_str();
    elementInfoList.append(elementInfo);
    
    if (i < 14)
    {
      drawFullScreenImage(elementInfo);
      offscreenTexture.copyCurrentBuffer();
      updateOffscreenTexture(&buttonTextures[i]);
      endIndex++;
      ++capacity;
    }
  }
  
  if (currentSetLabel = (GUILabel*) gui.getWidgetByCallbackString("SetCounter"))
    currentSetLabel->setLabelString(String("Villages ") + endIndex + String("/") + int(elementInfoList.length()));
    
  modelNameLabel = (GUILabel*) gui.getWidgetByCallbackString("ModelNameLabel");
  modelTriLabel = (GUILabel*) gui.getWidgetByCallbackString("ModelTriLabel");
  modelWidthLabel = (GUILabel*) gui.getWidgetByCallbackString("ModelWidthLabel");
  modelHeightLabel = (GUILabel*) gui.getWidgetByCallbackString("ModelHeightLabel");
  modelPropsPanel = (GUIPanel*) gui.getWidgetByCallbackString("ModelPropsPanel");
  
  grid.setOffsetY(-0.01f);
  grid.loadTexture("seamless_texture_08.jpg");
  grid.setup(64);
  grid.applyGaussianDistribution(1.0f, 6.0f);
  
  return true;
}

void VillageLabScene::beginScene()
{
  Scene::beginScene();
  glClearColor(0,0,0,1);
  //mouseVisible = Gateway::getConfiguration().enableCursor;
  
  if (modelPropsPanel)
    modelPropsPanel->setVisible(false);
    
  GUICheckBox* checkBox;
  if (checkBox = (GUICheckBox*) gui.getWidgetByCallbackString("BoundsCheckbox"))
    checkBox->setChecked(false);
  if (checkBox = (GUICheckBox*) gui.getWidgetByCallbackString("AxisCheckbox"))
    checkBox->setChecked(false);
  if (checkBox = (GUICheckBox*) gui.getWidgetByCallbackString("PropsCheckbox"))
    checkBox->setChecked(false);
    
  drawBounds = false;
  drawAxis = false;
}

void VillageLabScene::endScene()
{
}

void VillageLabScene::actionPerformed(GUIEvent &evt)
{
  const String &callbackString  = evt.getCallbackString();
  GUIRectangle *sourceRectangle = evt.getEventSource();
  int           widgetType      = sourceRectangle->getWidgetType();
  
  if (widgetType == CHECK_BOX)
  {
    GUICheckBox *checkBox = (GUICheckBox*) sourceRectangle;
    
    if (checkBox->isClicked())
    {
      if (callbackString == "BoundsCheckbox")
      {
        drawBounds = checkBox->isChecked();
        return;
      }
      
      if (callbackString == "AxisCheckbox")
      {
        drawAxis = checkBox->isChecked();
        return;
      }
      
      if (callbackString == "PropsCheckbox")
      {
        modelPropsPanel->setVisible(checkBox->isChecked());
        return;
      }
    }
  }
  
  if (widgetType == BUTTON)
  {
    GUIButton *button = (GUIButton*) sourceRectangle;
    
    if (button->isClicked())
    {
      if (callbackString == "Previous")
      {
        previousGroup();
        return;
      }
      
      if (callbackString == "Next")
      {
        nextGroup();
        return;
      }
      
      if (callbackString == "Start")
      {
        firstGroup();
        return;
      }
      
      if (callbackString == "End")
      {
        lastGroup();
        return;
      }
      
      if (callbackString == "AcceptButton")
      {
        if (TransformGroup* group = rootGroup->getGroup(currentInfo.name))
        {
          TransformGroup* rgroup = new TransformGroup();
          rgroup->addChild(new TransformGroup(*group));
          
          if (GUITextBox* textbox = (GUITextBox*) gui.getWidgetByCallbackString("PopulationTextbox"))
            if (const char* str = textbox->getLabelString())
            {
              String value(str);
              value.trim();
              if (!value.isBlank())
                if (!value.containsDelimiters("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ\t\v@$%^&;\"<>?/\\"))
                  currentInfo.population = int(atoi(value));
            }
          
          if (GUITextBox* textbox = (GUITextBox*) gui.getWidgetByCallbackString("MaxPopulationTextbox"))
            if (const char* str = textbox->getLabelString())
            {
              String value(str);
              value.trim();
              if (!value.isBlank())
                if (!value.containsDelimiters("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ\t\v@$%^&;\"<>?/\\"))
                  currentInfo.maxpopulation = int(atoi(value));
            }
          
          Gateway::setActiveVillageElement(currentInfo);
          Gateway::setActiveVillage(rgroup);
          sceneController.execute(callbackString);
        }
        return;
      }
      
      GSElementInfo* info = getGSElementInfo(callbackString);
      if (info)
      {
        TransformGroup* activeGroup = rootGroup->getGroup(info->name);
        if (activeGroup)
        {
          if (currentGroup)
            currentGroup->setVisible(false);
          activeGroup->setVisible(true);
          
          Tuple3f groupExtents = activeGroup->getBoundsDescriptor().getExtents();
          grid.applyGaussianDistribution(1.0f, groupExtents.getLengthSquared());
          
          currentGroup = activeGroup;
          camDistance = groupExtents.getLength() * 1.2f;
          
          if (modelNameLabel)
            modelNameLabel->setLabelString(String("Name: ") + activeGroup->getName());
            
          if (modelTriLabel)
            modelTriLabel->setLabelString(String("Triangles: ") + int(activeGroup->getTriangleCount()));
            
          if (modelWidthLabel)
            modelWidthLabel->setLabelString(String("Width: ") + groupExtents.x * 2);
            
          if (modelHeightLabel)
            modelHeightLabel->setLabelString(String("Height: ") + groupExtents.y * 2);
            
          currentInfo.activeGroup = activeGroup;
        }
        else
          camDistance = 8.0f;
      }
      
      sceneController.execute(callbackString);
    }
  }
}

void VillageLabScene::update(const FrameInfo &frameInfo)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  const FrameInfo* info = &frameInfo;
  
  orbitCamera(info->m_Interval);
  camera.update(info->m_Interval);
  frustum.update();
  
  grid.draw();
  if (drawAxis)
    axis.draw();
    
  rootGroup->render(FRONT_TO_BACK|SORTED_BY_TEXTURE|ALL_EFFECTS);
  if (currentGroup && drawBounds)
  {
    glColor3f(1,1,1);
    currentGroup->renderAABB();
  }
  
  Renderer::enter2DMode(info->m_Width, info->m_Height);
  gui.render(info->m_Interval);
  Renderer::exit2DMode();
}

void VillageLabScene::reset()
{
  camera.setViewerPosition(Tuple3f(8, 12, 8));
  camera.setFocusPosition(Tuple3f(0, 0, 0));
  camera.setUpDirection(Tuple3f(0, 1, 0));
  camera.setTranslationSpeed(0.03f);
  camera.update(0);
  frustum.update();
}

void VillageLabScene::orbitCamera(float tick)
{
  angle += 0.5f * tick;
  
  if (angle > 360.0f)
    angle = angle - 360.0f;
    
  float theta = fastSin(angle);
  camera.setViewerPosition(Tuple3f(camDistance * fastCos(angle), 0.7f * (camDistance + camDistance * 0.45f * theta), camDistance * theta));
}

void VillageLabScene::handleKeyEvent(KeyEvent evt, int extraInfo)
{
  Scene::handleKeyEvent(evt, extraInfo);
}

void VillageLabScene::previousGroup()
{
  startIndex = clamp(startIndex - capacity, 0, (int) elementInfoList.length());
  endIndex = clamp(startIndex + capacity, 0, (int) elementInfoList.length());
  
  flushUnusedGroups();
  rootGroup->destroy();
  
  updateButtonTextures(startIndex, endIndex);
  
  updateLabels();
}

void VillageLabScene::nextGroup()
{
  int i, j, end;
  startIndex = clamp(startIndex + capacity, 0, (int) elementInfoList.length());
  startIndex = (int) floor((float)startIndex / capacity) * capacity;
  endIndex = clamp(startIndex + capacity, 0, (int) elementInfoList.length());
  
  flushUnusedGroups();
  rootGroup->destroy();
  
  updateButtonTextures(startIndex, endIndex);
  
  end = startIndex + capacity;
  
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  offscreenTexture.copyCurrentBuffer();
  for (i = endIndex, j = capacity - (end - endIndex); i != end; i++, j++)
    updateOffscreenTexture(&buttonTextures[j]);
    
  updateLabels();
}

void VillageLabScene::firstGroup()
{
  int i, j, end;
  startIndex = 0;
  endIndex = clamp(startIndex + capacity, 0, (int) elementInfoList.length());
  
  flushUnusedGroups();
  rootGroup->destroy();
  
  updateButtonTextures(startIndex, endIndex);
  
  end = startIndex + capacity;
  
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  offscreenTexture.copyCurrentBuffer();
  for (i = endIndex, j = capacity - (end - endIndex); i != end; i++, j++)
    updateOffscreenTexture(&buttonTextures[j]);
    
  updateLabels();
}

void VillageLabScene::lastGroup()
{
  int i, j, end;
  startIndex = elementInfoList.length() - (elementInfoList.length() % capacity);
  endIndex = clamp(startIndex + capacity, 0, (int) elementInfoList.length());
  
  flushUnusedGroups();
  rootGroup->destroy();
  
  updateButtonTextures(startIndex, endIndex);
  
  end = startIndex + capacity;
  
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  offscreenTexture.copyCurrentBuffer();
  for (i = endIndex, j = capacity - (end - endIndex); i != end; i++, j++)
    updateOffscreenTexture(&buttonTextures[j]);
    
  updateLabels();
}

void VillageLabScene::updateLabels()
{
  if (currentSetLabel)
    currentSetLabel->setLabelString(String("Villages ") + endIndex + String("/") + int(elementInfoList.length()));
    
  if (modelNameLabel)
    modelNameLabel->setLabelString("Name: ");
    
  if (modelTriLabel)
    modelTriLabel->setLabelString(String("Triangles: "));
    
  if (modelWidthLabel)
    modelWidthLabel->setLabelString(String("Width: "));
    
  if (modelHeightLabel)
    modelHeightLabel->setLabelString(String("Height: "));
}

void VillageLabScene::flushUnusedGroups()
{
  PrototypeInfo* info;
  for (unsigned int i = 0; i < rootGroup->getGroupsCount(); i++)
  {
    info = PrototypeManager::getPrototypeInfo(rootGroup->getGroup(i)->getName());
    if (info)
      info->decreaseUserCount();
  }
  
  currentGroup = 0;
}

void VillageLabScene::drawFullScreenImage(const GSElementInfo& info)
{
  Tuple4i viewport;
  Tuple3f minp, minP, maxP;
  float xscale, yscale, maxlen;
  TransformGroup *proto = 0;
  TransformGroup *group = 0;
  PrototypeInfo* protoinfo;
  
  if (!(proto = Gateway::getVillagePrototype(info.name)))
  {
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    return;
  }
  
  protoinfo = PrototypeManager::getPrototypeInfo(proto->getName());
  protoinfo->increaseUserCount();
  
  group = new TransformGroup(*proto);
  group->setVisible(true);
  
  minp = group->getBoundsDescriptor().getMinEndAABB();
  
  Tuple3f cen = group->getBoundsDescriptor().getCenterAABB();
  group->getTransform().setTranslations(-cen.x, -minp.y, -cen.z);
  group->getTransform().rotateX(45.0f * DEG2RAD);
  
  rootGroup->addChild(group);
  rootGroup->updateBoundsDescriptor();
  
  minP = group->getBoundsDescriptor().getMinEndAABB();
  maxP = group->getBoundsDescriptor().getMaxEndAABB();
  
  xscale = (maxP.x - minP.x);
  yscale = (maxP.y - minP.y);
  maxlen = xscale > yscale ? xscale : yscale;
  
  glClearColor(1,1,1,1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(-maxlen/2, maxlen/2, minP.y, minP.y + maxlen, 1000, -1000);
  
  glGetIntegerv(GL_VIEWPORT, viewport);
  glViewport(0, 0, resolution, resolution);
  
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  
  rootGroup->render(FRONT_TO_BACK|SORTED_BY_TEXTURE|ALL_EFFECTS);
  
  group->getTransform().setIdentity();
  group->getTransform().setTranslations(-cen.x, -minp.y, -cen.z);
  group->setVisible(false);
  
  glClearColor(0,0,0,1);
  glViewport(0, 0, viewport.z, viewport.w);
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
}

void VillageLabScene::updateButtonTextures(unsigned int st, unsigned int en)
{
  for (int i = st, j = 0; i != en; i++, j++)
  {
    drawFullScreenImage(elementInfoList(i));
    offscreenTexture.copyCurrentBuffer();
    updateOffscreenTexture(&buttonTextures[j]);
  }
}

void VillageLabScene::updateOffscreenTexture(Texture *targetTexture)
{
  Tuple4i viewport;
  glGetIntegerv(GL_VIEWPORT, viewport);
  glViewport(0, 0, resolution, resolution);
  
  Renderer::enter2DMode();
  offscreenTexture.activate();
  drawFullScreenQuad(resolution, resolution);
  targetTexture->copyCurrentBuffer();
  offscreenTexture.deactivate();
  Renderer::exit2DMode();
  
  glViewport(0, 0, viewport.z, viewport.w);
}

void VillageLabScene::drawFullScreenQuad(int width, int height)
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

GSElementInfo* VillageLabScene::getGSElementInfo(const char* name)
{
  unsigned int index = elementInfoList.length();
  String key;
  
  if (elementInfoAVLTree.find(name, key))
  {
    index = (int) floor((float)startIndex/capacity) * capacity + elementInfoAVLTree[key].index;
    currentInfo.coords  = elementInfoAVLTree[key].coords;
    currentInfo.texture = elementInfoAVLTree[key].texture;
    currentInfo.index = index < elementInfoList.length() ? index : currentInfo.index;
    currentInfo.name = index < elementInfoList.length() ? elementInfoList(currentInfo.index).name : "";
    currentInfo.activeGroup = currentGroup;
  }
  
  return index < elementInfoList.length() ? &currentInfo : 0;
}

VillageLabScene::~VillageLabScene()
{
  deleteObject(rootGroup);
}
