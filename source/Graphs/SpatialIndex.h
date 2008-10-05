#ifndef SPATIALINDEX
#define SPATIALINDEX

#include "../Nodes/SpatialIndexBaseNode.h"
//#include "../Math/MathUtils.h"
#include "../Containers/ocarray.h"

class SpatialIndexCell;
class SpatialIndexNode;
class BoundsDescriptor;
class Visitor;

class SpatialIndex
{
  public:
    SpatialIndex();
    ~SpatialIndex();
    
    void inspect(Visitor* visitor);
    void compute();
    void reset();
    
    void setBranchSize(unsigned int size);
    void setCellSize(unsigned int size);
    unsigned int getBranchSize();
    unsigned int getCellSize();
    Tuple2i getArea();
    
    SpatialIndexBaseNode* getCell(unsigned int index);
    
  private:
    void setupCells();
    void setupSpatialTree();
    void computeCellBoundaries();
    void computeTreeBoundaries();
    BoundsDescriptor getRangeBoundaries(const Tuple4i &range);
    
  private:
    unsigned int cellSize;
    unsigned int maxBranchSize;
    Tuple2i area;
    
    SpatialIndexBaseNode baseNode;
    
    ArrayPtr <SpatialIndexBaseNode> cells;
    ArrayPtr <SpatialIndexBaseNode> nodes;
};

#endif