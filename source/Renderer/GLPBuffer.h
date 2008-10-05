#ifndef GLPBUFFER_H
#define GLPBUFFER_H
#include "../Tools/Logger.h"

#define PBUFFER_USE_DOUBLE_BUFFER          0x00000001
#define PBUFFER_RENDER_TO_TEXTURE          0x00000002
#define PBUFFER_TEXTURE_CUBE_MAP           0x00000004
#define PBUFFER_FORMAT_FLOAT               0x00000008
#define PBUFFER_RGBA                       0x00000010
#define PBUFFER_RECTANGLE_RGB_NV           0x00000020
#define PBUFFER_RECTANGLE_RGBA_NV          0x00000040
#define PBUFFER_RECTANGLE_RGB_ATI          0x00000080
#define PBUFFER_RECTANGLE_RGBA_ATI         0x00000100
#define PER_CHANNEL_COLOR_BITS_16          0x00000200
#define PER_CHANNEL_COLOR_BITS_24          0x00000400
#define PER_CHANNEL_COLOR_BITS_32          0x00000800
#define PBUFFER_RENDER_TO_TEXTURE_RECT_NV  0x00001000
#define PBUFFER_RENDER_TO_TEXTURE_RECT_ATI 0x00002000

class GLPBuffer
{
    HPBUFFERARB pBufferHandle;
    HGLRC       renderingContext;
    HDC         deviceContext;
    
    int         stencilBits,
    depthBits,
    height,
    width;
    
  public:
    HPBUFFERARB getPBufferHandle()
    {
      return pBufferHandle;
    }
    HGLRC       getRenderingContext()
    {
      return renderingContext;
    }
    bool  initialize(int newWidth,     int newHeight,
                     int newDepthBits, int newStencilBits, int pBufferFormat);
                     
    void  releasePBuffer();
    void  destroy();
    void  setCurrentCubeMapFace(int anIntFromZeroToFive);
    bool  makeCurrent();
    int   getWidth()
    {
      return width;
    }
    int   getHeight()
    {
      return height;
    }
    
};
#endif