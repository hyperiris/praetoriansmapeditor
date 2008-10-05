#ifndef MEADOW
#define MEADOW

#include "../Math/Tuple4.h"
#include "../Containers/ocarray.h"
#include "../Tools/NamedObject.h"

class TransformGroup;

class GrassPatch
{
  public:
    Tuple2i   position;
    Tuple2i   range;
    Tuple3ub  color;
};

class Meadow : public NamedObject
{
  public:
    Meadow(const char* strname = "PRADERA");
    ~Meadow();
    
  public:
    void setBounds(const Tuple4i& bounds);
    Tuple4i getBounds();
    
    void addPatch(GrassPatch* patch);
    GrassPatch* getPatch(unsigned int index);
    unsigned int getPatchCount();
    
    void addTileIndex(unsigned int index);
    bool containsTileIndex(unsigned int index);
    
    void updateBounds();
    
    void clearPatches();
    
  private:
    Array <unsigned int> tileIdentifierList;
    ArrayPtr <GrassPatch> patchesList;
    Tuple4i boundary;
};

#endif