#ifndef TERRAINTILE
#define TERRAINTILE

#include "../Math/Tuple4.h"

class Tile
{
  public:
    Tile();
    Tile(const Tile& copy);
    ~Tile();
    
  public:
    Tile &operator = (const Tile& right);
    
  public:
    void setPrimaryTextureID(short id);
    void setSecondaryTextureID(short id);
    void setFlags(unsigned short bits);
    
    void setVertices(Tuple3f* verts);
    void setColors(Tuple4ub* cols);
    Tuple4ub* getColors();
    Tuple3f* getVerts();
    
    short getPrimaryTextureID();
    short getSecondaryTextureID();
    unsigned short getFlags();
    
    void reset();
    
    //private:
    bool grass;
    bool boundary;
    
    unsigned short flags;
    
    short textureID[2];
    
    //unsigned int logic;
    
    Tuple4ub colors[9];
    Tuple3f vertices[9];
};

#endif
