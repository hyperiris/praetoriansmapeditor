#include "TerrainBrush.h"
#include "../Kernel/Gateway.h"
#include "../Geometry/Ray3D.h"

#include "../Factories/TerrainVisuals.h"
#include "../Factories/TerrainPasture.h"
#include "../Factories/TerrainLogic.h"
#include "../Factories/WorldVisuals.h"
#include "../Factories/Meadow.h"
#include "../Factories/Water.h"

#include "../Managers/ManagersUtils.h"

#include "../Controllers/VillageModelController.h"
#include "../Controllers/TileController.h"

#include "../Databases/TerrainDatabase.h"
#include "../Databases/ModelDatabase.h"

#include "../Nodes/SpatialIndexNode.h"
#include "../Nodes/SpatialIndexCell.h"
#include "../Nodes/TransformGroup.h"

#include "../Tools/NodeIterator.h"
#include "../Tools/RangeIterator.h"

#include "../Strategies/BrushBitEStrategy.h"
#include "../Strategies/BrushBitDStrategy.h"

#include "../Containers/ocsort.h"

TerrainBrush::TerrainBrush()
{
  strategies.append(new BrushBitEStrategy());
  strategies.append(new BrushBitDStrategy());
  reset();
}

void TerrainBrush::visit(SpatialIndexBaseNode* node)
{
  pointAVLTree.clear();
  
  if (intersect(&node->getBoundsDescriptor()))
  {
    NodeIterator iter(node->getFirstChild());
    
    while (!iter.end())
    {
      iter.current()->accept(this);
      iter++;
    }
  }
  
  if (pointAVLTree.isEmpty())
    return;
    
  if (saveCollPoint)
  {
    AVLTreeTIterator <float, Tuple3f, 4> pointIter(pointAVLTree);
    ++pointIter;
    cpoint = pointIter.value();
    if (!enabled) return;
  }
  
  switch (type)
  {
    case BrushTypes::TILE:
      editTiles();
      break;
      
    case BrushTypes::GRASS:
      editGrass();
      break;
      
    case BrushTypes::WATER:
      editWater();
      break;
      
    case BrushTypes::MODEL:
      editModel();
      break;
      
    default:
      break;
  }
}

void TerrainBrush::visit(SpatialIndexNode* node)
{
  if (intersect(&node->getBoundsDescriptor()))
  {
    NodeIterator iter(node->getFirstChild());
    
    while (!iter.end())
    {
      iter.current()->accept(this);
      iter++;
    }
  }
}

void TerrainBrush::visit(SpatialIndexCell* node)
{
  BoundsDescriptor *bounds = &node->getBoundsDescriptor();
  TileController* controller;
  RangeIterator iter;
  Tuple4f packet;
  Tuple3f point;
  float distance;
  
  TerrainVisuals* visuals = Gateway::getTerrainVisuals();
  TerrainDatabase* tdatabase = Gateway::getTerrainDatabase();
  
  if (node->isVisible())
    if (intersect(bounds))
    {
      if (showGuides)
      {
        glColor3ub(255, 255, 255);
        bounds->render(BoundsDescriptor::AABB | BoundsDescriptor::WIRE);
      }
      
      if (enabled || saveCollPoint)
      {
        iter.set(visuals->getArea().x, node->getRange());
        
        while (!iter.end())
        {
          controller = tdatabase->getController(iter.current());
          packet = intersector.intersectTriangles(ray, controller->getVertices(), visuals->getTileIndices(0), 8);
          
          if (1 == packet.w)
          {
            point.set(packet.x, packet.y, packet.z);
            distance = point.getDistance(ray->getOrigin());
            
            if (2.0f < distance)
              pointAVLTree.insertKeyAndValue(distance, point);
          }
          
          iter++;
        }
      }
    }
}

