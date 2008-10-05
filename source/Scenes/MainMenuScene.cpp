#include "MainMenuScene.h"
#include "../Renderer/Renderer.h"
#include "../Kernel/Gateway.h"
#include "../Managers/ManagersUtils.h"

MainMenuScene::MainMenuScene(const char* name) : Scene(name)
{
}

void MainMenuScene::beginScene()
{
  Scene::beginScene();
  glClearColor(0,0,0,1);
  //mouseVisible = Gateway::getConfiguration().enableCursor;
  const char* musicPath = Gateway::getMenuMusicPath();
  if (musicPath && Gateway::isMenuMusicEnabled())
    SoundManager::playSong(musicPath);
}

void MainMenuScene::endScene()
{
}

void MainMenuScene::update(const FrameInfo &frameInfo)
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

void MainMenuScene::actionPerformed(GUIEvent &evt)
{
  const String &callbackString  = evt.getCallbackString();
  GUIRectangle *sourceRectangle = evt.getEventSource();
  int           widgetType      = sourceRectangle->getWidgetType();
  
  if (widgetType == BUTTON)
  {
    GUIButton *button = (GUIButton*) sourceRectangle;
    
    if (button->isClicked())
      sceneController.execute(callbackString);
  }
}

void MainMenuScene::drawFullScreenQuad(int width, int height)
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

MainMenuScene::~MainMenuScene()
{}
