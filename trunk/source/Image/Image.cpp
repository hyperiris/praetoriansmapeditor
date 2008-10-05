#include "Image.h"
#include "PNGLoader.h"
#include "../Managers/MediaPathManager.h"

Image::Image(const char *path)
{
  internalFormat =    0;
  components     =    0;
  dataBuffer     = NULL;
  format         =    0;
  height         =    0;
  width          =    0;
  depth          =    0;
  
  if (path)
    load(path);
}

Image::Image(const Image &image)
{
  internalFormat =    0;
  components     =    0;
  dataBuffer     = NULL;
  format         =    0;
  height         =    0;
  width          =    0;
  depth          =    0;
  
  operator=(image);
}

Image &Image::operator=(const Image & copy)
{
  if (this != &copy)
  {
    internalFormat = copy.internalFormat;
    components     = copy.components;
    format         = copy.format;
    height         = copy.height;
    width          = copy.width;
    depth          = copy.depth;
    path           = copy.path;
    
    setDataBuffer(copy.dataBuffer);
  }
  return *this;
}

void Image::setWidth(unsigned short w)
{
  width = w;
}

void Image::setHeight(unsigned short h)
{
  height = h;
}

void Image::setDataBuffer(const GLubyte *buffer)
{
  if (!buffer || !width || !height || !components)
    return;
    
  deleteArray(dataBuffer);
  
  int size = (internalFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ?
             ((width + 3) / 4) * ((height + 3) / 4) * 8 :
             ((internalFormat == GL_COMPRESSED_RGBA_S3TC_DXT3_EXT) || (internalFormat == GL_COMPRESSED_RGBA_S3TC_DXT5_EXT)) ?
             ((width + 3) / 4) * ((height + 3) / 4) * 16 : width * height * components;
  dataBuffer = new GLubyte[size];
  memcpy(dataBuffer, buffer, size);
}

void Image::setFormat(GLuint f)
{
  format = f;
}

void Image::setInternalFormat(GLuint iformat)
{
  internalFormat = iformat;
}

void Image::setDepth(unsigned short d)
{
  depth = d;
}

void Image::setComponentsCount(GLuint c)
{
  components = c < 1 ? 1 : c > 4 ? 4 : c;
}

void Image::flipVertically()
{
  if (depth)
    return;
    
  GLubyte *newDataBuffer = new GLubyte[width*height*components];
  int      counterDown   = 0,
                           counterUp     = 0;
                           
  if (components == 3)
  {
    for (int y = 0, y1 = height - 1; y < height; y++, y1--)
      for (int x = 0; x < width; x++)
      {
        counterUp   = (x + y1 * width) * 3;
        counterDown = (x +  y * width) * 3;
        newDataBuffer[counterUp + 0]   = dataBuffer[counterDown + 0];
        newDataBuffer[counterUp + 1]   = dataBuffer[counterDown + 1];
        newDataBuffer[counterUp + 2]   = dataBuffer[counterDown + 2];
      }
  }
  
  if (components == 4)
  {
    for (int y = 0, y1 = height - 1; y < height; y++, y1--)
      for (int x = 0; x < width; x++)
      {
        counterUp   = (x + y1 * width) * components;
        counterDown = (x +  y * width) * components;
        newDataBuffer[counterUp + 0]   = dataBuffer[counterDown + 0];
        newDataBuffer[counterUp + 1]   = dataBuffer[counterDown + 1];
        newDataBuffer[counterUp + 2]   = dataBuffer[counterDown + 2];
        newDataBuffer[counterUp + 3]   = dataBuffer[counterDown + 3];
      }
  }
  
  if (components == 1)
  {
    for (int y = 0, y1 = height - 1; y < height; y++, y1--)
      for (int x = 0; x < width; x++)
      {
        counterUp   = x + y1 * width;
        counterDown = x +  y * width;
        newDataBuffer[counterUp + 0]   = dataBuffer[counterDown + 0];
      }
  }
  
  setDataBuffer(newDataBuffer);
  deleteArray(newDataBuffer);
}

bool Image::load(const char* path_)
{
  return loadSupportedFormat(path_);
}

bool Image::loadSupportedFormat(const char* path_)
{
  const char *verifiedPath = MediaPathManager::lookUpMediaPath(path_);
  
  if (!verifiedPath)
    return false;
    
  path = verifiedPath;
  
  if (isPNG(verifiedPath))
    return loadPNG(verifiedPath, this);
    
  if (isJPEG(verifiedPath))
    return loadJPG(verifiedPath);
    
  if (isTGA(verifiedPath))
    return loadTGA(verifiedPath);
    
  if (isDDS(verifiedPath))
    return loadDDS(verifiedPath);
    
  return false;
}

void Image::setPath(const char *p)
{
  path = p;
}

const GLuint   Image::getComponentsCount() const
{
  return components;
}
const GLuint   Image::getInternalFormat()  const
{
  return internalFormat;
}
const GLubyte* Image::getDataBuffer()      const
{
  return dataBuffer;
}
const GLuint   Image::getFormat()          const
{
  return format;
}
const String  &Image::getPath()            const
{
  return path;
}

const GLushort Image::getHeight() const
{
  return height;
}
const GLushort Image::getWidth()  const
{
  return width;
}
const GLushort Image::getDepth()  const
{
  return depth;
}

Image::~Image()
{
  if (dataBuffer)
    deleteArray(dataBuffer);
}