void TerrainBrush::editModel()
{
  ModelDatabase *ndatabase, *sdatabase, *vdatabase, *cdatabase;
  WorldVisuals* wVisuals;
  //int ctrlindex;
  GSElementInfo* info;
  ModelController* controller;
  
  AVLTreeTIterator <float, Tuple3f, 4> pointIter(pointAVLTree);
  ++pointIter;
  
  cpoint = pointIter.value();
  
  switch (layer)
  {
    case BrushLayers::CHARACTER:
      if (constant)
      {
        info = Gateway::getActiveCharacterInfo();
        if (info->name)
        {
          cdatabase = Gateway::getCharacterDatabase();
          controller = cdatabase->instantiateModel(info->name);
          controller->translateModel(cpoint.x, cpoint.y + 0.05f, cpoint.z);
          Gateway::getMapDescriptor().playerPositions.append(Tuple2f(cpoint.x, cpoint.z));
        }
      }
      break;
      
    case BrushLayers::CRITTER:
      if (constant)
      {
        info = Gateway::getActiveCritterInfo();
        if (info->name)
        {
          cdatabase = Gateway::getCritterDatabase();
          controller = cdatabase->instantiateModel(info->name);
          
          float y = controller->getTransformGroup()->getBoundsDescriptor().getMinEndAABB().y;
          Tuple3f cen = controller->getTransformGroup()->getBoundsDescriptor().getCenterAABB();
          controller->translateModel(cpoint.x - cen.x, cpoint.y + 0.05f - y, cpoint.z - cen.z);
          
          wVisuals = Gateway::getWorldVisuals();
          WorldObject worldObject;
          worldObject.flags = 0;
          worldObject.name = info->name;
          worldObject.type = WorldObjectTypes::CRITTER;
          worldObject.orientation = 0.0f;
          worldObject.position.set(cpoint.x, cpoint.y + 0.05f, cpoint.z);
          worldObject.windFactor = 0.0f;
          wVisuals->addObject(worldObject);
        }
      }
      break;
      
    case BrushLayers::NATURE:
      if (constant)
      {
        info = Gateway::getActiveNatureInfo();
        if (info->name)
        {
          ndatabase = Gateway::getNatureDatabase();
          
          controller = ndatabase->instantiateModel(info->name);
          controller->translateModel(cpoint.x, cpoint.y + 0.05f, cpoint.z);
          ///cannot subtract y because that would cause a problem with canopy models
          
          wVisuals = Gateway::getWorldVisuals();
          WorldObject worldObject;
          worldObject.flags = 0;
          worldObject.name = info->name;
          worldObject.type = WorldObjectTypes::NATURE;
          worldObject.orientation = 0.79f;
          worldObject.position.set(cpoint.x, cpoint.y + 0.05f, cpoint.z);
          worldObject.windFactor = info->windFactor;
          wVisuals->addObject(worldObject);
        }
      }
      break;
      
    case BrushLayers::STRUCTURE:
      if (constant)
      {
        info = Gateway::getActiveStructureInfo();
        if (info->name)
        {
          sdatabase = Gateway::getStructureDatabase();
          controller = sdatabase->instantiateModel(info->name);
          //controller = sdatabase->getModelController(ctrlindex);
          
          float y = controller->getTransformGroup()->getBoundsDescriptor().getMinEndAABB().y;
          Tuple3f cen = controller->getTransformGroup()->getBoundsDescriptor().getCenterAABB();
          controller->translateModel(cpoint.x - cen.x, cpoint.y + 0.05f - y, cpoint.z - cen.z);
          
          wVisuals = Gateway::getWorldVisuals();
          WorldObject worldObject;
          worldObject.flags = 0;
          worldObject.name = info->name;
          worldObject.type = WorldObjectTypes::STRUCTURE;
          worldObject.orientation = 0.0f;
          worldObject.position.set(cpoint.x, cpoint.y + 0.05f, cpoint.z);
          worldObject.windFactor = 0.0f;
          wVisuals->addObject(worldObject);
        }
      }
      break;
      
    case BrushLayers::VILLAGE:
      if (constant)
      {
        info = Gateway::getActiveVillageInfo();
        if (info->name)
        {
          vdatabase = Gateway::getVillageDatabase();
          VillageModelController* villageController = (VillageModelController*) vdatabase->instantiateModel(info->name);
          villageController->setPopulation(info->population);
          villageController->setMaxPopulation(info->maxpopulation);
          float y = villageController->getTransformGroup()->getBoundsDescriptor().getMinEndAABB().y;
          Tuple3f cen = villageController->getTransformGroup()->getBoundsDescriptor().getCenterAABB();
          villageController->translateModel(cpoint.x - cen.x, cpoint.y + 0.05f - y, cpoint.z - cen.z);
        }
      }
      break;
      
    default:
      break;
  }
}

