#ifndef FACTORYUTILS
#define FACTORYUTILS

#include "../Math/Tuple4.h"
#include "MapDescriptor.h"
#include "Tile.h"

namespace WorldObjectTypes
{
  enum Enum
  {
    UNUSED    = 0x00,
    NATURE    = 0x01,
    STRUCTURE = 0x02,
    CRITTER   = 0x04
  };
}

struct MemoryProfile
{
  MemoryProfile() : total(0), used(0) {}
  unsigned int total;
  unsigned int used;
  unsigned int peak;
  
  unsigned int totalgeo;
  unsigned int usedgeo;
  unsigned int totalterr;
  unsigned int usedterr;
  unsigned int totalfile;
  unsigned int usedfile;
};

struct TileIndexSet
{
  TileIndexSet()
  {
    memset(indices, 0, 9);
  }
  unsigned int indices[9];
};

struct WorldObject
{
  unsigned int flags;
  unsigned int type;
  float windFactor;
  float orientation;
  Tuple3f position;
  String name;
};

//------------
///world object flags (used by mob)
//------------
namespace WorldObjectFlags
{
  enum Enum
  {
    NO_SHADOW = 0x0001,
    LARGE     = 0x0002
  };
}

//------------
///tile flags (used by pve)
//------------
namespace TileFlags
{
  enum Enum
  {
    QUAD0_DIAGONAL    = 0x0001,
    QUAD1_DIAGONAL    = 0x0002,
    QUAD2_DIAGONAL    = 0x0004,
    QUAD3_DIAGONAL    = 0x0008,
    TEXTURE_ROTATE90  = 0x0010,
    TEXTURE_ROTATE180 = 0x0020,
    TEXTURE_ROTATE270 = 0x0040,
    TEXTURE_MIRRORX   = 0x0080,
    TEXTURE_MIRRORY   = 0x0100,
    DYNAMIC_MIX       = 0x0200,
    TILE_NOTESSELLATE = 0x0400,
    TILE_FLAT         = 0x0800,
    TILE_VISIBLE      = 0x8000
  };
}

//------------
///tile logic (used by mgl)
//------------
const unsigned int MASK_FLAGS       = 0x07ffffff;
const unsigned int MASK_TERRAINTYPE = 0xf8000000;

namespace TileLogic
{
  enum Enum
  {
    FLAG_NONE               = 0x00000000,
    FLAG_NOPASSABLE         = 0x00000001,
    FLAG_FLAT               = 0x00000010,// [0º,10º)
    FLAG_TILT1              = 0x00000020,// [10º,30º)
    FLAG_TILT2              = 0x00000040,// [30º,50º)
    FLAG_TILT3              = 0x00000080,// >= 50º
    FLAG_STRUCTURE          = 0x00000100,
    FLAG_BRIDGE             = 0x00000200,
    FLAG_MODIFIED           = 0x00000400,
    FLAG_DOOR               = 0x00000800,
    FLAG_NO_SIEGE           = 0x00001000,
    FLAG_BUILDABLE          = 0x00002000,
    FLAG_WALL_OPEN          = 0x00004000,
    FLAG_WALL_BLOCKED       = 0x00008000,
    FLAG_FORTRESS           = 0x00010000,
    FLAG_BLOCKED_BUILD      = 0x00020000,
    FLAG_CALCULO_IA         = 0x00040000,
    FLAG_FLAT_STRUCTURE     = 0x00080000,
    FLAG_SCENERY_END        = 0x00100000,
    FLAG_WALL_GROUND        = 0x00200000,
    FLAG_BLOCK_WAR_MACHINES = 0x00400000
  };
}

//------------
///tile type (used by mgl)
//------------
namespace TileTypes
{
  enum Enum
  {
    NORMAL,
    SAND,
    DEEP_WATER,
    SHORE,
    GRASSFIELD,
    LIGHT_TREES,
    FOREST,
    WALL,
    BLOCKED,
    SNOW,
    UNUSED,
    SCENERY_END
  };
}

namespace BrushModes
{
  enum Enum
  {
    RAISE,
    LOWER,
    PAINT,
    FILL,
    ERASE,
    RESTORE,
    BURN,
    HEAL,
    MARKER,
    PASTEL,
    ERASETILE,
    ROTATE90,
    ROTATE180,
    ROTATE270,
    MIRRORX,
    MIRRORY,
    VISIBLE,
    QUAD0DIAGONAL,
    QUAD1DIAGONAL,
    QUAD2DIAGONAL,
    QUAD3DIAGONAL,
    DYNAMICMIX,
    NOTESSELATE,
    LOGIC,
    FLAG,
    POINTER,
    VERTEX,
    TRIANGLE,
    ADVANCED
  };
}

namespace BrushTypes
{
  enum Enum
  {
    TILE,
    GRASS,
    WATER,
    MODEL,
    NONE
  };
}

namespace BrushLayers
{
  enum Enum
  {
    TILE,
    NATURE,
    STRUCTURE,
    CHARACTER,
    CRITTER,
    VILLAGE,
    GRASS,
    WATER,
    NONE
  };
}

struct BrushMatrixDescriptor
{
public:
  Array <Tile> tileList;
  Tuple2i dimensions;
};

//#define STR_ME( X ) ( # X )
//
//namespace GameFlowType
//{
//  enum Enum
//  {
//    Invalid,
//    Run,
//    Exit,
//    Restart,
//    Restore
//  };
//
//  const char* ToStr[] =
//  {
//    STR_ME(Invalid),
//    STR_ME(Run),
//    STR_ME(Exit),
//    STR_ME(Restart),
//    STR_ME(Restore)
//  };
//};
//
//cout << GameFlowType::ToStr[ GameFlowType::Invalid ] << endl;

#endif