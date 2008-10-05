#ifndef GATEWAY
#define GATEWAY

#include "../Factories/FactoryUtils.h"
#include "../Databases/DatabaseResources.h"

class SpatialIndex;
class TerrainTextures;
class TerrainVisuals;
class TerrainLogic;
class TerrainPasture;
class TerrainWater;
class WorldVisuals;
class ModelDatabase;
class TerrainDatabase;
class WaterDatabase;
class TransformGroup;
class Frustum;
class Dataset;
class Visitor;
class PageFile;
class BlockMemory;

struct CurrentInfoStructure
{
  GSElementInfo currentStructureElement;
  GSElementInfo currentNatureElement;
  GSElementInfo currentCharacterElement;
  GSElementInfo currentCritterElement;
  GSElementInfo currentVillageElement;
};

struct EditorConfiguration
{
  EditorConfiguration() :
      enableFog(false),
      fogDensity(0.0f),
      camSpeed(0.04f),
      camRadius(50.0f),
      infiniteView(0.0f),
      flagColor(1.0f, 0.0f, 0.0f, 0.3f),
      logicColor(0.0f, 0.0f, 1.0f, 0.3f),
      enableCursor(true)
  {}
  
  Tuple4f fogColor;
  Tuple4f flagColor;
  Tuple4f logicColor;
  float camSpeed;
  float camRadius;
  float infiniteView;
  float fogDensity;
  bool enableFog;
  bool enableCursor;
};

class Gateway
{
  public:
    static bool initialize();
    static void destroy();
    static void reset();
    
    ///loading
    static bool constructMap(const MapDescriptor& descriptor);
    static bool exportMap();
    
    ///dataset
    static TerrainTextures* getTerrainTextures();
    static TerrainVisuals* getTerrainVisuals();
    static TerrainLogic* getTerrainLogic();
    static TerrainPasture* getTerrainPasture();
    static TerrainWater* getTerrainWater();
    static WorldVisuals* getWorldVisuals();
    
    ///laboratories
    static void setActiveNatureElement(const GSElementInfo& element);
    static GSElementInfo* getActiveNatureInfo();
    static TransformGroup* getActiveNature();
    static void setActiveNature(TransformGroup* group);
    
    static void setActiveCharacterElement(const GSElementInfo& element);
    static GSElementInfo* getActiveCharacterInfo();
    static TransformGroup* getActiveCharacter();
    static void setActiveCharacter(TransformGroup* group);
    
    static void setActiveCritterElement(const GSElementInfo& element);
    static GSElementInfo* getActiveCritterInfo();
    static TransformGroup* getActiveCritter();
    static void setActiveCritter(TransformGroup* group);
    
    static void setActiveStructureElement(const GSElementInfo& element);
    static GSElementInfo* getActiveStructureInfo();
    static TransformGroup* getActiveStructure();
    static void setActiveStructure(TransformGroup* group);
    
    static void setActiveVillageElement(const GSElementInfo& element);
    static GSElementInfo* getActiveVillageInfo();
    static TransformGroup* getActiveVillage();
    static void setActiveVillage(TransformGroup* group);
    
    static TransformGroup* getNaturePrototype(const char* name);
    static TransformGroup* getCharacterPrototype(const char* name);
    static TransformGroup* getCritterPrototype(const char* name);
    static TransformGroup* getStructurePrototype(const char* name);
    static TransformGroup* getVillagePrototype(const char* name);
    
    ///databases
    static TerrainDatabase* getTerrainDatabase();
    static ModelDatabase* getNatureDatabase();
    static ModelDatabase* getCharacterDatabase();
    static ModelDatabase* getCritterDatabase();
    static ModelDatabase* getStructureDatabase();
    static ModelDatabase* getVillageDatabase();
    static WaterDatabase* getWaterDatabase();
    static bool findType(const char* objName, int& typeVal);
    
    ///terrain
    static void inspectTerrainGraph(unsigned int layer, Visitor* visitor);
    
    ///spatial index
    static SpatialIndex* getSpatialIndex();
    static void inspectSpatialIndex(Visitor* visitor);
    static void updateSpatialIndex(const Tuple3f &point, float radius);
    
    ///configuration
    static EditorConfiguration& getConfiguration();
    static MapDescriptor& getMapDescriptor();
    
    ///memory
    static void* aquireGeometryMemory(unsigned int bytes);
    static bool releaseGeometryMemory(void* addr);
    
    static void* aquireTerrainMemory(unsigned int bytes);
    static bool releaseTerrainMemory(void* addr);
    
    static void* aquirePageMemory(unsigned int bytes);
    static bool releasePageMemory(void* addr);
    
    static void* aquireFilebufferMemory(unsigned int bytes);
    static bool releaseFilebufferMemory(void* addr);
    
    static PageFile* getPageFile();
    
    static MemoryProfile getMemoryProfile();
    
    static void loadBackground(const char* path);
    static void destroyBackground();
    static void bindBackground();
    
    ///misc
    static void setExportPath(const char* path);
    static const char* getExportPath();
    
    static void setMenuMusicPath(const char* path);
    static const char* getMenuMusicPath();
    static void enableMenuMusic(bool enable);
    static bool isMenuMusicEnabled();
    static void update();
    
    static void setBrushMatrixDescriptor(const BrushMatrixDescriptor& descriptor);
    static BrushMatrixDescriptor& getBrushMatrixDescriptor();
    
  public:
    static Dataset* dataset;
    
    static TerrainDatabase* terrainDatabase;
    static ModelDatabase* natureDatabase;
    static ModelDatabase* structureDatabase;
    static ModelDatabase* characterDatabase;
    static ModelDatabase* critterDatabase;
    static ModelDatabase* villageDatabase;
    static WaterDatabase* waterDatabase;
    
    static SpatialIndex* spatialIndex;
    
    static CurrentInfoStructure currentInfoStructure;
    static EditorConfiguration editorConfig;
    static MapDescriptor mapDescriptor;
    
    static PageFile* pageFile;
    static BlockMemory* filebufferMemory;
    static BlockMemory* geometryMemory;
    static BlockMemory* terrainMemory;
    
    static Texture backgroundTexture;
    
    static TransformGroup* activeStructureGroup;
    static TransformGroup* activeNatureGroup;
    static TransformGroup* activeCharacterGroup;
    static TransformGroup* activeCritterGroup;
    static TransformGroup* activeVillageGroup;
    
    static String exportPath;
    static String menuMusicPath;
    static bool menuMusicEnabled;
    
    static BrushMatrixDescriptor brushMatrixDescriptor;
};

#endif