#ifndef MAPDESCRIPTOR
#define MAPDESCRIPTOR

#include "../Tools/String.h"
#include "../Math/Tuple4.h"
#include "../Containers/ocarray.h"

class XMLElement;

namespace MapTypes
{
  enum Enum
  {
    NEW_MAP,
    LOAD_MAP
  };
}

class MapDescriptor
{
  public:
    MapDescriptor();
    ~MapDescriptor();
    
  public:
    bool loadXMLSettings(XMLElement *node);
    
  public:
    String mapName;
    String mapMode;
    String mapVisual;
    String mapObjects;
    String mapCharacters;
    String mapMinimap;
    String mapTextures;
    String mapMeadows;
    String mapLogic;
    String mapColors;
    String mapProjectName;
    String mapPastureType;
    String mapWaters;
    Tuple2i mapDimensions;
    Tuple3f camposition;
    Tuple3f camfocus;
    unsigned int maxPlayers;
    unsigned int mapType;
    Tuple2f mapElevation;
    Array <Tuple2f> playerPositions;
};

#endif