void TerrainBrush::editTiles()
{
  if (layer != BrushLayers::TILE)
    return;
    
  TileController *controller;
  Tuple3f *verts;
  Tuple4ub *cols;
  float s, y, d;
  unsigned int lflag;
  char col;
  RangeIterator tileiter;
  TerrainVisuals* visuals = Gateway::getTerrainVisuals();
  TerrainLogic* terrainLogic = Gateway::getTerrainLogic();
  TerrainDatabase* tdatabase = Gateway::getTerrainDatabase();
  AVLTreeTIterator <float, Tuple3f, 4> pointIter(pointAVLTree);
  
  if (mode == BrushModes::FILL)
  {
    if (constant)
      if (txcoordsIndex >= 0)
        for (unsigned int i = 0; i < tdatabase->getControllerCount(); i++)
          tdatabase->changeTileTexture(level, txcoordsIndex, i, !tdatabase->getController(i)->isVisible());
    return;
  }
  
  ++pointIter;
  
  cpoint = pointIter.value();
  Tuple2i area(visuals->getArea());
  unsigned int xOff = clamp(int(cpoint.x - radius), 0, area.y - 1);
  unsigned int yOff = clamp(int(cpoint.z - radius), 0, area.x - 1);
  unsigned int zOff = clamp(int(cpoint.x + radius), 0, area.y - 1);
  unsigned int wOff = clamp(int(cpoint.z + radius), 0, area.x - 1);
  tileiter.set(area.x, Tuple4i(yOff, xOff, wOff, zOff));
  
  glPointSize(2.0f);
  glColor3ub(255, 200, 0);
  glBegin(GL_POINTS);
  
  while (!tileiter.end())
  {
    controller = tdatabase->getController(tileiter.current());
    verts = controller->getVertices();
    cols = controller->getColors();
    
    switch (mode)
    {
    
      case BrushModes::ROTATE90:
      
        if (constant)
          controller->setFlags(TileFlags::TEXTURE_ROTATE90);
          
        break;
        
      case BrushModes::ROTATE180:
      
        if (constant)
          controller->setFlags(TileFlags::TEXTURE_ROTATE180);
          
        break;
        
      case BrushModes::ROTATE270:
      
        if (constant)
          controller->setFlags(TileFlags::TEXTURE_ROTATE270);
          
        break;
        
      case BrushModes::MIRRORX:
      
        if (constant)
          controller->setFlags(TileFlags::TEXTURE_MIRRORX);
          
        break;
        
      case BrushModes::MIRRORY:
      
        if (constant)
          controller->setFlags(TileFlags::TEXTURE_MIRRORY);
          
        break;
        
        //case BrushModes::VISIBLE:
        //
        //  if (constant)
        //    controller->setFlags(TileFlags::TILE_VISIBLE);
        //
        //  break;
        //
        //case BrushModes::QUAD0DIAGONAL:
        //
        //  if (constant)
        //    controller->setFlags(TileFlags::QUAD0_DIAGONAL);
        //
        //  break;
        //
        //case BrushModes::QUAD1DIAGONAL:
        //
        //  if (constant)
        //    controller->setFlags(TileFlags::QUAD1_DIAGONAL);
        //
        //  break;
        //
        //case BrushModes::QUAD2DIAGONAL:
        //
        //  if (constant)
        //    controller->setFlags(TileFlags::QUAD2_DIAGONAL);
        //
        //  break;
        //
        //case BrushModes::QUAD3DIAGONAL:
        //
        //  if (constant)
        //    controller->setFlags(TileFlags::QUAD3_DIAGONAL);
        //
        //  break;
        //
        //case BrushModes::DYNAMICMIX:
        //
        //  if (constant)
        //    controller->setFlags(TileFlags::DYNAMIC_MIX);
        //
        //  break;
        //
        //case BrushModes::NOTESSELATE:
        //
        //  if (constant)
        //    controller->setFlags(TileFlags::TILE_NOTESSELLATE);
        //
        //  break;
        
      case BrushModes::RAISE:
      
        for (unsigned int v = 0; v < 9; v++)
        {
          d = verts[v].getDistance(cpoint);
          
          if (d <= radius)
          {
            s = -(d * d) / radius;
            verts[v].y += time * strength * 6 * exp(s);
            glVertex3f(verts[v].x, verts[v].y + 0.1f, verts[v].z);
          }
        }
        
        break;
        
      case BrushModes::LOWER:
      
        for (unsigned int v = 0; v < 9; v++)
        {
          d = verts[v].getDistance(cpoint);
          
          if (d <= radius)
          {
            s = -(d * d) / radius;
            verts[v].y += time * -strength * 6 * exp(s);
            glVertex3f(verts[v].x, verts[v].y + 0.1f, verts[v].z);
          }
        }
        
        break;
        
      case BrushModes::ERASE:
      
        for (unsigned int c = 0; c < 9; c++)
        {
          d = verts[c].getDistance(cpoint);
          
          if (d <= radius)
          {
            s = -(d * d) / radius;
            y = time * strength * 3 * exp(s);
            col = (char)(y * 255);
            cols[c].w -= cols[c].w <= col ? 0 : col;
            glVertex3f(verts[c].x, verts[c].y + 0.1f, verts[c].z);
          }
        }
        
        
        break;
        
      case BrushModes::RESTORE:
      
        for (unsigned int c = 0; c < 9; c++)
        {
          d = verts[c].getDistance(cpoint);
          
          if (d <= radius)
          {
            s = -(d * d) / radius;
            y = time * strength * 3 * exp(s);
            cols[c].w += (char)((255 - cols[c].w) * y);
            glVertex3f(verts[c].x, verts[c].y + 0.1f, verts[c].z);
          }
        }
        
        break;
        
      case BrushModes::BURN:
      
        for (unsigned int c = 0; c < 9; c++)
        {
          d = verts[c].getDistance(cpoint);
          
          if (d <= radius)
          {
            s = -(d * d) / radius;
            y = time * strength * 2 * exp(s);
            col = (char)(y * 255);
            cols[c].x -= cols[c].x < col ? 0 : col;
            cols[c].y -= cols[c].y < col ? 0 : col;
            cols[c].z -= cols[c].z < col ? 0 : col;
            glVertex3f(verts[c].x, verts[c].y + 0.1f, verts[c].z);
          }
        }
        
        break;
        
      case BrushModes::HEAL:
      
        for (unsigned int c = 0; c < 9; c++)
        {
          d = verts[c].getDistance(cpoint);
          
          if (d <= radius)
          {
            s = -(d * d) / radius;
            y = time * strength * 2 * exp(s);
            cols[c].x += (char) round((255 - cols[c].x) * y);
            cols[c].y += (char) round((255 - cols[c].y) * y);
            cols[c].z += (char) round((255 - cols[c].z) * y);
            glVertex3f(verts[c].x, verts[c].y + 0.1f, verts[c].z);
          }
        }
        
        break;
        
      case BrushModes::PAINT:
      
        if (verts[4].getDistance(cpoint) <= radius)
          tdatabase->changeTileTexture(level, txcoordsIndex, tileiter.current());
          
        break;
        
      case BrushModes::MARKER:
      
        for (unsigned int c = 0; c < 9; c++)
        {
          d = verts[c].getDistance(cpoint);
          
          if (d <= radius)
          {
            y = time * opacity;
            col = (char)(y * 255);
            cols[c].x -= cols[c].x <= col ? 0 : col;
            cols[c].y -= cols[c].y <= col ? 0 : col;
            cols[c].z -= cols[c].z <= col ? 0 : col;
            glVertex3f(verts[c].x, verts[c].y + 0.1f, verts[c].z);
          }
        }
        
        break;
        
      case BrushModes::PASTEL:
      
        for (unsigned int c = 0; c < 9; c++)
        {
          d = verts[c].getDistance(cpoint);
          
          if (d <= radius)
          {
            cols[c].x = char(255 * opacity);
            cols[c].y = char(255 * opacity);
            cols[c].z = char(255 * opacity);
            glVertex3f(verts[c].x, verts[c].y + 0.1f, verts[c].z);
          }
        }
        
        break;
        
      case BrushModes::ERASETILE:
      
        if (verts[4].getDistance(cpoint) <= radius)
          tdatabase->changeTileTexture(level, -1, tileiter.current());
          
        break;
        
      case BrushModes::LOGIC:
      
        if (logicflag != TileTypes::GRASSFIELD)
          if (verts[4].getDistance(cpoint) <= radius)
          {
            lflag = terrainLogic->getFlags(tileiter.current());
            terrainLogic->setFlags(tileiter.current(), strategy->getType((lflag & MASK_FLAGS), logicflag));
          }
          
        break;
        
      case BrushModes::FLAG:
      
        if (verts[4].getDistance(cpoint) <= radius)
        {
          lflag = terrainLogic->getFlags(tileiter.current());
          terrainLogic->setFlags(tileiter.current(), strategy->getFlags(lflag, logicflag));
        }
        
        break;
        
      case BrushModes::ADVANCED:
      
        if (verts[4].getDistance(cpoint) <= radius)
        {
          BrushMatrixDescriptor* descriptor = &Gateway::getBrushMatrixDescriptor();
          if (!descriptor->tileList.isEmpty())
          {
            int col = tileiter.current() - int(tileiter.current()/area.y) * area.y;
            int row = int(tileiter.current()/area.y);
            int mx = col % descriptor->dimensions.x;
            int my = row % descriptor->dimensions.y;
            Tile *mtile = &descriptor->tileList(my * descriptor->dimensions.x + mx);
            tdatabase->changeTileTexture(level, mtile->textureID[0], tileiter.current());
            controller->clearFlags();
            controller->setFlags(mtile->flags);
          }
        }
        
        break;
        
      case BrushModes::POINTER:
        break;
        
      default:
        break;
    }
    
    tileiter++;
  }
  
  glEnd();
  glPointSize(1.0f);
  
  if (mode < BrushModes::PAINT)
    Gateway::updateSpatialIndex(cpoint, radius);
}

