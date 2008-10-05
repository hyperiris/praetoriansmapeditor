#include "SpatialIndex.h"
#include "../Kernel/Gateway.h"
#include "../Nodes/SpatialIndexCell.h"
#include "../Nodes/SpatialIndexNode.h"
#include "../Tools/TemplatedNodeIterator.h"
#include "../Visitors/Visitor.h"
#include "../Factories/TerrainVisuals.h"
#include <stack>
using namespace std;

template <class T> int ceilPowerOfTwo(T x)
{
  return (int)pow((T)2, ceil(log10(x) / log10((T)2)));
}

template <class T> int ceilLogBaseTwo(T x)
{
  return (int)(ceil(log10(x) / log10(2.0f)));
}

SpatialIndex::SpatialIndex()
{
  cellSize = 8;
  maxBranchSize = 4;
}

void SpatialIndex::inspect(Visitor* visitor)
{
  //baseNode.accept(visitor);
  visitor->visit(&baseNode);
}

void SpatialIndex::compute()
{
  setupCells();
  computeCellBoundaries();
  setupSpatialTree();
  computeTreeBoundaries();
}

void SpatialIndex::setupCells()
{
  SpatialIndexCell *cell;
  Tuple4i tilerange;
  Tuple2i dimensions;
  TerrainVisuals* visuals;
  
  visuals = Gateway::getTerrainVisuals();
  dimensions = visuals->getArea();
  
  area.x = (dimensions.x / cellSize) + clamp(int(dimensions.x % cellSize), 0, 1);
  area.y = (dimensions.y / cellSize) + clamp(int(dimensions.y % cellSize), 0, 1);
  
  for (int y = 0; y < area.y; y++)
    for (int x = 0; x < area.x; x++)
    {
      tilerange.x = cellSize * x;
      tilerange.y = cellSize * y;
      tilerange.z = clamp(int(tilerange.x + (cellSize - 1)), 0, dimensions.x - 1);
      tilerange.w = clamp(int(tilerange.y + (cellSize - 1)), 0, dimensions.y - 1);
      
      cell = new SpatialIndexCell();
      cell->setRange(tilerange);
      cells.append(cell);
    }
}

void SpatialIndex::computeCellBoundaries()
{
  Tuple2i dimensions;
  Tuple2f depth;
  Tuple4i subrange;
  Tuple3f minEnd;
  Tuple3f maxEnd;
  Tuple3f *vertices;
  unsigned int index;
  float elevation;
  BoundsDescriptor descriptor;
  TerrainVisuals* visuals;
  
  visuals = Gateway::getTerrainVisuals();
  dimensions = visuals->getArea();
  
  for (unsigned int i = 0; i < cells.length(); i++)
  {
    subrange = cells(i)->getRange();
    
    minEnd = visuals->getTileMinEnd(subrange.y * dimensions.x + subrange.x);
    maxEnd = visuals->getTileMaxEnd(subrange.w * dimensions.x + subrange.z);
    minEnd.y =  1e3;
    maxEnd.y = -1e3;
    
    for (int y = 0; y < (subrange.w - subrange.y) + 1; y++)
      for (int x = 0; x < (subrange.z - subrange.x) + 1; x++)
      {
        index = (subrange.y + y) * dimensions.x + subrange.x + x;
        vertices = visuals->getTileVertices(index);
        depth.set(-100, 100);
        
        for (int j = 0; j < 9; j++)
        {
          elevation = vertices[j].y;
          depth.x = depth.x > elevation ? depth.x : elevation;
          depth.y = depth.y > elevation ? elevation : depth.y;
        }
        
        maxEnd.y = depth.x > maxEnd.y ? depth.x : maxEnd.y;
        minEnd.y = depth.y > minEnd.y ? minEnd.y : depth.y;
      }
      
    descriptor.computeBounds(minEnd, maxEnd);
    cells(i)->setBoundsDescriptor(descriptor);
  }
}

