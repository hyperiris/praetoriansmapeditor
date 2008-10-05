#include "EditorScene.h"
#include "../Kernel/Gateway.h"
#include "../Geometry/Geometry.h"
#include "../Nodes/TransformGroup.h"
#include "../Managers/ManagersUtils.h"
#include "../Memory/PageFile.h"

#include "../Visitors/TileGraphRenderer.h"
#include "../Visitors/TileRevealer.h"
#include "../Visitors/BoundsViewer.h"
#include "../Visitors/DebugViewer.h"
#include "../Visitors/LogicViewer.h"
#include "../Visitors/FlagViewer.h"
#include "../Visitors/TerrainBrush.h"

#include "../Factories/WorldVisuals.h"
#include "../Factories/TerrainVisuals.h"
#include "../Factories/TerrainTextures.h"
#include "../Factories/TerrainPasture.h"
#include "../Factories/TerrainWater.h"
#include "../Factories/Meadow.h"
#include "../Factories/Water.h"

#include "../Databases/TerrainDatabase.h"
#include "../Databases/ModelDatabase.h"
#include "../Databases/DatabaseResources.h"
#include "../Databases/WaterDatabase.h"

TerrainBrush brush;
TileGraphRenderer renderer;
BoundsViewer boundsViewer;
TileRevealer tileRevealer;
DebugViewer debugViewer;
FlagViewer flagViewer;
LogicViewer logicViewer;

int grassIDX = 1;

EditorScene::EditorScene(const char* name) :
    Scene(name),
    terrainMemoryMonitor(60,false),
    geometryMemoryMonitor(60,false),
    pagefileMemoryMonitor(60,false)
{
  resetFlags();
  natureGroup = 0;
  structureGroup = 0;
  characterGroup = 0;
  critterGroup = 0;
  villageGroup = 0;
  waterGroup = 0;
  
  currentWater = 0;
  
  tickCounter = 0.0f;
}