void TerrainBrush::editGrass()
{
  if (layer != BrushLayers::GRASS && mode != BrushModes::LOGIC && type != BrushTypes::GRASS)
    return;
    
  if (meadowname.isBlank())
    return;
    
  Tuple3f *verts;
  RangeIterator tileiter;
  GrassPatch* patch;
  Meadow* meadow;
  int meadowindex;
  unsigned int lflag;
  
  TerrainVisuals* visuals = Gateway::getTerrainVisuals();
  TerrainDatabase* tdatabase = Gateway::getTerrainDatabase();
  TerrainPasture* terrainPasture = Gateway::getTerrainPasture();
  TerrainLogic* terrainLogic = Gateway::getTerrainLogic();
  
  AVLTreeTIterator <float, Tuple3f, 4> pointIter(pointAVLTree);
  ++pointIter;
  
  cpoint = pointIter.value();
  Tuple2i area(visuals->getArea());
  unsigned int xOff = clamp(int(cpoint.x - radius), 0, area.y - 1);
  unsigned int yOff = clamp(int(cpoint.z - radius), 0, area.x - 1);
  unsigned int zOff = clamp(int(cpoint.x + radius), 0, area.y - 1);
  unsigned int wOff = clamp(int(cpoint.z + radius), 0, area.x - 1);
  tileiter.set(area.x, Tuple4i(yOff, xOff, wOff, zOff));
  
  while (!tileiter.end())
  {
    verts = tdatabase->getController(tileiter.current())->getVertices();
    
    if (verts[4].getDistance(cpoint) <= radius)
    {
      meadowindex = terrainPasture->getMeadowIndex(meadowname);
      meadow = terrainPasture->getMeadow(meadowindex);
      if (!meadow->containsTileIndex(tileiter.current()))
      {
        if (patch = terrainPasture->createPatch(meadowindex))
        {
          patch->position.set(tileiter.current() - int(tileiter.current()/area.y) * area.y, int(tileiter.current()/area.y));
          patch->range.set(0x00000000,0x00000000);
          patch->color.set(0xff);
        }
        lflag = terrainLogic->getFlags(tileiter.current());
        terrainLogic->setFlags(tileiter.current(), (lflag & MASK_FLAGS) | (TileTypes::GRASSFIELD << 27));
        //terrainLogic->setFlags(tileiter.current(), strategy->getType((lflag & MASK_FLAGS), logicflag));
        meadow->addTileIndex(tileiter.current());
      }
    }
    
    tileiter++;
  }
}

