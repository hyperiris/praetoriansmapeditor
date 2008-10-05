#include "GLPBuffer.h"

int facesAttributes[3] = { WGL_CUBE_MAP_FACE_ARB,
                           WGL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB,
                           0
                         };

bool GLPBuffer::initialize(int newWidth,     int newHeight,
                           int newDepthBits, int newStencilBits,
                           int pBufferFormat)
{
  //Check for pbuffer support
  if (!GLEE_WGL_ARB_extensions_string ||
      !GLEE_WGL_ARB_pixel_format      ||
      !GLEE_WGL_ARB_pbuffer)
    return Logger::writeErrorLog("Extensions required for pbuffer unsupported");
    
  bool   renderToTexture = pBufferFormat & PBUFFER_RENDER_TO_TEXTURE_RECT_ATI ||
                           pBufferFormat & PBUFFER_RENDER_TO_TEXTURE_RECT_NV  ||
                           pBufferFormat & PBUFFER_RENDER_TO_TEXTURE;
                           
  GLuint format;
  GLint  attributes[256],
  pixelFormat,
  i = 0;
  
  memset(attributes, 0, sizeof(int)*256);
  
  width       = newWidth;
  height      = newHeight;
  
  depthBits   = newDepthBits;
  stencilBits = newStencilBits;
  
  HDC  hCurrentDC = wglGetCurrentDC();
  
  if (!hCurrentDC)
    return Logger::writeErrorLog("Unable to get current Device Context");
    
  int numberOfBits = pBufferFormat & PER_CHANNEL_COLOR_BITS_16 ? 16 :
                     pBufferFormat & PER_CHANNEL_COLOR_BITS_24 ? 24 :
                     pBufferFormat & PER_CHANNEL_COLOR_BITS_32 ? 32 : 8;
                     
  attributes[i++] = WGL_RED_BITS_ARB;
  attributes[i++] = numberOfBits;
  attributes[i++] = WGL_GREEN_BITS_ARB;
  attributes[i++] = numberOfBits;
  attributes[i++] = WGL_BLUE_BITS_ARB;
  attributes[i++] = numberOfBits;
  
  if (pBufferFormat & PBUFFER_RGBA)
  {
    attributes[i++] = WGL_ALPHA_BITS_ARB;
    attributes[i++] = numberOfBits;
  }
  
  if (depthBits)
  {
    attributes[i++] = WGL_DEPTH_BITS_ARB,
                      attributes[i++] = depthBits;
  }
  
  if (stencilBits)
  {
    attributes[i++] = WGL_STENCIL_BITS_ARB,
                      attributes[i++] = stencilBits;
  }
  
  if (pBufferFormat & PBUFFER_FORMAT_FLOAT)
  {
    attributes[i++] = WGL_PIXEL_TYPE_ARB;
    if (GLEE_NV_float_buffer)
    {
      attributes[i++] = WGL_TYPE_RGBA_ARB;
      attributes[i++] = WGL_FLOAT_COMPONENTS_NV;
      attributes[i++] = GL_TRUE;
    }
    else
    {
      if (GLEE_WGL_ATI_pixel_format_float)
        attributes[i++] = WGL_TYPE_RGBA_FLOAT_ATI;
      else
        attributes[i++] = WGL_TYPE_RGBA_ARB;
    }
  }
  
  attributes[i++] = WGL_DRAW_TO_PBUFFER_ARB;
  attributes[i++] = GL_TRUE;
  
  attributes[i++] = WGL_SUPPORT_OPENGL_ARB;
  attributes[i++] = GL_TRUE;
  
  if (pBufferFormat & PBUFFER_USE_DOUBLE_BUFFER)
  {
    attributes[i++] = WGL_DOUBLE_BUFFER_ARB;
    attributes[i++] = GL_TRUE;
  }
  
  if (GLEE_WGL_ARB_render_texture && renderToTexture)
  {
    if (pBufferFormat & PBUFFER_RECTANGLE_RGB_NV)
    {
      attributes[i++] = (pBufferFormat & PBUFFER_FORMAT_FLOAT) ?
                        WGL_BIND_TO_TEXTURE_RECTANGLE_FLOAT_RGB_NV :
                        WGL_BIND_TO_TEXTURE_RECTANGLE_RGB_NV;
    }
    else
    {
      if (pBufferFormat & PBUFFER_RECTANGLE_RGBA_NV)
      {
        attributes[i++] = (pBufferFormat & PBUFFER_FORMAT_FLOAT) ?
                          WGL_BIND_TO_TEXTURE_RECTANGLE_FLOAT_RGBA_NV :
                          WGL_BIND_TO_TEXTURE_RECTANGLE_RGBA_NV;
      }
      else
      {
        if (pBufferFormat & PBUFFER_RGBA)
        {
          attributes[i++] = WGL_BIND_TO_TEXTURE_RGBA_ARB;
        }
        else
        {
          attributes[i++] = WGL_BIND_TO_TEXTURE_RGB_ARB;
        }
      }
    }
    attributes[i++] = GL_TRUE;
  }
  
  if (!wglChoosePixelFormatARB(hCurrentDC, attributes, NULL, 1, &pixelFormat, &format))
    return Logger::writeErrorLog("Failed to select a proper pixel format!");
    
  memset(attributes, 0, sizeof(int)*256);
  i = 0;
  
  if (GLEE_WGL_ARB_render_texture && renderToTexture)
  {
    attributes[i++] = WGL_TEXTURE_FORMAT_ARB;
    attributes[i++] = pBufferFormat & PBUFFER_RGBA ?
                      WGL_TEXTURE_RGBA_ARB : WGL_TEXTURE_RGB_ARB;
                      
    attributes[i++] = WGL_TEXTURE_TARGET_ARB;
    
    if (pBufferFormat & PBUFFER_TEXTURE_CUBE_MAP)
    {
      attributes[i++] = WGL_TEXTURE_CUBE_MAP_ARB;
    }
    else
    {
      if (pBufferFormat & PBUFFER_RENDER_TO_TEXTURE_RECT_NV)
      {
        cout << "her";
        attributes[i++] = WGL_TEXTURE_RECTANGLE_NV;
      }
      else
      {
        attributes[i++] = WGL_TEXTURE_2D_ARB;
      }
    }
  }
  pBufferHandle =wglCreatePbufferARB(hCurrentDC, pixelFormat, width, height, attributes);
  
  if (!pBufferHandle)
    return Logger::writeErrorLog("Unable to create pbuffer");
    
  deviceContext =wglGetPbufferDCARB(pBufferHandle);
  
  if (!deviceContext)
    return Logger::writeErrorLog("Unable to get pbuffer's device context");
    
  if (!deviceContext)
    return Logger::writeErrorLog("Unable to get pbuffer's device context");
    
  //Create a rendering context for the pbuffer
  renderingContext = wglCreateContext(deviceContext);
  
  if (!renderingContext)
    return Logger::writeErrorLog("Unable to create pbuffer's rendering context");
    
  wglShareLists(wglGetCurrentContext(), renderingContext);
  return GL_TRUE;
}