bool EditorScene::initialize()
{
  Scene::initialize();
  
  natureGroup    = Gateway::getNatureDatabase()->getRootGroup();
  structureGroup = Gateway::getStructureDatabase()->getRootGroup();
  characterGroup = Gateway::getCharacterDatabase()->getRootGroup();
  critterGroup   = Gateway::getCritterDatabase()->getRootGroup();
  villageGroup   = Gateway::getVillageDatabase()->getRootGroup();
  waterGroup    = Gateway::getWaterDatabase()->getRootGroup();
  
  minimapPanel      = (GUIPanel*) gui.getWidgetByCallbackString("Minimap");
  fpsCounter        = (GUILabel*) gui.getWidgetByCallbackString("FpsCounter");
  //brushPanel        = (GUIPanel*) gui.getWidgetByCallbackString("BrushPanel");
  tilesetLabel      = (GUILabel*) gui.getWidgetByCallbackString("SetCounter");
  tilesetPanel      = (GUIPanel*) gui.getWidgetByCallbackString("TilesetPanel");
  okcancelPanel     = (GUIPanel*) gui.getWidgetByCallbackString("EditorToolbar");
  labPanel          = (GUIPanel*) gui.getWidgetByCallbackString("LaboratoryPanel");
  toolsConfigPanel  = (GUIPanel*) gui.getWidgetByCallbackString("ToolsConfigPanel");
  fogDensity        = (GUISlider*) gui.getWidgetByCallbackString("Density");
  autoMap           = (GUIMaterialSurface*) gui.getWidgetByCallbackString("AutoMap");
  memStatsPanel     = (GUIPanel*) gui.getWidgetByCallbackString("MemoryStatsPanel");
  memstatsSurface   = (GUIMaterialSurface*) gui.getWidgetByCallbackString("MemStatsWindow");
  
  const Tuple2f& dim = memstatsSurface->getDimensions();
  memstats.create2DShell("memstats", (int)dim.x, (int)dim.y, GL_RGBA8, GL_RGBA, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
  offscreenTexture.create2DShell("TilesetPageTexture", 256, 256, GL_RGB8, GL_RGB, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
  currentPageTexture.create2DShell("TilesetCurrentTexture", 256, 256, GL_RGB8, GL_RGB, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
  minimap.create2DShell("minimap", 256, 256, GL_RGBA8, GL_RGBA, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
  finalmap.create2DShell("finalmap", 256, 256, GL_RGBA8, GL_RGBA, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
  
  if (memstatsSurface)
    memstatsSurface->getAppearance()->setTexture(0, memstats);
    
  if (autoMap)
    autoMap->getAppearance()->setTexture(0, finalmap);
    
  currentTileInfo.texture = &currentPageTexture;
  
  GUIButton* tsButton;
  TerrainTextures* ttextures = Gateway::getTerrainTextures();
  if (tilesetPanel)
    for (int i = 0; i < 16; i++)
    {
      TSElementInfo info;
      String btnName(String("TSBTN_") + i);
      Tuple2f* cbounds = ttextures->getTextureCoords(i);
      
      tsButton = (GUIButton*) gui.getWidgetByCallbackString(btnName);
      tsButton->setTexture(offscreenTexture);
      
      info.texture = &offscreenTexture;
      info.pageIndex = 0;
      info.coordsIndex = i;
      info.coords = tsButton->getTextureRectangle();
      elementInfoAVLTree.insertKeyAndValue(btnName, info);
    }
    
  resetTileTextureHandles();
  resetVisitors();
  resetCamera();
  
  for (int i = 0; i < terrainMemoryMonitor.capacity(); i++)
    terrainMemoryMonitor.put(0.0f);
    
  for (int i = 0; i < geometryMemoryMonitor.capacity(); i++)
    geometryMemoryMonitor.put(0.0f);
    
  for (int i = 0; i < pagefileMemoryMonitor.capacity(); i++)
    pagefileMemoryMonitor.put(0.0f);
    
  cylinder.set(10.0f, 80.0f, 20);
  cylinder.setTopVerticesColor(0.0f, 0.0f, 1.0f, 0.0f);
  cylinder.setBottomVerticesColor(0.1f, 0.45f, 1.0f, 0.42f);
  
  //if (!mouseVisible)
  //{
  //  cursors[CursorTypes::POINTER].load2D("Cursor.tga");
  //  cursors[CursorTypes::PAINT].load2D("Paint.tga");
  //  cursors[CursorTypes::FILL].load2D("Fill.tga");
  //  cursors[CursorTypes::MARKER].load2D("Marker.tga");
  //  cursors[CursorTypes::PASTEL].load2D("Pastel.tga");
  //  cursors[CursorTypes::ERASE].load2D("Erase.tga");
  //  cursors[CursorTypes::RESTORE].load2D("Restore.tga");
  //  cursors[CursorTypes::BURN].load2D("Flame.tga");
  //  cursors[CursorTypes::HEAL].load2D("Cure.tga");
  //  cursors[CursorTypes::RAISE].load2D("Raise.tga");
  //  cursors[CursorTypes::LOWER].load2D("Lower.tga");
  //  cursors[CursorTypes::TILE_ROTATE].load2D("Rotate.tga");
  //  cursors[CursorTypes::TILE_MIRROR].load2D("Mirror.tga");
  //  cursors[CursorTypes::TILE_LOGIC].load2D("Flag.tga");
  //  cursors[CursorTypes::TILE_ERASE].load2D("Erase.tga");
  //  cursors[CursorTypes::TILE_FLAG].load2D("Flag.tga");
  //  cursors[CursorTypes::NATURE].load2D("Nature.tga");
  //  cursors[CursorTypes::VILLAGE].load2D("Village.tga");
  //  cursors[CursorTypes::STRUCTURE].load2D("Structure.tga");
  //  cursors[CursorTypes::CHARACTER].load2D("Character.tga");
  //  cursors[CursorTypes::CRITTER].load2D("Critter.tga");
  //  cursors[CursorTypes::GRASS].load2D("Grass.tga");
  //  cursor = &cursors[CursorTypes::POINTER];
  //}
  
  return true;
}

void EditorScene::beginScene()
{
  Scene::beginScene();
  //mouseVisible = Gateway::getConfiguration().enableCursor;
  
  if (!paused)
  {
    reset();
    
    camera.setViewerPosition(Gateway::getMapDescriptor().camposition);
    camera.setFocusPosition(Gateway::getMapDescriptor().camfocus);
    
    GUICheckBox* checkbox;
    if (checkbox = (GUICheckBox*) gui.getWidgetByCallbackString("DebugCheckbox"))
    {
      switch (Gateway::getMapDescriptor().mapType)
      {
        case MapTypes::NEW_MAP:
          debugView = true;
          break;
          
        case MapTypes::LOAD_MAP:
          debugView = false;
          break;
          
      }
      
      checkbox->setChecked(debugView);
    }
    
    
    ///grid settings
    if (checkbox = (GUICheckBox*) gui.getWidgetByCallbackString("GridCheckbox"))
      checkbox->setChecked(false);
      
    Tuple2i gridArea = Gateway::getTerrainVisuals()->getArea();
    float s = gridArea.x > gridArea.y ? (float)gridArea.x : (float)gridArea.y;
    grid.setup(1, (float)round(s/4) * 4 , 4);
  }
  
  GUIButton* structureBtn = (GUIButton*) gui.getWidgetByCallbackString("StructureTexture");
  GUIButton* natureBtn = (GUIButton*) gui.getWidgetByCallbackString("NatureTexture");
  GUIButton* characterBtn = (GUIButton*) gui.getWidgetByCallbackString("CharacterTexture");
  GUIButton* critterBtn = (GUIButton*) gui.getWidgetByCallbackString("CritterTexture");
  GUIButton* villageBtn = (GUIButton*) gui.getWidgetByCallbackString("VillageTexture");
  GSElementInfo* info;
  
  TransformGroup* proto;
  Transform3D trans;
  Tuple3f minend, center;
  Texture btntexture;
  Tuple4f txrect(0,0,1,1);
  
  info = Gateway::getActiveStructureInfo();
  if (proto = Gateway::getStructurePrototype(info->name))
  {
    minend = proto->getBoundsDescriptor().getMinEndAABB();
    center = proto->getBoundsDescriptor().getCenterAABB();
    trans.setTranslations(-center.x, -minend.y, center.z);
    Gateway::getStructureDatabase()->renderPrototypeImage(proto->getName(), trans, Tuple2i(128,128), FRONT_TO_BACK|SORTED_BY_TEXTURE|ALL_EFFECTS);
    btntexture.create2DShell("sbtntx", 128, 128, GL_RGBA8, GL_RGBA, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    btntexture.copyCurrentBuffer();
    structureBtn->setTexture(btntexture);
    structureBtn->setTextureRectangle(txrect);
  }
  
  info = Gateway::getActiveNatureInfo();
  if (proto = Gateway::getNaturePrototype(info->name))
  {
    minend = proto->getBoundsDescriptor().getMinEndAABB();
    trans.setIdentity();
    trans.setTranslations(0, -minend.y, 0);
    trans.rotateY(90.0f * DEG2RAD);
    Gateway::getNatureDatabase()->renderPrototypeImage(proto->getName(), trans, Tuple2i(128,128), FRONT_TO_BACK|SORTED_BY_TEXTURE|ALL_EFFECTS);
    btntexture.create2DShell("nbtntx", 128, 128, GL_RGBA8, GL_RGBA, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    btntexture.copyCurrentBuffer();
    natureBtn->setTexture(btntexture);
    natureBtn->setTextureRectangle(txrect);
  }
  
  info = Gateway::getActiveCharacterInfo();
  if (proto = Gateway::getCharacterPrototype(info->name))
  {
    minend = proto->getBoundsDescriptor().getMinEndAABB();
    trans.setIdentity();
    trans.setTranslations(0, -minend.y, 0);
    trans.rotateY(-90.0f * DEG2RAD);
    Gateway::getCharacterDatabase()->renderPrototypeImage(proto->getName(), trans, Tuple2i(128,128), FRONT_TO_BACK|SORTED_BY_TEXTURE|ALL_EFFECTS);
    btntexture.create2DShell("cbtntx", 128, 128, GL_RGBA8, GL_RGBA, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    btntexture.copyCurrentBuffer();
    characterBtn->setTexture(btntexture);
    characterBtn->setTextureRectangle(txrect);
  }
  
  info = Gateway::getActiveCritterInfo();
  if (proto = Gateway::getCritterPrototype(info->name))
  {
    minend = proto->getBoundsDescriptor().getMinEndAABB();
    center = proto->getBoundsDescriptor().getCenterAABB();
    trans.setIdentity();
    trans.setTranslations(-center.x, -minend.y, -center.z);
    //trans.rotateY(45.0f * DEG2RAD);
    Gateway::getCritterDatabase()->renderPrototypeImage(proto->getName(), trans, Tuple2i(128,128), FRONT_TO_BACK|SORTED_BY_TEXTURE|ALL_EFFECTS);
    btntexture.create2DShell("crbtntx", 128, 128, GL_RGBA8, GL_RGBA, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    btntexture.copyCurrentBuffer();
    critterBtn->setTexture(btntexture);
    critterBtn->setTextureRectangle(txrect);
  }
  
  info = Gateway::getActiveVillageInfo();
  if (proto = Gateway::getVillagePrototype(info->name))
  {
    minend = proto->getBoundsDescriptor().getMinEndAABB();
    center = proto->getBoundsDescriptor().getCenterAABB();
    trans.setIdentity();
    trans.setTranslations(-center.x, -minend.y, -center.z);
    trans.rotateX(45.0f * DEG2RAD);
    Gateway::getVillageDatabase()->renderPrototypeImage(proto->getName(), trans, Tuple2i(128,128), FRONT_TO_BACK|SORTED_BY_TEXTURE|ALL_EFFECTS);
    btntexture.create2DShell("vbtntx", 128, 128, GL_RGBA8, GL_RGBA, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    btntexture.copyCurrentBuffer();
    villageBtn->setTexture(btntexture);
    villageBtn->setTextureRectangle(txrect);
  }
  
  resetConfiguration();
}

void EditorScene::pauseScene()
{
  paused = true;
  if (Gateway::getConfiguration().enableFog)
  {
    glDisable(GL_FOG);
    glClearColor(0,0,0,0);
  }
}

void EditorScene::endScene()
{
  paused = false;
  Gateway::reset();
}

void EditorScene::update(const FrameInfo &frameInfo)
{
  const FrameInfo* info = &frameInfo;
  
  camera.update(info->m_Interval);
  frustum.update();
  
  updateMinimap();
  
  if (memStatsPanel)
    updateMemoryMonitor(info->m_Interval);
    
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  //axis.draw();
  
  Gateway::inspectSpatialIndex(&tileRevealer);
  
  if (pickEnabled)
  {
    brush.update(info->m_Interval);
    Gateway::inspectSpatialIndex(&brush);
    brush.enableConstant(false);
  }
  
  if (drawBase)
  {
    renderer.enableBlend(false);
    Gateway::inspectTerrainGraph(0, &renderer);
  }
  
  if (drawTop)
  {
    renderer.enableBlend(blendEnabled);
    Gateway::inspectTerrainGraph(1, &renderer);
  }
  
  if (debugView)
  {
    glPolygonMode(GL_FRONT, GL_LINE);
    Gateway::inspectSpatialIndex(&debugViewer);
    glPolygonMode(GL_FRONT, GL_FILL);
  }
  
  if (drawGrid)
    grid.draw();
    
  if (drawLogic)
    Gateway::inspectSpatialIndex(&logicViewer);
    
  if (drawFlags)
    Gateway::inspectSpatialIndex(&flagViewer);
    
  if (drawVillages)
    villageGroup->render(FRONT_TO_BACK|SORTED_BY_TEXTURE|ALL_EFFECTS, &frustum);
    
  if (drawStructures)
    structureGroup->render(FRONT_TO_BACK|SORTED_BY_TEXTURE|ALL_EFFECTS, &frustum);
    
  if (drawNature)
    natureGroup->render(FRONT_TO_BACK|SORTED_BY_TEXTURE|ALL_EFFECTS, &frustum);
    
  if (drawCharacters)
    characterGroup->render(FRONT_TO_BACK|SORTED_BY_TEXTURE|ALL_EFFECTS, &frustum);
    
  if (drawCritters)
    critterGroup->render(FRONT_TO_BACK|SORTED_BY_TEXTURE|ALL_EFFECTS, &frustum);
    
  if (drawCollMark)
    drawCollisionMark();
    
  if (drawWater)
  {
    waterGroup->render(FRONT_TO_BACK|SORTED_BY_TEXTURE|ALL_EFFECTS, &frustum);
//  TerrainWater* terrainWater = Gateway::getTerrainWater();
//  for (unsigned int i = 0; i < terrainWater->getWaterCount(); i++)
//   terrainWater->getWater(i)->draw(Water::FILL);

    if (currentWater)
      currentWater->draw(Water::FILL | Water::WIRE);
  }
  
  if (fpsCounter)
    fpsCounter->setLabelString(String("FPS: ") + int(info->m_Fps));
    
  Renderer::enter2DMode(info->m_Width, info->m_Height);
  gui.render(info->m_Interval);
  drawCursor();
  Renderer::exit2DMode();
}

void EditorScene::actionPerformed(GUIEvent &evt)
{
  const String &callbackString = evt.getCallbackString();
  GUIRectangle *sourceRectangle = evt.getEventSource();
  int      widgetType   = sourceRectangle->getWidgetType();
  
  //------------------------
  ///GUICheckBox
  //------------------------
  if (widgetType == CHECK_BOX)
  {
    GUICheckBox *checkBox = (GUICheckBox*) sourceRectangle;
    
    if (checkBox->isClicked())
    {
      //
      //layer tab
      //
      if (callbackString == "TileLayer1Visibility")
      {
        drawTop = checkBox->isChecked();
        return;
      }
      
      if (callbackString == "TileLayer0Visibility")
      {
        drawBase = checkBox->isChecked();
        return;
      }
      
      if (callbackString == "NatureLayerVisibility")
      {
        drawNature = checkBox->isChecked();
        return;
      }
      
      if (callbackString == "StructureLayerVisibility")
      {
        drawStructures = checkBox->isChecked();
        return;
      }
      
      if (callbackString == "VillageLayerVisibility")
      {
        drawVillages = checkBox->isChecked();
        return;
      }
      
      if (callbackString == "CharacterLayerVisibility")
      {
        drawCharacters = checkBox->isChecked();
        return;
      }
      
      if (callbackString == "CritterLayerVisibility")
      {
        drawCritters = checkBox->isChecked();
        return;
      }
      
      if (callbackString == "GrassLayerVisibility")
      {
        drawPasture = checkBox->isChecked();
        return;
      }
      
      if (callbackString == "WaterLayerVisibility")
      {
        drawWater = checkBox->isChecked();
        return;
      }
      
      if (callbackString == "LayerBlendCheckbox")
      {
        blendEnabled = checkBox->isChecked();
        return;
      }
      
      //
      //brush tab
      //
      if (callbackString == "PickingCheckbox")
      {
        pickEnabled = checkBox->isChecked();
        return;
      }
      
      //
      //debug tab
      //
      if (callbackString == "TextureCheckbox")
      {
        renderer.enableTexture(checkBox->isChecked());
        return;
      }
      
      if (callbackString == "GuidesCheckbox")
      {
        brush.enableGuides(checkBox->isChecked());
        return;
      }
      
      if (callbackString == "DebugCheckbox")
      {
        debugView = checkBox->isChecked();
        return;
      }
      
      if (callbackString == "LogicCheckbox")
      {
        drawLogic = checkBox->isChecked();
        return;
      }
      
      if (callbackString == "FlagsCheckbox")
      {
        drawFlags = checkBox->isChecked();
        return;
      }
      
      if (callbackString == "WireCheckbox")
      {
        renderer.enableWireMode(checkBox->isChecked());
        return;
      }
      
      if (callbackString == "GridCheckbox")
      {
        drawGrid = checkBox->isChecked();
        return;
      }
      
      //
      //pastures
      //
      //TerrainPasture* terrainPasture = Gateway::getTerrainPasture();
      //TransformGroup* mwTGGroup;
      //for (unsigned int i = 0; i < terrainPasture->getMeadowCount(); i++)
      //{
      //  const String& meadowName = terrainPasture->getMeadow(i)->getName();
      //  OCString ocstring(callbackString.getBytes());
      //  if (ocstring.find(meadowName.getBytes()) != OCString::npos)
      //  {
      //    if (mwTGGroup = mwdatabase->getRootGroup()->getGroup(meadowName))
      //      mwTGGroup->setVisible(checkBox->isChecked());
      //    return;
      //  }
      //}
    }
  }
  
  //------------------------
  ///GUIButton
  //------------------------
  if (widgetType == BUTTON)
  {
    GUIButton *button = (GUIButton*) sourceRectangle;
    
    if (button->isClicked())
    {
      if (callbackString == "TileTexture")
      {
        GUIPanel* tsPanel = (GUIPanel*) gui.getWidgetByCallbackString("TilesetPanel");
        tsPanel->setVisible(!tsPanel->isVisible());
        return;
      }
      
      if (callbackString == "ClearTileLayer0")
      {
        TerrainDatabase* tbatadase = Gateway::getTerrainDatabase();
        for (unsigned int i = 0; i < tbatadase->getControllerCount(); i++)
          tbatadase->changeTileTexture(0, -1, i);
        return;
      }
      
      if (callbackString == "ClearTileLayer1")
      {
        TerrainDatabase* tbatadase = Gateway::getTerrainDatabase();
        for (unsigned int i = 0; i < tbatadase->getControllerCount(); i++)
          tbatadase->changeTileTexture(1, -1, i);
        return;
      }
      
      if (callbackString == "ClearNatureLayer")
      {
        //natureGroup->destroy();
        Gateway::getNatureDatabase()->reset();
        Gateway::getWorldVisuals()->destroyObjects(WorldObjectTypes::NATURE);
        return;
      }
      
      if (callbackString == "ClearStructureLayer")
      {
        //structureGroup->destroy();
        Gateway::getStructureDatabase()->reset();
        Gateway::getWorldVisuals()->destroyObjects(WorldObjectTypes::STRUCTURE);
        return;
      }
      
      if (callbackString == "ClearVillageLayer")
      {
        //villageGroup->destroy();
        Gateway::getVillageDatabase()->reset();
        return;
      }
      
      if (callbackString == "ClearCharacterLayer")
      {
        //characterGroup->destroy();
        Gateway::getCharacterDatabase()->reset();
        return;
      }
      
      if (callbackString == "ClearCritterLayer")
      {
        //critterGroup->destroy();
        Gateway::getCritterDatabase()->reset();
        return;
      }
      
      if (callbackString == "ClearWaterLayer")
      {
//        Gateway::getTerrainWater()->destroyAllWaters();
        Gateway::getWaterDatabase()->reset();
        deleteObject(currentWater);
        brush.setWaterModel(0);
        return;
      }
      
      if (callbackString == "GrassCreateButton")
      {
        TerrainPasture* terrainPasture = Gateway::getTerrainPasture();
        Meadow* meadow = terrainPasture->createMeadow();
        addPastureToCombobox(meadow->getName());
        return;
      }
      
      if (callbackString == "WaterCreateButton")
      {
        if (currentWater)
          return;
        currentWater = Gateway::getTerrainWater()->createWater();
        if (GUIButton* btn = (GUIButton*) gui.getWidgetByCallbackString("WaterColorPreviewWindow"))
        {
          Tuple4f btncol = btn->getColor();
          currentWater->setColor(btncol.x, btncol.y, btncol.z, ((GUISlider*)gui.getWidgetByCallbackString("WaterAlphaSlider"))->getProgress());
        }
        brush.setWaterModel(currentWater);
        GUIRadioButton* rbutton;
        if (rbutton = (GUIRadioButton*) gui.getWidgetByCallbackString("WaterTabAddVertRBTN"))
        {
          rbutton->setVisible(true);
          rbutton->setActive(true);
          rbutton->setChecked(false);
        }
        if (rbutton = (GUIRadioButton*) gui.getWidgetByCallbackString("WaterTabRemoveTriRBTN"))
        {
          rbutton->setVisible(true);
          rbutton->setActive(true);
          rbutton->setChecked(false);
        }
        return;
      }
      
      if (callbackString == "WaterCancelButton")
      {
        if (!currentWater)
          return;
        deleteObject(currentWater);
        brush.setWaterModel(0);
        GUIRadioButton* rbutton;
        if (rbutton = (GUIRadioButton*) gui.getWidgetByCallbackString("WaterTabAddVertRBTN"))
        {
          rbutton->setVisible(false);
          rbutton->setActive(false);
          rbutton->setChecked(false);
        }
        if (rbutton = (GUIRadioButton*) gui.getWidgetByCallbackString("WaterTabRemoveTriRBTN"))
        {
          rbutton->setVisible(false);
          rbutton->setActive(false);
          rbutton->setChecked(false);
        }
        return;
      }
      
      if (callbackString == "WaterFinalizeButton")
      {
        if (!currentWater)
          return;
        if (currentWater->getTriangleCount() > 0)
        {
//     Gateway::getTerrainWater()->addWater(currentWater);
          Gateway::getWaterDatabase()->addWaterModel(currentWater);
          currentWater = 0;
          brush.setWaterModel(0);
          GUIRadioButton* rbutton;
          if (rbutton = (GUIRadioButton*) gui.getWidgetByCallbackString("WaterTabAddVertRBTN"))
          {
            rbutton->setVisible(false);
            rbutton->setActive(false);
            rbutton->setChecked(false);
          }
          if (rbutton = (GUIRadioButton*) gui.getWidgetByCallbackString("WaterTabRemoveTriRBTN"))
          {
            rbutton->setVisible(false);
            rbutton->setActive(false);
            rbutton->setChecked(false);
          }
        }
        return;
      }
      
      if (callbackString == "PMOKButton")
      {
        GUIPanel* messagePanel = (GUIPanel*) gui.getWidgetByCallbackString("PopupMessagePanel");
        if (messagePanel)
          messagePanel->setVisible(false);
        enableGUI(true);
        return;
      }
      
      if (callbackString == "Export")
      {
        GUIPanel* messagePanel = (GUIPanel*) gui.getWidgetByCallbackString("PopupMessagePanel");
        GUILabel* messageLabel = (GUILabel*) gui.getWidgetByCallbackString("PopupMessageLabel");
        
        if (!Gateway::exportMap())
        {
          if (messageLabel)
            messageLabel->getLabel()->setString("Error while exporting dataset");
        }
        else
        {
          if (messageLabel)
            messageLabel->getLabel()->setString("Dataset exported successfully");
        }
        
        if (messagePanel)
          messagePanel->setVisible(true);
        enableGUI(false);
        
        return;
      }
      
      GUIPanel* tsPanel = (GUIPanel*) gui.getWidgetByCallbackString("TilesetPanel");
      if (tsPanel->isVisible())
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
        
        if (TSElementInfo* info = getTSElementInfo(callbackString))
        {
          GUIButton* tileBTN = (GUIButton*) gui.getWidgetByCallbackString("TileTexture");
          tileBTN->setTextureRectangle(info->coords);
          tileBTN->setTexture(*info->texture);
          
          GUIPanel* labPanel = (GUIPanel*) gui.getWidgetByCallbackString("LaboratoryPanel");
          labPanel->forceUpdate(true);
          
          brush.setPaintIndex(info->pageIndex * 16 + info->coordsIndex);
          
          return;
        }
      }
      
      //
      //pastures
      //
      ///clear the meadow
      TerrainPasture* terrainPasture = Gateway::getTerrainPasture();
      for (unsigned int i = 0; i < terrainPasture->getMeadowCount(); i++)
      {
        const String& meadowName = terrainPasture->getMeadow(i)->getName();
        OCString ocstring(callbackString.getBytes());
        if (ocstring.find(meadowName.getBytes()) != OCString::npos)
        {
          terrainPasture->clearMeadowPatches(meadowName);
          return;
        }
      }
      
      sceneController.execute(callbackString);
    }
  }
  
  //------------------------
  ///GUISlider
  //------------------------
  if (widgetType == SLIDER)
  {
    GUISlider *slider = (GUISlider*) sourceRectangle;
    
    if (callbackString == "BrushRadius")
    {
      if (slider->isReleased())
        brush.setRadius(slider->getProgress() * 10.0f);
        
      slider->setLabelString(String("Radius: ") + slider->getProgress() * 10.0f);
      
      return;
    }
    
    if (callbackString == "BrushStrength")
    {
      if (slider->isReleased())
        brush.setStrength(slider->getProgress());
        
      slider->setLabelString(String("Strength: ") + slider->getProgress());
      
      return;
    }
    
    if (callbackString == "BrushOpacity")
    {
      if (slider->isReleased())
        brush.setOpacity(slider->getProgress());
        
      slider->setLabelString(String("Opacity: ") + slider->getProgress());
      
      return;
    }
    
    if (callbackString == "WaterRedSlider")
    {
      if (slider->isPressed())
      {
        if (currentWater)
        {
          Tuple4f c = currentWater->getColor();
          currentWater->setColor(slider->getProgress(), c.y, c.z, c.w);
        }
      }
      
      if (GUILabel* lbl = (GUILabel*) gui.getWidgetByCallbackString("WRLabel"))
        lbl->setLabelString(String("R: ") + slider->getProgress());
        
      if (GUIButton* btn = (GUIButton*) gui.getWidgetByCallbackString("WaterColorPreviewWindow"))
      {
        Tuple4f btncolor = btn->getColor();
        btncolor.x = slider->getProgress();
        btn->setColor(btncolor.x, btncolor.y, btncolor.z);
      }
      
      return;
    }
    
    if (callbackString == "WaterGreenSlider")
    {
      if (slider->isPressed())
      {
        if (currentWater)
        {
          Tuple4f c = currentWater->getColor();
          currentWater->setColor(c.x, slider->getProgress(), c.z, c.w);
        }
      }
      
      if (GUILabel* lbl = (GUILabel*) gui.getWidgetByCallbackString("WGLabel"))
        lbl->setLabelString(String("G: ") + slider->getProgress());
        
      if (GUIButton* btn = (GUIButton*) gui.getWidgetByCallbackString("WaterColorPreviewWindow"))
      {
        Tuple4f btncolor = btn->getColor();
        btncolor.y = slider->getProgress();
        btn->setColor(btncolor.x, btncolor.y, btncolor.z);
      }
      
      return;
    }
    
    if (callbackString == "WaterBlueSlider")
    {
      if (slider->isPressed())
      {
        if (currentWater)
        {
          Tuple4f c = currentWater->getColor();
          currentWater->setColor(c.x, c.y, slider->getProgress(), c.w);
        }
      }
      
      if (GUILabel* lbl = (GUILabel*) gui.getWidgetByCallbackString("WBLabel"))
        lbl->setLabelString(String("B: ") + slider->getProgress());
        
      if (GUIButton* btn = (GUIButton*) gui.getWidgetByCallbackString("WaterColorPreviewWindow"))
      {
        Tuple4f btncolor = btn->getColor();
        btncolor.z = slider->getProgress();
        btn->setColor(btncolor.x, btncolor.y, btncolor.z);
      }
      return;
    }
    
    if (callbackString == "WaterAlphaSlider")
    {
      if (slider->isPressed())
      {
        if (currentWater)
        {
          Tuple4f c = currentWater->getColor();
          currentWater->setColor(c.x, c.y, c.z, slider->getProgress());
        }
      }
      
      if (GUILabel* lbl = (GUILabel*) gui.getWidgetByCallbackString("WALabel"))
        lbl->setLabelString(String("A: ") + slider->getProgress());
        
      //if (GUIButton* btn = (GUIButton*) gui.getWidgetByCallbackString("WaterColorPreviewWindow"))
      //  btn->setMinAlpha(slider->getProgress());
      if (GUIButton*btn=(GUIButton*)gui.getWidgetByCallbackString("WaterColorPreviewWindow"))
        btn->setAlpha(slider->getProgress());
      return;
    }
  }
  
  //------------------------
  ///GUIRadioButton
  //------------------------
  if (widgetType == RADIO_BUTTON)
  {
    GUIRadioButton *radioButton = (GUIRadioButton*) sourceRectangle;
    
    if (radioButton->isPressed())
    {
      //
      //brush types
      //
      if (callbackString == "TileMode")
      {
        brush.setType(BrushTypes::TILE);
        return;
      }
      
      if (callbackString == "GrassMode")
      {
        brush.setType(BrushTypes::GRASS);
        return;
      }
      
      if (callbackString == "WaterMode")
      {
        brush.setType(BrushTypes::WATER);
        return;
      }
      
      if (callbackString == "ModelMode")
      {
        brush.setType(BrushTypes::MODEL);
        return;
      }
      
      //
      //brush modes
      //
      if (callbackString == "PrimaryLayer")
      {
        brush.setPaintLayer(0);
        brush.setLayer(BrushLayers::TILE);
        //cursor = &cursors[CursorTypes::POINTER];
        return;
      }
      
      if (callbackString == "SecondaryLayer")
      {
        brush.setPaintLayer(1);
        brush.setLayer(BrushLayers::TILE);
        //cursor = &cursors[CursorTypes::POINTER];
        return;
      }
      
      if (callbackString == "NatureLayerSelection")
      {
        brush.setLayer(BrushLayers::NATURE);
        //cursor = &cursors[CursorTypes::NATURE];
        return;
      }
      
      if (callbackString == "StructureLayerSelection")
      {
        brush.setLayer(BrushLayers::STRUCTURE);
        //cursor = &cursors[CursorTypes::STRUCTURE];
        return;
      }
      
      if (callbackString == "VillageLayerSelection")
      {
        brush.setLayer(BrushLayers::VILLAGE);
        //cursor = &cursors[CursorTypes::VILLAGE];
        return;
      }
      
      if (callbackString == "CharacterLayerSelection")
      {
        brush.setLayer(BrushLayers::CHARACTER);
        //cursor = &cursors[CursorTypes::CHARACTER];
        return;
      }
      
      if (callbackString == "CritterLayerSelection")
      {
        brush.setLayer(BrushLayers::CRITTER);
        //cursor = &cursors[CursorTypes::CRITTER];
        return;
      }
      
      if (callbackString == "GrassLayerSelection")
      {
        brush.setLayer(BrushLayers::GRASS);
        //cursor = &cursors[CursorTypes::GRASS];
        return;
      }
      
      if (callbackString == "WaterLayerSelection")
      {
        brush.setLayer(BrushLayers::WATER);
        //cursor = &cursors[CursorTypes::WATER];
        return;
      }
      
      if (callbackString == "WaterTabAddVertRBTN")
      {
        brush.setMode(BrushModes::VERTEX);
        return;
      }
      
      if (callbackString == "WaterTabRemoveTriRBTN")
      {
        brush.setMode(BrushModes::TRIANGLE);
        return;
      }
      
      //
      //pastures
      //
      TerrainPasture* terrainPasture = Gateway::getTerrainPasture();
      for (unsigned int i = 0; i < terrainPasture->getMeadowCount(); i++)
      {
        const String& meadowName = terrainPasture->getMeadow(i)->getName();
        OCString ocstring(callbackString.getBytes());
        if (ocstring.find(meadowName.getBytes()) != OCString::npos)
        {
          brush.setMeadowName(meadowName);
          return;
        }
      }
    }
  }
  
  //------------------------
  ///GUIComboBox
  //------------------------
  if (widgetType == COMBO_BOX)
  {
    GUIComboBox *comboBox = (GUIComboBox*) sourceRectangle;
    const String &item = comboBox->getSelectedItem();
    
    //
    //brush modes
    //
    if (callbackString == "BrushTypesCB")
    {
      if (item == "Pointer")
      {
        brush.setMode(BrushModes::POINTER);
        //cursor = &cursors[CursorTypes::POINTER];
        return;
      }
      
      if (item == "Paint")
      {
        brush.setMode(BrushModes::PAINT);
        //cursor = &cursors[CursorTypes::PAINT];
        return;
      }
      
      if (item == "Fill")
      {
        brush.setMode(BrushModes::FILL);
        //cursor = &cursors[CursorTypes::FILL];
        return;
      }
      
      if (item == "Marker")
      {
        brush.setMode(BrushModes::MARKER);
        //cursor = &cursors[CursorTypes::MARKER];
        return;
      }
      
      if (item == "Pastel")
      {
        brush.setMode(BrushModes::PASTEL);
        //cursor = &cursors[CursorTypes::PASTEL];
        return;
      }
      
      if (item == "Raise")
      {
        brush.setMode(BrushModes::RAISE);
        //cursor = &cursors[CursorTypes::RAISE];
        GUISlider* bslider = (GUISlider*) gui.getWidgetByCallbackString("BrushStrength");
        if (bslider)
          brush.setStrength(bslider->getProgress());
        return;
      }
      
      if (item == "Lower")
      {
        brush.setMode(BrushModes::LOWER);
        //cursor = &cursors[CursorTypes::LOWER];
        GUISlider* bslider = (GUISlider*) gui.getWidgetByCallbackString("BrushStrength");
        if (bslider)
          brush.setStrength(bslider->getProgress());
        return;
      }
      
      if (item == "Erase")
      {
        brush.setMode(BrushModes::ERASE);
        //cursor = &cursors[CursorTypes::ERASE];
        return;
      }
      
      if (item == "Restore")
      {
        brush.setMode(BrushModes::RESTORE);
        //cursor = &cursors[CursorTypes::RESTORE];
        return;
      }
      
      if (item == "Burn")
      {
        brush.setMode(BrushModes::BURN);
        //cursor = &cursors[CursorTypes::BURN];
        return;
      }
      
      if (item == "Heal")
      {
        brush.setMode(BrushModes::HEAL);
        //cursor = &cursors[CursorTypes::HEAL];
        return;
      }
      
      if (item == "Tile erase")
      {
        brush.setMode(BrushModes::ERASETILE);
        //cursor = &cursors[CursorTypes::ERASE];
        return;
      }
      
      if (item == "Tile logic")
      {
        brush.setMode(BrushModes::LOGIC);
        //cursor = &cursors[CursorTypes::TILE_LOGIC];
        return;
      }
      
      if (item == "Tile flag")
      {
        brush.setMode(BrushModes::FLAG);
        //cursor = &cursors[CursorTypes::TILE_FLAG];
        return;
      }
      
      if (item == "Advanced")
      {
        brush.setMode(BrushModes::ADVANCED);
        //cursor = &cursors[CursorTypes::PAINT];
        return;
      }
    }
    
    //
    //tile types
    //
    if (callbackString == "TileTypesCB")
    {
      if (item == "Scenery end")
      {
        setLogicFlag(TileTypes::SCENERY_END);
        return;
      }
      
      if (item == "Unused")
      {
        setLogicFlag(TileTypes::UNUSED);
        return;
      }
      
      if (item == "Snow")
      {
        setLogicFlag(TileTypes::SNOW);
        return;
      }
      
      if (item == "Blocked")
      {
        setLogicFlag(TileTypes::BLOCKED);
        return;
      }
      
      if (item == "Wall")
      {
        setLogicFlag(TileTypes::WALL);
        return;
      }
      
      if (item == "Forest")
      {
        setLogicFlag(TileTypes::FOREST);
        return;
      }
      
      if (item == "Light trees")
      {
        setLogicFlag(TileTypes::LIGHT_TREES);
        return;
      }
      
      if (item == "Grassfield")
      {
        setLogicFlag(TileTypes::GRASSFIELD);
        return;
      }
      
      if (item == "Shore")
      {
        setLogicFlag(TileTypes::SHORE);
        return;
      }
      
      if (item == "Normal")
      {
        setLogicFlag(TileTypes::NORMAL);
        return;
      }
      
      if (item == "Sand")
      {
        setLogicFlag(TileTypes::SAND);
        return;
      }
      
      if (item == "Deep water")
      {
        setLogicFlag(TileTypes::DEEP_WATER);
        return;
      }
    }
    
    //
    //tile logic
    //
    if (callbackString == "TileLogicCB")
    {
      if (item == "None")
      {
        setFlag(TileLogic::FLAG_NONE);
        return;
      }
      
      if (item == "Not passable")
      {
        setFlag(TileLogic::FLAG_NOPASSABLE);
        return;
      }
      
      if (item == "Flat")
      {
        setFlag(TileLogic::FLAG_FLAT);
        return;
      }
      
      if (item == "Tilt_1")
      {
        setFlag(TileLogic::FLAG_TILT1);
        return;
      }
      
      if (item == "Tilt_2")
      {
        setFlag(TileLogic::FLAG_TILT2);
        return;
      }
      
      if (item == "Tilt_3")
      {
        setFlag(TileLogic::FLAG_TILT3);
        return;
      }
      
      if (item == "Structure")
      {
        setFlag(TileLogic::FLAG_STRUCTURE);
        return;
      }
      
      if (item == "Bridge")
      {
        setFlag(TileLogic::FLAG_BRIDGE);
        return;
      }
      
      if (item == "Modified")
      {
        setFlag(TileLogic::FLAG_MODIFIED);
        return;
      }
      
      if (item == "Door")
      {
        setFlag(TileLogic::FLAG_DOOR);
        return;
      }
      
      if (item == "No siege")
      {
        setFlag(TileLogic::FLAG_NO_SIEGE);
        return;
      }
      
      if (item == "Buildable")
      {
        setFlag(TileLogic::FLAG_BUILDABLE);
        return;
      }
      
      if (item == "Wall open")
      {
        setFlag(TileLogic::FLAG_WALL_OPEN);
        return;
      }
      
      if (item == "Wall blocked")
      {
        setFlag(TileLogic::FLAG_WALL_BLOCKED);
        return;
      }
      
      if (item == "Fortress")
      {
        setFlag(TileLogic::FLAG_FORTRESS);
        return;
      }
      
      if (item == "Blocked build")
      {
        setFlag(TileLogic::FLAG_BLOCKED_BUILD);
        return;
      }
      
      if (item == "Flat structure")
      {
        setFlag(TileLogic::FLAG_FLAT_STRUCTURE);
        return;
      }
      
      if (item == "Scenery end")
      {
        setFlag(TileLogic::FLAG_SCENERY_END);
        return;
      }
      
      if (item == "Wall ground")
      {
        setFlag(TileLogic::FLAG_WALL_GROUND);
        return;
      }
      
      if (item == "Block war machines")
      {
        setFlag(TileLogic::FLAG_BLOCK_WAR_MACHINES);
        return;
      }
    }
    
    //
    //tile orientation
    //
    if (callbackString == "TileOrientationCB")
    {
      if (item == "Rotate 90")
      {
        brush.setMode(BrushModes::ROTATE90);
        //cursor = &cursors[CursorTypes::TILE_ROTATE];
        return;
      }
      
      if (item == "Rotate 180")
      {
        brush.setMode(BrushModes::ROTATE180);
        //cursor = &cursors[CursorTypes::TILE_ROTATE];
        return;
      }
      
      if (item == "Rotate 270")
      {
        brush.setMode(BrushModes::ROTATE270);
        //cursor = &cursors[CursorTypes::TILE_ROTATE];
        return;
      }
    }
    
    if (callbackString == "TileMirrorCB")
    {
      if (item == "Mirror X")
      {
        brush.setMode(BrushModes::MIRRORX);
        //cursor = &cursors[CursorTypes::TILE_MIRROR];
        return;
      }
      
      if (item == "Mirror Y")
      {
        brush.setMode(BrushModes::MIRRORY);
        //cursor = &cursors[CursorTypes::TILE_MIRROR];
        return;
      }
      
      //if (item == "Visible")
      //{
      //  brush.setMode(BrushModes::VISIBLE);
      //  //cursor = &cursors[CursorTypes::TILE_MIRROR];
      //  return;
      //}
      //
      //if (item == "Diagonal 0")
      //{
      //  brush.setMode(BrushModes::QUAD0DIAGONAL);
      //  //cursor = &cursors[CursorTypes::TILE_MIRROR];
      //  return;
      //}
      //
      //if (item == "Diagonal 1")
      //{
      //  brush.setMode(BrushModes::QUAD1DIAGONAL);
      //  //cursor = &cursors[CursorTypes::TILE_MIRROR];
      //  return;
      //}
      //
      //if (item == "Diagonal 2")
      //{
      //  brush.setMode(BrushModes::QUAD2DIAGONAL);
      //  //cursor = &cursors[CursorTypes::TILE_MIRROR];
      //  return;
      //}
      //
      //if (item == "Diagonal 3")
      //{
      //  brush.setMode(BrushModes::QUAD3DIAGONAL);
      //  //cursor = &cursors[CursorTypes::TILE_MIRROR];
      //  return;
      //}
      //
      //if (item == "Dynamic mix")
      //{
      //  brush.setMode(BrushModes::DYNAMICMIX);
      //  //cursor = &cursors[CursorTypes::TILE_MIRROR];
      //  return;
      //}
      //
      //if (item == "No tesselate")
      //{
      //  brush.setMode(BrushModes::NOTESSELATE);
      //  //cursor = &cursors[CursorTypes::TILE_MIRROR];
      //  return;
      //}
    }
  }
}

void EditorScene::handleMouseEvent(MouseEvent& evt, int extraInfo)
{
  Scene::handleMouseEvent(evt, extraInfo);
  
  GUIPanel* messagePanel = (GUIPanel*) gui.getWidgetByCallbackString("PopupMessagePanel");
  if (messagePanel->isVisible())
    return;
    
  switch (extraInfo)
  {
    case MOVED:
    
      camera.lockMouse(false);
      camera.setMouseInfo(evt.getX(), evt.getY());
      
      if (pickEnabled)
        ray.set(camera.getViewerPosition(), Renderer::getWorldCoords(Tuple3f((float)evt.getX(), (float)evt.getYInverse(), 1)));
        
      break;
      
    case CLICKED:
      if (gui.isVisible())
      {
        Widgets wgs = gui.getWidgets();
        for (size_t t = 0; t < wgs.size(); t++)
          if (wgs[t]->getWidgetType() == PANEL && wgs[t]->isVisible())
          {
            const Tuple4i* panelBounds = &wgs[t]->getWindowBounds();
            mouseLocked |= ((evt.getY() >= panelBounds->y) && (evt.getY() <= panelBounds->w) &&
                            (evt.getX() >= panelBounds->x) && (evt.getX() <= panelBounds->z));
          }
      }
      
      brush.enableConstant(true);
      
      if (evt.getButtonID() == BUTTON1)
        brush.enable(!mouseLocked);
        
      break;
      
    case DRAGGED:
    
      if (evt.getButtonID() == BUTTON2)
      {
        camera.lockMouse(!mouseLocked);
        camera.setMouseInfo(evt.getX(), evt.getY());
      }
      
      if (pickEnabled)
        ray.set(camera.getViewerPosition(), Renderer::getWorldCoords(Tuple3f((float)evt.getX(), (float)evt.getYInverse(), 1)));
        
      break;
      
    case RELEASED:
    
      mouseLocked = false;
      brush.enable(false);
      
      break;
  }
}

void EditorScene::handleKeyEvent(KeyEvent evt, int extraInfo)
{
  int keyID;
  TerrainDatabase* tdatabase;
  //GUIPanel* messagePanel;
  
  Scene::handleKeyEvent(evt, extraInfo);
  
  //messagePanel = (GUIPanel*) gui.getWidgetByCallbackString("PopupMessagePanel");
  if (gui.getWidgetByCallbackString("PopupMessagePanel")->isVisible())
    return;
    
  keyID = evt.getKeyID();
  
  switch (extraInfo)
  {
    case KEY_RELEASED:
      switch (keyID)
      {
        case KEY_H:
          gui.setVisible(!gui.isVisible());
          break;
          
        case KEY_M:
          if (autoMap)
            autoMap->setVisible(!autoMap->isVisible());
          break;
          
        case KEY_P:
          if (memStatsPanel)
            memStatsPanel->setVisible(!memStatsPanel->isVisible());
          break;
          
        case KEY_T:
          if (toolsConfigPanel)
            toolsConfigPanel->setVisible(!toolsConfigPanel->isVisible());
          break;
          
        case KEY_C:
          drawCollMark = false;
          brush.saveCollisionPoint(false);
          break;
          
        case KEY_SPACE:
          brush.enableLogicFlag(true);
          break;
      }
      
      camera.setKeyboardInput(evt, false);
      
      break;
      
    case KEY_PRESSED:
      switch (keyID)
      {
        case KEY_C:
          drawCollMark = true;
          brush.saveCollisionPoint(true);
          break;
          
        case KEY_F1:
          tdatabase = Gateway::getTerrainDatabase();
          tdatabase->clearAlphaMap();
          break;
          
        case KEY_F2:
          tdatabase = Gateway::getTerrainDatabase();
          tdatabase->clearColorMap();
          break;
          
        case KEY_SPACE:
          brush.enableLogicFlag(false);
          break;
      }
      
      camera.setKeyboardInput(evt, true);
      
      break;
  }
}

void EditorScene::drawFullScreenQuad(int width, int height)
{
  glBegin(GL_TRIANGLE_STRIP);
  glTexCoord2f(0.0f, 0.0f);
  glVertex2i(0, 0);
  glTexCoord2f(0.0f, 1.0f);
  glVertex2i(0, height);
  glTexCoord2f(1.0f, 0.0f);
  glVertex2i(width, 0);
  glTexCoord2f(1.0f, 1.0f);
  glVertex2i(width, height);
  glEnd();
}

void EditorScene::drawFullScreenQuadInverse(int width, int height)
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

void EditorScene::drawCollisionMark()
{
  Tuple4f collpoint = brush.getCollisionPoint();
  Tuple3f ext;
  int brushlayer = brush.getLayer();
  GSElementInfo* gseInfo = 0;
  TransformGroup* proto = 0;
  if (collpoint.w)
  {
    switch (brushlayer)
    {
      case BrushLayers::NATURE:
        proto = Gateway::getActiveNature();
        break;
        
      case BrushLayers::CHARACTER:
        proto = Gateway::getActiveCharacter();
        break;
        
      case BrushLayers::CRITTER:
        proto = Gateway::getActiveCritter();
        break;
        
      case BrushLayers::STRUCTURE:
        proto = Gateway::getActiveStructure();
        break;
        
      case BrushLayers::VILLAGE:
        proto = Gateway::getActiveVillage();
        break;
        
      default:
        break;
    }
    
    if (proto)
    {
      ext = proto->getBoundsDescriptor().getExtents();
      ext.y = 0;
      
      cylinder.setRadius(ext.getLength());
      
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
      glTranslatef(collpoint.x, collpoint.y + 0.05f, collpoint.z);
      proto->render(ALL_EFFECTS);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE);
      glEnable(GL_CULL_FACE);
      glCullFace(GL_BACK);
      glTranslatef(0, -collpoint.y - 0.05f -6.0f, 0);
      cylinder.draw();
      glDisable(GL_CULL_FACE);
      glDisable(GL_BLEND);
      glPopMatrix();
    }
  }
}

void EditorScene::firstTileSet()
{
  currentIndex = 0;
  updateOffscreenTexture(&offscreenTexture);
  
  if (tilesetLabel)
    tilesetLabel->setLabelString(String("Tileset ") + (currentIndex + 1) + String("/") + int(textureHandles.length()));
}

void EditorScene::lastTileSet()
{
  currentIndex = textureHandles.length() - 1;
  updateOffscreenTexture(&offscreenTexture);
  
  if (tilesetLabel)
    tilesetLabel->setLabelString(String("Tileset ") + (currentIndex + 1) + String("/") + int(textureHandles.length()));
}

void EditorScene::previousTileSet()
{
  currentIndex = clamp(--currentIndex, 0, (int)textureHandles.length() - 1);
  updateOffscreenTexture(&offscreenTexture);
  
  if (tilesetLabel)
    tilesetLabel->setLabelString(String("Tileset ") + (currentIndex + 1) + String("/") + int(textureHandles.length()));
}

void EditorScene::nextTileSet()
{
  currentIndex = clamp(++currentIndex, 0, (int)textureHandles.length() - 1);
  updateOffscreenTexture(&offscreenTexture);
  
  if (tilesetLabel)
    tilesetLabel->setLabelString(String("Tileset ") + (currentIndex + 1) + String("/") + int(textureHandles.length()));
}

void EditorScene::setFlag(unsigned int flag)
{
  flagViewer.setLogic(flag);
  brush.setLogic(flag);
}

void EditorScene::setLogicFlag(unsigned int flag)
{
  logicViewer.setLogic(flag);
  brush.setLogic(flag);
}

TSElementInfo* EditorScene::getTSElementInfo(const char* name)
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

void EditorScene::reset()
{
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glDepthMask(GL_TRUE);
  
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(1, 1);
  glPolygonMode(GL_FRONT, GL_FILL);
  
  resetTileTextureHandles();
  resetVisitors();
  resetCamera();
  resetFlags();
  resetGUI();
  resetMemoryStats();
  
  gui.forceUpdate(true);
}

void EditorScene::resetGUI()
{
  GUIComboBox* cbBox;
  GUIPanel* pPanel;
  GUISlider* pSlider;
  GUICheckBox* cBox;
  GUITabbedPanel* tPanel;
  Widgets wgs;
  
  ///reset tileset
  if (tilesetPanel)
    tilesetPanel->setVisible(false);
    
  if (memStatsPanel)
    memStatsPanel->setVisible(false);
    
  ///reset toolbar
  if (cbBox = (GUIComboBox*) gui.getWidgetByCallbackString("BrushTypesCB"))
    cbBox->setSelectedItem(0);
    
  if (cbBox = (GUIComboBox*) gui.getWidgetByCallbackString("TileTypesCB"))
    cbBox->setSelectedItem(0);
    
  if (cbBox = (GUIComboBox*) gui.getWidgetByCallbackString("TileLogicCB"))
    cbBox->setSelectedItem(0);
    
  if (cbBox = (GUIComboBox*) gui.getWidgetByCallbackString("TileOrientationCB"))
    cbBox->setSelectedItem(0);
    
  if (cbBox = (GUIComboBox*) gui.getWidgetByCallbackString("TileMirrorCB"))
    cbBox->setSelectedItem(0);
    
  ///layers tab
  if (pPanel = (GUIPanel*) gui.getWidgetByCallbackString("TileLayerSelection"))
  {
    wgs = pPanel->getWidgets();
    for (size_t t = 0; t < wgs.size(); t++)
      if (wgs[t]->getWidgetType() == RADIO_BUTTON)
        ((GUIRadioButton*)wgs[t])->setChecked(false);
  }
  
  if (pPanel = (GUIPanel*) gui.getWidgetByCallbackString("TileLayerVisibility"))
  {
    wgs = pPanel->getWidgets();
    for (size_t t = 0; t < wgs.size(); t++)
      if (wgs[t]->getWidgetType() == CHECK_BOX)
        ((GUICheckBox*)wgs[t])->setChecked(true);
  }
  
  if (cBox = (GUICheckBox*) gui.getWidgetByCallbackString("LayerBlendCheckbox"))
    cBox->setChecked(true);
    
  ///brush tab
  if (pPanel = (GUIPanel*) gui.getWidgetByCallbackString("BrushModes"))
  {
    wgs = pPanel->getWidgets();
    for (size_t t = 0; t < wgs.size(); t++)
      if (wgs[t]->getWidgetType() == RADIO_BUTTON)
        ((GUIRadioButton*)wgs[t])->setChecked(false);
  }
  
  if (pPanel = (GUIPanel*) gui.getWidgetByCallbackString("BrushTip"))
  {
    if (pSlider = (GUISlider*) pPanel->getWidgetByCallbackString("BrushRadius"))
    {
      pSlider->setProgress(0.20f);
      pSlider->setLabelString("Radius: 2.00");
    }
    
    if (pSlider = (GUISlider*) pPanel->getWidgetByCallbackString("BrushStrength"))
    {
      pSlider->setProgress(0.00f);
      pSlider->setLabelString("Strength: 0.00");
    }
    
    if (pSlider = (GUISlider*) pPanel->getWidgetByCallbackString("BrushOpacity"))
    {
      pSlider->setProgress(0.00f);
      pSlider->setLabelString("Opacity: 0.00");
    }
  }
  
  if (cBox = (GUICheckBox*) gui.getWidgetByCallbackString("PickingCheckbox"))
    cBox->setChecked(false);
    
  ///debug tab
  if (pPanel = (GUIPanel*) gui.getWidgetByCallbackString("DebugProperties"))
  {
    wgs = pPanel->getWidgets();
    for (size_t t = 0; t < wgs.size(); t++)
      if (wgs[t]->getWidgetType() == CHECK_BOX)
        ((GUICheckBox*)wgs[t])->setChecked(false);
        
    if (cBox = (GUICheckBox*) pPanel->getWidgetByCallbackString("TextureCheckbox"))
      cBox->setChecked(true);
  }
  
  ///config tabbed panels
  if (tPanel = (GUITabbedPanel*) gui.getWidgetByCallbackString("ToolsConfigTabs"))
  {
    const Widgets &widgets = tPanel->getLowerPanel()->getWidgets();
    const Widgets &buttons = tPanel->getUpperPanel()->getWidgets();
    int target = atoi("Layers");
    int count = int(widgets.size());
    for (int t = 0; t < count; t++)
    {
      widgets[t]->setVisible(t == target);
      ((GUIAlphaElement*)buttons[t])->setMinAlpha((t == target) ? 1.0f : 0.5f);
    }
  }
  
  if (tPanel = (GUITabbedPanel*) gui.getWidgetByCallbackString("GenConfigTabs"))
  {
    const Widgets &widgets = tPanel->getLowerPanel()->getWidgets();
    const Widgets &buttons = tPanel->getUpperPanel()->getWidgets();
    int target = atoi("Grass");
    int count = int(widgets.size());
    for (int t = 0; t < count; t++)
    {
      widgets[t]->setVisible(t == target);
      ((GUIAlphaElement*)buttons[t])->setMinAlpha((t == target) ? 1.0f : 0.5f);
    }
  }
  
  ///grass tab
  GUIPanel* parentPanel = (GUIPanel*) gui.getWidgetByCallbackString("PastureLayerList");
  
  if (pPanel = (GUIPanel*) gui.getWidgetByCallbackString("PastureLayerClear"))
  {
    pPanel->clear();
    pPanel->setParent(parentPanel);
  }
  
  if (pPanel = (GUIPanel*) gui.getWidgetByCallbackString("PastureLayerVisibility"))
  {
    pPanel->clear();
    pPanel->setParent(parentPanel);
  }
  
  if (pPanel = (GUIPanel*) gui.getWidgetByCallbackString("PastureLayerSelection"))
  {
    pPanel->clear();
    pPanel->setParent(parentPanel);
  }
  
  TerrainPasture* terrainPasture = Gateway::getTerrainPasture();
  for (unsigned int i = 0; i < terrainPasture->getMeadowCount(); i++)
    addPastureToCombobox(terrainPasture->getMeadow(i)->getName());
}
void EditorScene::resetTileTextureHandles()
{
  unsigned int txid;
  TerrainTextures* dsttextures = Gateway::getTerrainTextures();
  
  textureHandles.clear();
  currentIndex = 0;
  
  for (unsigned int i = 0; i < dsttextures->getTextureCount(); i++)
  {
    txid = dsttextures->getTextureID(i);
    
    if (!textureHandles.contains(txid))
      textureHandles.append(txid);
      
    if (i == 0)
      updateOffscreenTexture(&offscreenTexture);
  }
  
  if (tilesetLabel)
    tilesetLabel->setLabelString(String("Tileset ") + (currentIndex + 1) + String("/") + int(textureHandles.length()));
}

void EditorScene::resetVisitors()
{
  tileRevealer.reset();
  tileRevealer.setCamera(&camera);
  tileRevealer.setFrustum(&frustum);
  tileRevealer.setRadius(50.0f);
  
  renderer.reset();
  renderer.enableBlend(true);
  renderer.enableTexture(true);
  
  brush.reset();
  brush.setRay(&ray);
  brush.setNatureTransformGroup(natureGroup);
  brush.setCharacterTransformGroup(characterGroup);
  brush.setCritterTransformGroup(critterGroup);
  brush.setStructureTransformGroup(structureGroup);
  brush.setVillageTransformGroup(villageGroup);
  
  logicViewer.reset();
  flagViewer.reset();
}

void EditorScene::resetCamera()
{
  camera.setViewerPosition(Tuple3f(0,12,0));
  camera.setFocusPosition(Tuple3f(8,0,8));
  camera.setUpDirection(Tuple3f(0,1,0));
  camera.setTranslationSpeed(0.4f);
  camera.update(0);
  frustum.update();
}

void EditorScene::resetFlags()
{
  debugView = false;
  drawLogic = false;
  drawFlags = false;
  mouseLocked = false;
  pickEnabled = false;
  drawBase = true;
  drawTop = true;
  drawGrid = false;
  
  blendEnabled = true;
  
  paused = false;
  
  drawNature = true;
  drawStructures = true;
  drawVillages = true;
  drawCharacters = true;
  drawCritters = true;
  drawPasture = true;
  drawWater = true;
  
  drawCollMark = false;
}

void EditorScene::resetConfiguration()
{
  tileRevealer.setRadius(Gateway::getConfiguration().camRadius);
  camera.setTranslationSpeed(Gateway::getConfiguration().camSpeed);
  
  logicViewer.setRenderColor(Gateway::getConfiguration().logicColor);
  flagViewer.setRenderColor(Gateway::getConfiguration().flagColor);
  
  if (Gateway::getConfiguration().enableFog)
  {
    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE, GL_EXP2);
    glFogf(GL_FOG_DENSITY, Gateway::getConfiguration().fogDensity);
    fogColor = Gateway::getConfiguration().fogColor;
    glFogfv(GL_FOG_COLOR, fogColor);
    glHint(GL_FOG_HINT, GL_NICEST);
    
    glClearColor(fogColor.x, fogColor.y, fogColor.z, fogColor.w);
  }
  else
  {
    glDisable(GL_FOG);
    glClearColor(0,0,0,0);
    fogColor.set(0,0,0,1);
  }
}

void EditorScene::resetMemoryStats()
{
  pagefileMemoryMonitor.consume(pagefileMemoryMonitor.length());
  terrainMemoryMonitor.consume(terrainMemoryMonitor.length());
  geometryMemoryMonitor.consume(geometryMemoryMonitor.length());
  
  for (int i = 0; i < terrainMemoryMonitor.capacity(); i++)
    terrainMemoryMonitor.put(0.0f);
    
  for (int i = 0; i < geometryMemoryMonitor.capacity(); i++)
    geometryMemoryMonitor.put(0.0f);
    
  for (int i = 0; i < pagefileMemoryMonitor.capacity(); i++)
    pagefileMemoryMonitor.put(0.0f);
}

void EditorScene::updateMemoryMonitor(float frameInterval)
{
  if (!memStatsPanel->isVisible())
    return;
    
  if ((tickCounter += frameInterval) < 1.0f)
    return;
    
  Tuple4i viewport;
  MemoryProfile profile;
  Tuple2f dim;
  float x, y;
  GUILabel* l;
  int totalsec;
  
  totalsec = terrainMemoryMonitor.capacity();
  profile = Gateway::getMemoryProfile();
  dim = memstatsSurface->getDimensions();
  
  y = dim.y / (float)(profile.total);
  x = dim.x / (float)totalsec;
  
  glGetIntegerv(GL_VIEWPORT, viewport);
  glViewport(0, 0, (int)dim.x, (int)dim.y);
  Renderer::enter2DModeInverse((int)dim.x, (int)dim.y);
  glClearColor(0,0.01f,0,0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  ///draw page mem profile
  pagefileMemoryMonitor.get();
  pagefileMemoryMonitor.put(float(profile.used));
  glColor3f(0.0f,0.3f,0.3f);
  glBegin(GL_TRIANGLE_STRIP);
  for (int i = 0; i < totalsec; i++)
  {
    glVertex2f(x * i, 0);
    glVertex2f(x * i, y * pagefileMemoryMonitor.peek(i));
  }
  glEnd();
  glColor3f(0,1,1);
  glBegin(GL_LINES);
  for (int i = 0; i < totalsec-1; i++)
  {
    glVertex2f(x * i, y * pagefileMemoryMonitor.peek(i));
    glVertex2f(x * (i + 1), y * pagefileMemoryMonitor.peek(i + 1));
  }
  glEnd();
  
  ///draw terrain mem profile
  terrainMemoryMonitor.get();
  terrainMemoryMonitor.put(float(profile.usedterr));
  glColor3f(0.0f,0.3f,0.0f);
  glBegin(GL_TRIANGLE_STRIP);
  for (int i = 0; i < totalsec; i++)
  {
    glVertex2f(x * i, 0);
    glVertex2f(x * i, y * terrainMemoryMonitor.peek(i));
  }
  glEnd();
  glColor3f(0,1,0);
  glBegin(GL_LINES);
  for (int i = 0; i < totalsec-1; i++)
  {
    glVertex2f(x * i, y * terrainMemoryMonitor.peek(i));
    glVertex2f(x * (i + 1), y * terrainMemoryMonitor.peek(i + 1));
  }
  glEnd();
  
  ///draw geometry mem profile
  geometryMemoryMonitor.get();
  geometryMemoryMonitor.put(float(profile.usedgeo));
  glColor3f(0.3f,0.0f,0.0f);
  glBegin(GL_TRIANGLE_STRIP);
  for (int i = 0; i < totalsec; i++)
  {
    glVertex2f(x * i, 0);
    glVertex2f(x * i, y * geometryMemoryMonitor.peek(i));
  }
  glEnd();
  glColor3f(1,0,0);
  glBegin(GL_LINES);
  for (int i = 0; i < totalsec-1; i++)
  {
    glVertex2f(x * i, y * geometryMemoryMonitor.peek(i));
    glVertex2f(x * (i + 1), y * geometryMemoryMonitor.peek(i + 1));
  }
  glEnd();
  
  memstats.copyCurrentBuffer();
  
  Renderer::exit2DMode();
  
  glColor3f(1,1,1);
  glViewport(0, 0, viewport.z, viewport.w);
  glClearColor(fogColor.x, fogColor.y, fogColor.z, fogColor.w);
  
  l = (GUILabel*) gui.getWidgetByCallbackString("TotalMem");
  l->setLabelString(String("Total size: ") + (int)(profile.total / (1024*1024)) + "MB");
  l = (GUILabel*) gui.getWidgetByCallbackString("TerrainUsedMem");
  l->setLabelString(String("Used size (T): ") + (float)((terrainMemoryMonitor.peek(totalsec - 1) / profile.total) * 100.0f) + "%");
  l = (GUILabel*) gui.getWidgetByCallbackString("GeometryUsedMem");
  l->setLabelString(String("Used size (G): ") + (float)((geometryMemoryMonitor.peek(totalsec - 1) / profile.total) * 100.0f) + "%");
  l = (GUILabel*) gui.getWidgetByCallbackString("FilebufferUsedMem");
  l->setLabelString(String("Used size (F): ") + (float)((profile.usedfile / profile.total) * 100.0f) + "%");
  l = (GUILabel*) gui.getWidgetByCallbackString("PeakMem");
  l->setLabelString(String("Peak size: ") + (float)((profile.peak / profile.total) * 100.0f) + "%");
  
  tickCounter = 0.0f;
}

void EditorScene::updateMinimap()
{
  if (!minimapPanel)
    return;
    
  if (!gui.isVisible())
    return;
    
  if (!minimapPanel->isVisible())
    return;
    
  Tuple4i viewport;
  Tuple2i area;
  int max;
  Tuple3f cp;
  Tuple3f fp;
  Tuple3f l;
  Tuple2f b;
  
  glClearColor(0.3f,0,0,0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glGetIntegerv(GL_VIEWPORT, viewport);
  
  area = Gateway::getTerrainVisuals()->getArea();
  max = area.x > area.y ? area.x : area.y;
  
  cp = camera.getViewerPosition();
  fp = camera.getFocusPosition();
  l = (fp - cp).normalize() * 18;
  b.set(-l.x + l.z, l.x + l.z);
  
  Renderer::enter2DModeInverse(max, max);
  glViewport(0, 0, 256, 256);
  
  glColor4f(0.2f, 0.22f, 0.29f, 1.0f);
  glBegin(GL_TRIANGLE_FAN);
  glVertex2i(0, 0);
  glVertex2i(0, area.y);
  glVertex2i(area.x, area.y);
  glVertex2i(area.x, 0);
  glEnd();
  
  glLineWidth(2);
  glColor3f(1,1,1);
  glBegin(GL_LINE_LOOP);
  glVertex2f(cp.z, cp.x);
  glVertex2f(b.x + cp.z,  b.y + cp.x);
  glVertex2f(b.y + cp.z, -b.x + cp.x);
  glEnd();
  
  finalmap.copyCurrentBuffer();
  Renderer::exit2DMode();
  
  glLineWidth(1);
  glPointSize(1);
  glColor3f(1,1,1);
  glClearColor(fogColor.x, fogColor.y, fogColor.z, fogColor.w);
  
  glViewport(0, 0, viewport.z, viewport.w);
}

void EditorScene::updateOffscreenTexture(Texture* targetTexture)
{
  Tuple4i viewport;
  glGetIntegerv(GL_VIEWPORT, viewport);
  glViewport(0, 0, 256, 256);
  Renderer::enter2DMode(256, 256);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, textureHandles(currentIndex));
  glColor3f(1,1,1);
  drawFullScreenQuadInverse(256, 256);
  targetTexture->copyCurrentBuffer();
  glDisable(GL_TEXTURE_2D);
  Renderer::exit2DMode();
  glViewport(0, 0, viewport.z, viewport.w);
}

void EditorScene::addPastureToCombobox(const String& pastureName)
{
  if (grassIDX > 12)
    return;
    
  GUIPanel* pastureLayerPanel = (GUIPanel*) gui.getWidgetByCallbackString("PastureLayerList");
  
  GUIPanel* pastureClrPanel = (GUIPanel*) gui.getWidgetByCallbackString("PastureLayerClear");
  GUIPanel* pastureVisPanel = (GUIPanel*) gui.getWidgetByCallbackString("PastureLayerVisibility");
  GUIPanel* pastureSelPanel = (GUIPanel*) gui.getWidgetByCallbackString("PastureLayerSelection");
  
  GUIButton* btn = new GUIButton(String(pastureName) + "Clear");
  Texture btntexture;
  btntexture.load2D("GUIElements.png");
  btn->setColor(1,1,1);
  btn->setDimensions(32.0f, 27.0f);
  btn->setClipSize(0);
  btn->setTexture(btntexture);
  btn->setTextureRectangle(126, 98, 158, 72);
  btn->setVisibleBounds(false);
  pastureClrPanel->addWidget(btn);
  
  GUICheckBox* cb = new GUICheckBox(String(pastureName) + "Visibility");
  cb->setChecked(true);
  cb->setColor(1,1,1);
  pastureVisPanel->addWidget(cb);
  
  GUIRadioButton* rb = new GUIRadioButton(String(pastureName) + "Selection");
  rb->setMinAlpha(0.35f);
  rb->setColor(1,1,1);
  rb->setLabelString(pastureName);
  pastureSelPanel->addWidget(rb);
  
  gui.forceUpdate(true);
  
  grassIDX++;
}

void EditorScene::enableGUI(bool enable)
{
  Widgets wgs = gui.getWidgets();
  for (size_t t = 0; t < wgs.size(); t++)
    if (wgs[t]->getWidgetType() == PANEL && wgs[t]->getCallbackString() != "PopupMessagePanel")
      ((GUIPanel*)wgs[t])->setActive(enable);
}

EditorScene::~EditorScene()
{
}