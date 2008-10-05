#include "FrameBufferObject.h"

FrameBufferObject::FrameBufferObject()
{
  sharedStencilBuffer = false;
  sharedDepthBuffer   = false;
  stencilBufferID     =     0;
  frameBufferID       =     0;
  depthBufferID       =     0;
  height              =     0;
  width               =     0;
}

bool  FrameBufferObject::initialize(Texture &texture,
                                    GLuint   sharedStencilBufferID,
                                    GLuint   sharedDepthBufferID,
                                    bool     depthOnly)
{
  if (frameBufferID)
    return Logger::writeErrorLog("FrameBufferObject already initialized!");
    
  if (texture.getWidth() <= 0 || texture.getHeight() <= 0)
    return Logger::writeErrorLog("Width and Height of FBO must be positive, non-zero");
    
  if (texture.getTarget() != GL_TEXTURE_2D           &&
      texture.getTarget() != GL_TEXTURE_CUBE_MAP_ARB &&
      texture.getTarget() != GL_TEXTURE_RECTANGLE_ARB)
    return Logger::writeErrorLog("Current FBO implementation only supports 2D/RECT/CUBE textures");
    
  if (!texture.getID())
    return Logger::writeErrorLog("FBO need a valid Texture ID");
    
  sharedStencilBuffer = (sharedStencilBufferID!= 0);
  sharedDepthBuffer   = (sharedDepthBufferID  != 0);
  
  height              = texture.getHeight();
  width               = texture.getWidth();
  
  glGenFramebuffersEXT(1, &frameBufferID);
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frameBufferID);
  
  if (!sharedDepthBufferID && !depthOnly)
  {
    glGenRenderbuffersEXT(1, &depthBufferID);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthBufferID);
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, width, height);
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
                                 GL_RENDERBUFFER_EXT, depthBufferID);
  }
  
  if (sharedDepthBufferID)
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
                                 GL_RENDERBUFFER_EXT, sharedDepthBufferID);
                                 
  if (!sharedStencilBufferID)
  {
    glGenRenderbuffersEXT(1, &stencilBufferID);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, stencilBufferID);
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_STENCIL_INDEX, width, height);
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT,
                                 GL_RENDERBUFFER_EXT, stencilBufferID);
  }
  else
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT,
                                 GL_RENDERBUFFER_EXT, sharedStencilBufferID);
                                 
                                 
  if ((texture.getTarget() == GL_TEXTURE_RECTANGLE_ARB) ||
      (texture.getTarget() == GL_TEXTURE_2D))
  {
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                              depthOnly ? GL_DEPTH_ATTACHMENT_EXT : GL_COLOR_ATTACHMENT0_EXT,
                              texture.getTarget(),
                              texture.getID(),
                              0);
    if (depthOnly)
    {
      glDrawBuffer(GL_NONE);
      glReadBuffer(GL_NONE);
    }
  }
  else
  {
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                              GL_COLOR_ATTACHMENT0_EXT,
                              GL_TEXTURE_CUBE_MAP_POSITIVE_X,
                              texture.getID(),
                              0);
                              
  }
  bool result = FrameBufferObject::checkStatus();
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
  return result;
}

/***************************************************************************************/
/*                                                                                     */
/*This function will report the status of the currently bound FBO                      */
/*                                                                                     */
/***************************************************************************************/
const bool FrameBufferObject::checkStatus()
{

  GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
  
  //Our FBO is perfect, return true
  if (status == GL_FRAMEBUFFER_COMPLETE_EXT)
    return true;
    
  switch (status)
  {
    case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
      return Logger::writeErrorLog("FBO has one or several image attachments with different internal formats");
      
    case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
      return Logger::writeErrorLog("FBO has one or several image attachments with different dimensions");
      
    case GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT:
      return Logger::writeErrorLog("FBO has a duplicate image attachment");
      
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
      return Logger::writeErrorLog("FBO missing an image attachment");
      
    case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
      return Logger::writeErrorLog("FBO format unsupported");
  }
  return Logger::writeErrorLog("Unknown FBO error");
}

void FrameBufferObject::attachRenderTarget(Texture &texture, int index, int newTarget)
{
  newTarget = (newTarget == -1) ? texture.getTarget() : newTarget;
  index     = clamp(index, 0, 15);
  
  if (frameBufferID)
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + index,
                              newTarget, texture.getID(), 0);
  else
    Logger::writeErrorLog("Invalid FrameBufferObject index");
}

void FrameBufferObject::detachRenderTarget(Texture &texture, int index, int newTarget)
{
  newTarget = (newTarget == -1) ? texture.getTarget() : newTarget;
  index     = clamp(index, 0, 15);
  
  if (frameBufferID)
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + index,
                              newTarget, 0, 0);
  else
    Logger::writeErrorLog("Invalid FrameBufferObject index");
}

void FrameBufferObject::activate()
{
  if (frameBufferID)
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frameBufferID);
  else
    Logger::writeErrorLog("Invalid FrameBufferObject index");
}

void const FrameBufferObject::deactivate()
{
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

const GLuint FrameBufferObject::getStencilBufferID() const
{
  return stencilBufferID;
}
const GLuint FrameBufferObject::getDepthBufferID()   const
{
  return depthBufferID;
}
const GLuint FrameBufferObject::getFrameBufferID()   const
{
  return frameBufferID;
}
const GLuint FrameBufferObject::getHeight()          const
{
  return height;
}
const GLuint FrameBufferObject::getWidth()           const
{
  return width;
}

FrameBufferObject::~FrameBufferObject()
{
  if (stencilBufferID && !sharedStencilBuffer)
    glDeleteRenderbuffersEXT(1, &stencilBufferID);
    
  if (depthBufferID && !sharedDepthBuffer)
    glDeleteRenderbuffersEXT(1, &depthBufferID);
    
  if (frameBufferID)
    glDeleteFramebuffersEXT(1, &frameBufferID);
    
  sharedStencilBuffer = false;
  sharedDepthBuffer   = false;
  stencilBufferID     =     0;
  frameBufferID       =     0;
  depthBufferID       =     0;
  height              =     0;
  width               =     0;
}