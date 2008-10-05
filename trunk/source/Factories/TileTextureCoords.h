#ifndef TILETEXTURECOORDS
#define TILETEXTURECOORDS

#include "FactoryUtils.h"

class TileTextureCoords
{

  public:
    TileTextureCoords();
    TileTextureCoords(const TileTextureCoords &copy);
    TileTextureCoords(const Tuple4i &bounds);
    TileTextureCoords(int xOff, int yOff, int zOff, int wOff);
    ~TileTextureCoords();
    
  public:
    TileTextureCoords& operator = (const TileTextureCoords& right);
    
  public:
    void computeTextureCoords(const Tuple4i& bounds);
    void computeTextureCoords(int x, int y, int z, int w);
    Tuple2f* getTextureCoords();
    void transformImage(unsigned int flags);
    void transformCoords(Tuple2f *coords);
    
  private:
    void rotate(float angle);
    
  private:
    Tuple2f textCoords[10];
};

#endif