void SpatialIndex::setupSpatialTree()
{
  SpatialIndexBaseNode *pointer;
  SpatialIndexBaseNode *branch;
  SpatialIndexBaseNode *cell;
  unsigned int logarithm;
  unsigned int boundary;
  unsigned int level;
  unsigned int power;
  unsigned int minima;
  int offset;
  Tuple4i branchRange;
  Tuple4i terrainRange;
  Tuple4i point;
  BoundsDescriptor descriptor;
  stack <SpatialIndexBaseNode*> spatialtree;
  
  terrainRange.set(0, 0, area.x - 1, area.y - 1);
  
  descriptor.computeBounds(Tuple3f(1e3, 1e3, 1e3), Tuple3f(-1e3, -1e3, -1e3));
  
  for (int y = terrainRange.y; y <= terrainRange.w; y++)
    for (int x = terrainRange.x; x <= terrainRange.z; x++)
      descriptor += cells(y * area.x + x)->getBoundsDescriptor();
      
  baseNode.setRange(terrainRange);
  baseNode.updateBounds(descriptor);
  spatialtree.push(&baseNode);
  boundary  = terrainRange.z > terrainRange.w ? terrainRange.z : terrainRange.w;
  
  power = ceilLogBaseTwo((float)boundary + 1);
  logarithm = ceilPowerOfTwo((float)boundary + 1);
  minima = ceilLogBaseTwo((float)maxBranchSize);
  
  while (!spatialtree.empty())
  {
    pointer = spatialtree.top();
    level = pointer->getLevel();
    branchRange = pointer->getRange();
    
    spatialtree.pop();
    
    if (level < power - minima)
    {
      offset = logarithm >> (level + 1);
      
      for (unsigned int i = 0; i < 4; i++)
      {
        point.y = branchRange.y + (i > 1 ? offset : 0);
        point.x = branchRange.x + (i & 1 ? offset : 0);
        
        if (point.x > terrainRange.z || point.y > terrainRange.w)
          continue;
          
        point.z = clamp(point.x + offset - 1, point.x, terrainRange.z);
        point.w = clamp(point.y + offset - 1, point.y, terrainRange.w);
        
        branch = new SpatialIndexNode();
        branch->setLevel(level + 1);
        branch->setRange(point);
        branch->attach(pointer);
        
        spatialtree.push(branch);
        nodes.append(branch);
      }
    }
    else
      for (int y = branchRange.y; y <= branchRange.w; y++)
        for (int x = branchRange.x; x <= branchRange.z; x++)
        {
          cell = cells(y * area.x + x);
          cell->setLevel(level + 1);
          cell->attach(pointer);
        }
  }
}

void SpatialIndex::computeTreeBoundaries()
{
  stack <SpatialIndexBaseNode*> spatialtree;
  TemplatedNodeIterator <SpatialIndexBaseNode> iter;
  SpatialIndexBaseNode *pbranch;
  BoundsDescriptor descriptor;
  Tuple4i terrainrange;
  unsigned int boundary;
  unsigned int minima;
  unsigned int power;
  SpatialIndexBaseNode* piter = 0;
  
  terrainrange.set(0, 0, area.x - 1, area.y - 1);
  boundary = terrainrange.z > terrainrange.w ? terrainrange.z : terrainrange.w;
  power = ceilLogBaseTwo((float)boundary + 1);
  minima = ceilLogBaseTwo((float)maxBranchSize);
  spatialtree.push(&baseNode);
  
  while (!spatialtree.empty())
  {
    pbranch = spatialtree.top();
    spatialtree.pop();
    
    if (pbranch->getLevel() > power - minima)
      continue;
      
    descriptor = getRangeBoundaries(pbranch->getRange());
    pbranch->updateBounds(descriptor);
    
    iter.setNode((SpatialIndexBaseNode*)pbranch->getFirstChild());
    while (!iter.end())
    {
      piter = iter.current();
      spatialtree.push(piter);
      iter++;
    }
  }
}

BoundsDescriptor SpatialIndex::getRangeBoundaries(const Tuple4i &range)
{
  BoundsDescriptor descriptor;
  SpatialIndexBaseNode *cell;
  float max = 1e4;
  
  descriptor.computeBounds(Tuple3f(max, max, max), Tuple3f(-max, -max, -max));
  
  for (int y = range.y; y <= range.w; y++)
    for (int x = range.x; x <= range.z; x++)
    {
      cell = cells(y * area.x + x);
      descriptor += cell->getBoundsDescriptor();
    }
    
  return descriptor;
}

Tuple2i SpatialIndex::getArea()
{
  return area;
}

SpatialIndexBaseNode* SpatialIndex::getCell(unsigned int index)
{
  return cells(index);
}

void SpatialIndex::setBranchSize(unsigned int size)
{
  maxBranchSize = size;
}

void SpatialIndex::setCellSize(unsigned int size)
{
  cellSize = size;
}

unsigned int SpatialIndex::getBranchSize()
{
  return maxBranchSize;
}

unsigned int SpatialIndex::getCellSize()
{
  return cellSize;
}

void SpatialIndex::reset()
{
  baseNode.reset();
  nodes.clearAndDestroy();
  cells.clearAndDestroy();
  area.set(0, 0);
}

SpatialIndex::~SpatialIndex()
{
  reset();
}