bool TerrainBrush::intersect(BoundsDescriptor *bounds)
{
  Tuple3f ro;
  Tuple3f rd;
  Tuple3f d;
  Tuple3f a;
  Tuple3f e;
  Tuple3f c;
  
  ro = ray->getOrigin();
  rd = ray->getDestination();
  
  d = (rd - ro) * 0.5f;
  a.set(fabsf(d.x), fabsf(d.y), fabsf(d.z));
  
  e = bounds->getExtents();
  c = ro + d - bounds->getCenterAABB();
  
  if (fabsf(c.x) > e.x + a.x || fabsf(c.y) > e.y + a.y || fabsf(c.z) > e.z + a.z)
    return false;
    
  if (fabsf(d.y * c.z - d.z * c.y) > (e.y * a.z + e.z * a.y) ||
      fabsf(d.z * c.x - d.x * c.z) > (e.z * a.x + e.x * a.z) ||
      fabsf(d.x * c.y - d.y * c.x) > (e.x * a.y + e.y * a.x))
    return false;
    
  return true;
}

void TerrainBrush::editWater()
{
  if (layer != BrushLayers::WATER)
    return;
    
  if (!waterModel)
    return;
    
  Tuple3f point;
  float distance;
  Tuple4f packet;
  Array < DistanceObject<unsigned int> > distObjs;
  DistanceObject <unsigned int> object;
  AVLTreeTIterator <float, Tuple3f, 4> pointIter(pointAVLTree);
  
  ++pointIter;
  
  cpoint = pointIter.value();
  
  switch (mode)
  {
    case BrushModes::VERTEX:
    {
      if (constant)
        waterModel->addVertex(Tuple3f(cpoint.x, cpoint.y + 0.05f, cpoint.z));
    }
    break;
    
    case BrushModes::TRIANGLE:
    {
      if (constant)
      {
        for (unsigned int i = 0; i < waterModel->getTriangleCount(); i++)
        {
          packet = intersector.intersectTriangles(ray, waterModel->getVertices(), &waterModel->getTriangles()[i], 1);
          if (1 == packet.w)
          {
            point.set(packet.x, packet.y, packet.z);
            distance = point.getDistance(ray->getOrigin());
            
            if (2.0f < distance)
            {
              object.set(distance, i);
              distObjs.append(object);
            }
          }
        }
      }
      
      if (!distObjs.isEmpty())
      {
        OCQuickSort(distObjs, 0, distObjs.length());
        waterModel->removeTriangle(distObjs(0).getObject());
      }
      break;
    }
  }
}

