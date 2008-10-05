#ifndef FRAME_BUFFER_OBJECT_H
#define FRAME_BUFFER_OBJECT_H

#include "../Appearance/Texture.h"
#include "../Tools/Logger.h"

class FrameBufferObject
{
  private:
    GLuint stencilBufferID,
    depthBufferID,
    frameBufferID,
    height,
    width;
    
    bool   sharedStencilBuffer,
    sharedDepthBuffer;
    
  public:
    FrameBufferObject();
    ~FrameBufferObject();
    
    bool initialize(Texture &texture,
                    GLuint   sharedStencilBufferID = 0,
                    GLuint   sharedDepthBufferID   = 0,
                    bool     depthOnly             = false);
    void attachRenderTarget(Texture &texture, int index = 0, int newTarget = -1);
    void detachRenderTarget(Texture &texture, int index = 0, int newTarget = -1);
    
    static const void deactivate();
    void   activate();
    
    const GLuint getStencilBufferID() const;
    const GLuint getDepthBufferID()   const;
    const GLuint getFrameBufferID()   const;
    const GLuint getHeight()          const;
    const GLuint getWidth()           const;
    
    static  const bool  checkStatus();
};
#endif