void GLPBuffer::destroy()
{
  Logger::writeInfoLog("Destroying pixel buffer");
  if (renderingContext)
  {
    if (!wglDeleteContext(renderingContext))
    {
      Logger::writeErrorLog("Release of Pbuffer Rendering Context Failed");
    }
    renderingContext =NULL;
  }
  
  if (deviceContext  && !wglReleasePbufferDCARB(pBufferHandle , deviceContext))
  {
    Logger::writeErrorLog("Release of Pbuffer Device Context Failed");
    deviceContext =NULL;
  }
  
  if (!wglDestroyPbufferARB(pBufferHandle))
  {
    Logger::writeErrorLog("Unable to destroy pbuffer");
  }
}

void GLPBuffer::setCurrentCubeMapFace(int anIntFromZeroToFive)
{
  if (anIntFromZeroToFive < 0 || anIntFromZeroToFive > 5)
  {
    Logger::writeErrorLog("Face index must be from Zero To Five");
    return;
  }
  facesAttributes[1] =  WGL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + anIntFromZeroToFive;
  wglSetPbufferAttribARB(pBufferHandle, facesAttributes);
}

void GLPBuffer::releasePBuffer()
{
  wglReleaseTexImageARB(pBufferHandle, WGL_FRONT_LEFT_ARB);
}

bool GLPBuffer::makeCurrent()
{
  if (!wglMakeCurrent(deviceContext , renderingContext))
    return Logger::writeErrorLog("Unable to change current context");
    
  return true;
}