void TerrainBrush::setRay(Ray3D* r)
{
  ray = r;
}

void TerrainBrush::setPaintIndex(int index)
{
  txcoordsIndex = index;
}

void TerrainBrush::setPaintLayer(int layer)
{
  level = layer;
}

void TerrainBrush::enable(bool e)
{
  enabled = e;
}

void TerrainBrush::enableConstant(bool enable)
{
  constant = enable;
}

void TerrainBrush::setRadius(float r)
{
  radius = r;
}

void TerrainBrush::update(float tick)
{
  time = tick;
}

void TerrainBrush::setStrength(float s)
{
  strength = s;
}

void TerrainBrush::setOpacity(float o)
{
  opacity = o;
}

void TerrainBrush::setLogic(unsigned int flags)
{
  logicflag = flags;
}

void TerrainBrush::setMode(int m)
{
  mode = m;
}

unsigned int TerrainBrush::getMode()
{
  return mode;
}

void TerrainBrush::setType(int m)
{
  type = m;
}

unsigned int TerrainBrush::getType()
{
  return type;
}

void TerrainBrush::setLayer(int m)
{
  layer = m;
}

int TerrainBrush::getLayer()
{
  return layer;
}

void TerrainBrush::enableGuides(bool enable)
{
  showGuides = enable;
}

