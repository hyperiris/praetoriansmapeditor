#include "BrushLabScene.h"
#include "../Kernel/Gateway.h"
#include "../Renderer/Renderer.h"
#include "../Factories/TerrainTextures.h"
#include "../Containers/ocsort.h"

BrushLabScene::BrushLabScene(const char* name) :
    Scene(name),
    matrixTileList(16),
    matrixElementCBStrings(16)
{
}

bool BrushLabScene::initialize()
{
  Scene::initialize();
  
  tilesetPanel = (GUIPanel*) gui.getWidgetByCallbackString("TilesetPanel");
  tilesetLabel = (GUILabel*) gui.getWidgetByCallbackString("SetCounter");
  
  tilesetPanel->setVisible(true);
  tilesetPanel->setPosition(2.0f, 0.5f);
  
  offscreenTexture.create2DShell("BrushTilesetPageTexture", 256, 256, GL_RGB8, GL_RGB, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
  currentPageTexture.create2DShell("BrushTilesetCurrentTexture", 256, 256, GL_RGB8, GL_RGB, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
  
  currentTileInfo.texture = &currentPageTexture;
  
  GUIButton* tsButton;
  TerrainTextures* ttextures = Gateway::getTerrainTextures();
  if (tilesetPanel)
  {
    for (int i = 0; i < 16; i++)
    {
      TSElementInfo info;
      String btnName(String("TSBTN_") + i);
      Tuple2f* cbounds = ttextures->getTextureCoords(i);
      
      tsButton = (GUIButton*) gui.getWidgetByCallbackString(btnName);
      tsButton->setTexture(offscreenTexture);
      tsButton->setTextureRectangle(Tuple4f(cbounds[0].x, cbounds[0].y, cbounds[8].x, cbounds[8].y));
      
      info.texture = &offscreenTexture;
      info.pageIndex = 0;
      info.coordsIndex = i;
      info.coords = tsButton->getTextureRectangle();
      elementInfoAVLTree.insertKeyAndValue(btnName, info);
    }
  }
  
  unsigned int txid;
  
  textureHandles.clear();
  currentIndex = 0;
  
  for (unsigned int i = 0; i < ttextures->getTextureCount(); i++)
  {
    txid = ttextures->getTextureID(i);
    
    if (!textureHandles.contains(txid))
      textureHandles.append(txid);
      
    if (i == 0)
      updateOffscreenTexture(&offscreenTexture);
  }
  
  if (tilesetLabel)
    tilesetLabel->setLabelString(String("Tileset ") + (currentIndex + 1) + String("/") + int(textureHandles.length()));
    
  return true;
}

void BrushLabScene::beginScene()
{
  Scene::beginScene();
  glClearColor(0,0,0,1);
}

void BrushLabScene::endScene()
{
}

void BrushLabScene::update(const FrameInfo &frameInfo)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  const FrameInfo* info = &frameInfo;
  Renderer::enter2DMode(info->m_Width, info->m_Height);
  gui.render(info->m_Interval);
  Renderer::exit2DMode();
}

void BrushLabScene::actionPerformed(GUIEvent &evt)
{
  const String &callbackString  = evt.getCallbackString();
  GUIRectangle *sourceRectangle = evt.getEventSource();
  int           widgetType      = sourceRectangle->getWidgetType();
  
  //------------------------
  ///GUIButton
  //------------------------
  if (widgetType == BUTTON)
  {
    GUIButton *button = (GUIButton*) sourceRectangle;
    
    if (button->isClicked())
    {
      if (callbackString == "Start")
      {
        firstTileSet();
        return;
      }
      
      if (callbackString == "End")
      {
        lastTileSet();
        return;
      }
      
      if (callbackString == "Previous")
      {
        previousTileSet();
        return;
      }
      
      if (callbackString == "Next")
      {
        nextTileSet();
        return;
      }
      
      if (callbackString == "BrushMatrixCreateBTN")
      {
        GUIComboBox* cb = (GUIComboBox*) gui.getWidgetByCallbackString("BrushMatrixSizeCB");
        String item = cb->getSelectedItem();
        
        if (item == "2x2")
        {
          createBrushMatrix(2,2);
          return;
        }
        
        if (item == "3x3")
        {
          createBrushMatrix(3,3);
          return;
        }
        
        if (item == "4x4")
        {
          createBrushMatrix(4,4);
          return;
        }
        
        return;
      }
      
      if (callbackString == "AcceptButton")
      {
        BrushMatrixDescriptor descriptor;
        String item = ((GUIComboBox*)gui.getWidgetByCallbackString("BrushMatrixSizeCB"))->getSelectedItem();
        
        descriptor.dimensions.set((item == "2x2") ? 2 : (item == "3x3") ? 3 : (item == "4x4") ? 4 : 0);
        
        Array <Tile> temparr(matrixTileList);
        for (int i = 0; i < descriptor.dimensions.x; i++)
          OCSwap(temparr(i), temparr((descriptor.dimensions.y-1) * descriptor.dimensions.y + i));
        descriptor.tileList = temparr;
        
        Gateway::setBrushMatrixDescriptor(descriptor);
        
        sceneController.execute(callbackString);
        
        return;
      }
      
      ///is it from the tileset?
      if (TSElementInfo* info = getTSElementInfo(callbackString))
      {
        return;
      }
      
      ///is it from the brush matrix?
      size_t index = matrixElementCBStrings.index(callbackString);
      if (index != Array<String>::ARRAY_NPOS)
      {
        String item;
        Texture texture;
        Tuple4i viewport;
        Matrix4f matrix;
        GUIButton* button;
        short flags = 0;
        
        //matrixTileList(index).reset();
        matrixTileList(index).flags &= 0xff80;
        
        item = ((GUIComboBox*)gui.getWidgetByCallbackString("TileMatrixOrientationCB"))->getSelectedItem();
        matrixTileList(index).flags |= (item == "Rotate 90")  ? TileFlags::TEXTURE_ROTATE90  :
                                       (item == "Rotate 180") ? TileFlags::TEXTURE_ROTATE180 :
                                       (item == "Rotate 270") ? TileFlags::TEXTURE_ROTATE270 : 0;
                                       
        item = ((GUIComboBox*)gui.getWidgetByCallbackString("TileMatrixMirrorCB"))->getSelectedItem();
        matrixTileList(index).flags ^= (item == "Mirror X") ? TileFlags::TEXTURE_MIRRORX :
                                       (item == "Mirror Y") ? TileFlags::TEXTURE_MIRRORY : 0;
                                       
        matrixTileList(index).textureID[0] = currentTileInfo.pageIndex * 16 + currentTileInfo.coordsIndex;
        flags = matrixTileList(index).flags;
        
        button = (GUIButton*) gui.getWidgetByCallbackString(callbackString);
        texture.create2DShell(callbackString, 256, 256, GL_RGB8, GL_RGB, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
        
        TerrainTextures* ttextures = Gateway::getTerrainTextures();
        Tuple2f* pCoords = ttextures->getTextureCoords(currentTileInfo.coordsIndex);
        
        glGetIntegerv(GL_VIEWPORT, viewport);
        glViewport(0, 0, 256, 256);
        Renderer::enter2DMode(256, 256);
        
        glMatrixMode(GL_TEXTURE);
        glPushMatrix();
        glLoadIdentity();
        
        if (flags & TileFlags::TEXTURE_MIRRORX)
        {
          glTranslatef(pCoords[4].x, pCoords[4].y, 0);
          glScalef(-1, 1, 1);
          glTranslatef(-pCoords[4].x, -pCoords[4].y, 0);
        }
        
        if (flags & TileFlags::TEXTURE_MIRRORY)
        {
          glTranslatef(pCoords[4].x, pCoords[4].y, 0);
          glScalef(1, -1, 1);
          glTranslatef(-pCoords[4].x, -pCoords[4].y, 0);
        }
        
        if (flags & TileFlags::TEXTURE_ROTATE90)
        {
          glTranslatef(pCoords[4].x, pCoords[4].y, 0);
          glRotatef(90.0f, 0, 0, 1);
          glTranslatef(-pCoords[4].x, -pCoords[4].y, 0);
        }
        else if (flags & TileFlags::TEXTURE_ROTATE180)
        {
          glTranslatef(pCoords[4].x, pCoords[4].y, 0);
          glRotatef(180.0f, 0, 0, 1);
          glTranslatef(-pCoords[4].x, -pCoords[4].y, 0);
        }
        else if (flags & TileFlags::TEXTURE_ROTATE270)
        {
          glTranslatef(pCoords[4].x, pCoords[4].y, 0);
          glRotatef(270.0f, 0, 0, 1);
          glTranslatef(-pCoords[4].x, -pCoords[4].y, 0);
        }
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        currentTileInfo.texture->activate();
        glColor3f(1,1,1);
        drawFullScreenQuadInverse(256, 256);
        texture.copyCurrentBuffer();
        currentTileInfo.texture->deactivate();
        
        glMatrixMode(GL_TEXTURE);
        glPopMatrix();
        
        Renderer::exit2DMode();
        glViewport(0, 0, viewport.z, viewport.w);
        
        button->setTexture(texture);
        button->setTextureRectangle(currentTileInfo.coords);
        
        gui.forceUpdate(true);
        
        return;
      }
      
      sceneController.execute(callbackString);
    }
  }
}

void BrushLabScene::createBrushMatrix(unsigned int width, unsigned int height)
{
  GUIPanel* mainpanel = (GUIPanel*) gui.getWidgetByCallbackString("TileMatrixPanel");
  mainpanel->clear();
  mainpanel->setParent(((GUIPanel*)gui.getWidgetByCallbackString("TileMatrixParentPanel")));
  
  matrixElementCBStrings.clearFast();
  matrixTileList.clearFast();
  
  for (unsigned int y = 0; y < height; y++)
  {
    GUIPanel* panel = new GUIPanel(String("BrushMatrixRow") + (int)y);
    panel->setInterval(0, 0);
    panel->setLayout(XAXIS_LAYOUT);
    
    for (unsigned int x = 0; x < width; x++)
    {
      GUIButton* button = new GUIButton(String("BrushMatrixButton") + int(y * width + x));
      button->setColor(1.0f, 1.0f, 1.0f);
      button->setDimensions(64.0f, 64.0f);
      //button->setTextureRectangle(0.0f, 0.0f, 0.25f, 0.25f);
      button->setMinAlpha(1.0f);
      button->setClipSize(0);
      button->enableBounce(false);
      button->setBordersColor(58.0f/255.0f, 48.0f/255.0f, 0.0f);
      button->setVisibleBounds(true);
      panel->addWidget(button);
      
      matrixElementCBStrings.append(button->getCallbackString());
      matrixTileList.append(Tile());
    }
    
    mainpanel->addWidget(panel);
  }
  
  gui.forceUpdate(true);
}

void BrushLabScene::firstTileSet()
{
  currentIndex = 0;
  updateOffscreenTexture(&offscreenTexture);
  
  if (tilesetLabel)
    tilesetLabel->setLabelString(String("Tileset ") + (currentIndex + 1) + String("/") + int(textureHandles.length()));
}

void BrushLabScene::lastTileSet()
{
  currentIndex = textureHandles.length() - 1;
  updateOffscreenTexture(&offscreenTexture);
  
  if (tilesetLabel)
    tilesetLabel->setLabelString(String("Tileset ") + (currentIndex + 1) + String("/") + int(textureHandles.length()));
}

void BrushLabScene::previousTileSet()
{
  currentIndex = clamp(--currentIndex, 0, (int)textureHandles.length() - 1);
  updateOffscreenTexture(&offscreenTexture);
  
  if (tilesetLabel)
    tilesetLabel->setLabelString(String("Tileset ") + (currentIndex + 1) + String("/") + int(textureHandles.length()));
}

void BrushLabScene::nextTileSet()
{
  currentIndex = clamp(++currentIndex, 0, (int)textureHandles.length() - 1);
  updateOffscreenTexture(&offscreenTexture);
  
  if (tilesetLabel)
    tilesetLabel->setLabelString(String("Tileset ") + (currentIndex + 1) + String("/") + int(textureHandles.length()));
}

void BrushLabScene::updateOffscreenTexture(Texture* targetTexture)
{
  Tuple4i viewport;
  
  glGetIntegerv(GL_VIEWPORT, viewport);
  glViewport(0, 0, 256, 256);
  Renderer::enter2DMode(256, 256);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, textureHandles(currentIndex));
  glColor3f(1,1,1);
  drawFullScreenQuadInverse(256, 256);
  targetTexture->copyCurrentBuffer();
  glDisable(GL_TEXTURE_2D);
  Renderer::exit2DMode();
  glViewport(0, 0, viewport.z, viewport.w);
}

TSElementInfo* BrushLabScene::getTSElementInfo(const char* name)
{
  String key;
  
  if (elementInfoAVLTree.find(name, key))
  {
    currentTileInfo.pageIndex = currentIndex;
    currentTileInfo.coords = elementInfoAVLTree[key].coords;
    currentTileInfo.coordsIndex = elementInfoAVLTree[key].coordsIndex;
    updateOffscreenTexture(currentTileInfo.texture);
    return &currentTileInfo;
  }
  
  return 0;
}

void BrushLabScene::drawFullScreenQuadInverse(int width, int height)
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

BrushLabScene::~BrushLabScene()
{
}
