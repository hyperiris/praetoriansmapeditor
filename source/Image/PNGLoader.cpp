#include "PNGLoader.h"

bool loadPNG(const char* filename, Image* image)
{
  pngRawInfo info;
  
  if (!pngLoadRaw(filename, &info))
    return false;
    
  image->setComponentsCount(info.Components);
  image->setWidth(info.Width);
  image->setHeight(info.Height);
  image->setFormat(info.Components == 4 ? GL_RGBA :
                   info.Components == 3 ? GL_RGB  : GL_LUMINANCE);
  image->setInternalFormat(info.Components == 4 ? GL_RGBA8 :
                           info.Components == 3 ? GL_RGB8  : GL_LUMINANCE8);
  image->setDataBuffer(info.Data);
  
  delete info.Data;
  delete info.Palette;
  
  return true;
}