void TerrainBrush::setMeadowName(const char* name)
{
  meadowname = name;
}

void TerrainBrush::setNatureTransformGroup(TransformGroup* group)
{
  natureRoot = group;
}

void TerrainBrush::setCharacterTransformGroup(TransformGroup* group)
{
  characterRoot = group;
}

void TerrainBrush::setCritterTransformGroup(TransformGroup* group)
{
  critterRoot = group;
}

void TerrainBrush::setStructureTransformGroup(TransformGroup* group)
{
  structureRoot = group;
}

void TerrainBrush::setVillageTransformGroup(TransformGroup* group)
{
  villageRoot = group;
}

Tuple4f TerrainBrush::getCollisionPoint()
{
  if (pointAVLTree.isEmpty())
    return Tuple4f(0,0,0,0);
    
  return Tuple4f(cpoint.x, cpoint.y, cpoint.z, 1);
}

void TerrainBrush::saveCollisionPoint(bool save)
{
  saveCollPoint = save;
}

void TerrainBrush::drawCollisionMark()
{
}

void TerrainBrush::enableLogicFlag(bool enable)
{
  if (enable)
    strategy = strategies(0);
  else
    strategy = strategies(1);
}

void TerrainBrush::setWaterModel(Water* water)
{
  waterModel = water;
}

void TerrainBrush::reset()
{
  natureRoot = 0;
  characterRoot = 0;
  structureRoot = 0;
  critterRoot = 0;
  villageRoot = 0;
  
  waterModel = 0;
  
  showGuides = false;
  enabled = false;
  constant = false;
  
  txcoordsIndex = -1;
  level = 0;
  
  meadowname.clear();
  
  radius = 2.0f;
  strength = 0.0f;
  time = 0.0f;
  opacity = 0.0f;
  
  saveCollPoint = false;
  
  logicflag = TileLogic::FLAG_NONE | (TileTypes::UNUSED << 27);
  mode = BrushModes::POINTER;
  type = BrushTypes::NONE;
  layer = BrushLayers::NONE;
  
  strategy = strategies(0);
}

TerrainBrush::~TerrainBrush()
{
  strategies.clear();
}