#include "Texture.h"
#include "../Tools/Perlin.h"
#include "../Managers/MediaPathManager.h"
#include <gl/glu.h>

bool Texture::load2D(const char* filename,
                     GLuint clampS   , GLuint clampT,
                     GLuint magFilter, GLuint minFilter,
                     bool  mipmap)
{
  const char  *texturePath = MediaPathManager::lookUpMediaPath(filename);
  
  if (!texturePath)
    return Logger::writeErrorLog(String("Couldn't locate the Texture file at <") +
                                 filename + "> even with a look up");
                                 
  if (checkForRepeat(texturePath))
    return true;
    
  Image  image;
  
  if (!image.load(filename))
    return Logger::writeErrorLog(String("Could not load Texture2D file at -> ") + filename);
    
  return load2DImage(image, clampS, clampT, magFilter, minFilter, mipmap);
}

bool Texture::load2DImage(const Image& image,
                          GLuint clampS   , GLuint clampT,
                          GLuint magFilter, GLuint minFilter,
                          bool  mipmap)
{
  const char *path = image.getPath();
  if (checkForRepeat(path))
    return true;
    
  destroy();
  target = GL_TEXTURE_2D;
  
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, getValidMagFilter(magFilter));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, getValidMinFilter(minFilter));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     getValidWrapMode(clampS));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     getValidWrapMode(clampT));
  
  if (loadTextureFace(image, GL_TEXTURE_2D, mipmap));
  else
  {
    destroy();
    return Logger::writeErrorLog(String("Could not load Texture2D file."));
  }
  
  height = image.getHeight();
  width  = image.getWidth();
  depth  = 1;
  
  if (finalizeLoading(path))
  {
    Logger::writeInfoLog(String("Loaded Texture2D file at -> ") + path);
    return true;
  }
  return false;
}

bool Texture::loadCube(const char* pathPXdotExtension,
                       GLuint clampS   , GLuint clampT,
                       GLuint magFilter, GLuint minFilter,
                       bool  mipmap)
{
  String initialPath  = MediaPathManager::lookUpMediaPath(pathPXdotExtension);
  String temp;
  
  if (!initialPath.getLength())
    return Logger::writeErrorLog(String("Couldn't locate the texture file at <") +
                                 pathPXdotExtension + "> even with a look up");
  destroy();
  target = GL_TEXTURE_CUBE_MAP;
  
  std::string  verifiedPaths[6],
  faces[6]     = { "PX", "NX", "PY", "NY", "PZ", "NZ"},
                 body;
                 
  int extensionIndex = 0;
  
  for (int j = 0; j < int(initialPath.getLength()); j++)
    if (initialPath[j] == '.')
      extensionIndex = j;
      
  if (extensionIndex == 0)
    return false;
    
  const  char   *extension  = initialPath.getBytes() + extensionIndex;
  GLint  strl        = 0;
  
  strl  = initialPath.getLength();
  strl -= int(strlen(extension)) + 2;
  
  for (int i = 0; i< 6; i++)
  {
    for (int l = 0; l < strl; l++)
      verifiedPaths[i] += initialPath[l];
      
    verifiedPaths[i] += faces[i];
    verifiedPaths[i] += extension;
    temp = MediaPathManager::lookUpMediaPath(verifiedPaths[i].data());
    
    if (temp.getLength())
      verifiedPaths[i] = temp;
    else
      return Logger::writeErrorLog(String("Failed to locate cubemap texture face ") + verifiedPaths[i].data());
  }
  
  if (checkForRepeat(verifiedPaths[0].data()))
    return true;
    
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, id);
  glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, getValidMagFilter(magFilter));
  glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, getValidMinFilter(minFilter));
  glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S,     getValidWrapMode(clampS));
  glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T,     getValidWrapMode(clampT));
  
  for (int i = 0; i < 6; i++)
  {
    Image image;
    
    if (!image.load(verifiedPaths[i].data()))
    {
      destroy();
      return Logger::writeErrorLog(String("Failed at loading CubeMap face: ") + i);
    }
    
    if (!loadTextureFace(image, GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + i, mipmap))
      return Logger::writeErrorLog(String("Failed while loading CubeMap face: ") + i);
      
    if (i == 5)
    {
      height = image.getHeight();
      width  = image.getWidth();
      depth  = 1;
    }
  }
  
  Logger::writeInfoLog(String("Loaded TextureCubeMap at ->") + initialPath);
  return  finalizeLoading(initialPath);
}

bool Texture::create2DShell(const char* name,
                            GLuint width    , GLuint height,
                            GLuint inFormat , GLuint format,
                            GLuint clampS   , GLuint clampT,
                            GLuint magFilter, GLuint minFilter)
{
  if (!name)
    return Logger::writeErrorLog("2D Texture Shell must have a valid name");
    
  if (checkForRepeat(name))
    return true;
    
  destroy();
  target = GL_TEXTURE_2D;
  
  //Logger::writeInfoLog(String("Loading new 2D Shell: width = ") + int(width)
  //                     + ", height = " + int(height));
  
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, getValidMagFilter(magFilter));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, getValidMinFilter(minFilter));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     getValidWrapMode(clampS));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     getValidWrapMode(clampT));
  glTexImage2D(GL_TEXTURE_2D, 0, inFormat, width, height, 0, format, GL_UNSIGNED_BYTE, NULL);
  
  this->height = height;
  this->width  = width;
  depth        = 1;
  
  return finalizeLoading(name);
}

bool Texture::createNoise3D(const char* name, GLuint size)
{
  if (!name)
    return Logger::writeErrorLog("Noise3D Texture must have a valid name");
    
  if (checkForRepeat(name))
    return true;
    
  destroy();
  target = GL_TEXTURE_3D;
  
  Logger::writeInfoLog(String("Creating new Noise3D Texture: width = ")
                       + int(size) + ", height = " + int(size) + ", depth =" + int(size));
                       
                       
  GLubyte *noise3DBuffer = new GLubyte[size * size * size * 4],
  *ptr           = NULL;
  
  Tuple3d  ni;
  
  double   inci= 0.0,
                 incj = 0.0,
                        inck = 0.0,
                               amp  = 0.5;
                               
  GLuint   f, i, j, k, inc,
  startFrequency = 4,
                   frequency      = 4,
                                    numOctaves     = 4;
                                    
  for (f = 0, inc = 0; f < numOctaves; ++f, frequency *= 2, ++inc, amp *= 0.5)
  {
    Perlin::setNoiseFrequency(frequency);
    ptr = noise3DBuffer;
    ni.set(0.0, 0.0, 0.0);
    
    inci = 1.0 / (size / frequency);
    for (i = 0; i < size; ++i, ni[0] += inci)
    {
      incj = 1.0 / (size / frequency);
      for (j = 0; j < size; ++j, ni[1] += incj)
      {
        inck = 1.0 / (size / frequency);
        for (k = 0; k < size; ++k, ni[2] += inck, ptr += 4)
          *(ptr + inc) = (GLubyte)(((Perlin::noise3(ni) + 1.0) * amp) * 128.0);
      }
    }
  }
  
  glGenTextures(1, &id);
  glBindTexture(target, id);
  glTexParameterf(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameterf(target, GL_TEXTURE_WRAP_R, GL_REPEAT);
  glTexParameterf(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexImage3DEXT(target, 0, GL_RGBA,
                  size, size, size,
                  0, GL_RGBA, GL_UNSIGNED_BYTE, noise3DBuffer);
  deleteArray(noise3DBuffer);
  return true;
}

bool Texture::createRectShell(const char* name,
                              GLuint width, GLuint height,
                              GLuint inFormat, GLuint format,
                              GLuint clampS  , GLuint clampT,
                              GLuint magFilter, GLuint minFilter)
{
  if (!name)
    return Logger::writeErrorLog("Rectangle Texture Shell must have a valid name");
    
  if (checkForRepeat(name))
    return true;
    
  destroy();
  target = GL_TEXTURE_RECTANGLE_ARB;
  
  Logger::writeInfoLog(String("Loading new Rectangle Shell: width = ") + int(width)
                       + ", height = " + int(height));
                       
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_RECTANGLE_ARB, id);
  
  glTexParameteri(target, GL_TEXTURE_MAG_FILTER, getValidMagFilter(magFilter));
  glTexParameteri(target, GL_TEXTURE_MIN_FILTER, getValidMinFilter(minFilter));
  glTexParameteri(target, GL_TEXTURE_WRAP_S,     getValidWrapMode(clampS));
  glTexParameteri(target, GL_TEXTURE_WRAP_T,     getValidWrapMode(clampT));
  glTexImage2D(target, 0, inFormat, width, height, 0, format, GL_UNSIGNED_BYTE, NULL);
  
  this->height = height;
  this->width  = width;
  depth        = 1;
  
  return finalizeLoading(name);
}

bool Texture::create1DShell(const char* name,
                            GLuint width    ,
                            GLuint inFormat , GLuint format,
                            GLuint clampS   , GLuint clampT,
                            GLuint magFilter, GLuint minFilter)
{
  if (!name)
    return Logger::writeErrorLog("1D Texture Shell must have a valid name");
  destroy();
  target = GL_TEXTURE_1D;
  
  
  Logger::writeInfoLog(String("Loading new 1D Shell: width = ") + int(width)
                       + ", height = 1");
                       
  glGenTextures(1, &id);
  glBindTexture(target, id);
  
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, getValidMagFilter(magFilter));
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, getValidMinFilter(minFilter));
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S,     getValidWrapMode(clampS));
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T,     getValidWrapMode(clampT));
  glTexImage1D(GL_TEXTURE_1D, 0, inFormat, width, 0, format, GL_UNSIGNED_BYTE, NULL);
  
  this->width  = width;
  height       = 1;
  depth        = 1;
  
  return finalizeLoading(name);
}

bool Texture::create3DShell(const char* name,
                            GLuint width, GLuint height, GLuint depth,
                            GLuint inFormat , GLuint format,
                            GLuint clampS   , GLuint clampT, GLuint clampR,
                            GLuint magFilter, GLuint minFilter)
{
  if (!name)
    return Logger::writeErrorLog("3D Texture Shell must have a valid name");
  destroy();
  target = GL_TEXTURE_3D;
  
  
  Logger::writeInfoLog(String("Loading new 3D Shell: width = ") + int(width)
                       + ", height = " + int(height) + ", depth = "  + int(depth));
                       
  glGenTextures(1, &id);
  glBindTexture(target, id);
  
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, getValidMagFilter(magFilter));
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, getValidMinFilter(minFilter));
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S,     getValidWrapMode(clampS));
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T,     getValidWrapMode(clampT));
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R,     getValidWrapMode(clampR));
  glTexImage3D(GL_TEXTURE_3D, 0, inFormat, width, height, depth, 0, format, GL_UNSIGNED_BYTE, NULL);
  
  this->height = height;
  this->width  = width;
  this->depth  = depth;
  
  return finalizeLoading(name);
}

bool Texture::createCubeShell(const char* name,
                              GLuint size     ,
                              GLuint inFormat , GLuint format,
                              GLuint clampS   , GLuint clampT,
                              GLuint magFilter, GLuint minFilter)
{
  if (!name)
    return Logger::writeErrorLog("Cube Texture Shell must have a valid name");
    
  if (checkForRepeat(name))
    return true;
    
  destroy();
  target = GL_TEXTURE_CUBE_MAP;
  
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, id);
  
  glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, getValidMagFilter(magFilter));
  glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, getValidMinFilter(minFilter));
  glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S,     getValidWrapMode(clampS));
  glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T,     getValidWrapMode(clampT));
  
  Logger::writeInfoLog(String("Loading Cube Shell: size = ") + int(size));
  
  for (int i = 0; i < 6; i++)
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + i,
                 0,
                 inFormat,
                 size,
                 size,
                 0,
                 format,
                 GL_UNSIGNED_BYTE,
                 NULL);
                 
  this->height = size;
  this->width  = size;
  depth        = 1;
  
  return finalizeLoading(name);
}

bool Texture::createNormalizingCube(const char* name,
                                    GLuint size,
                                    bool   buildMipmaps)
{
  size = getClosestPowerOfTwo(size);
  
  if (!name)
    return Logger::writeErrorLog("Normalized Cube Texture must have a valid name");
    
  if (checkForRepeat(name))
    return true;
    
  destroy();
  
  int levels = buildMipmaps ? int(log(double(size))/log(2.0)) : 1;
  target = GL_TEXTURE_CUBE_MAP;
  Logger::writeInfoLog(String("Creating Normalization Texture Cube Map, size = ")
                       + int(size) + ", MipMaps = " + int(levels));
                       
  GLuint minFilter = buildMipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;
  
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, id);
  
  glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, minFilter);
  
  
  for (int currentLevel = 0; currentLevel < levels; currentLevel++)
  {
    float fX    = 0.0,
                  fY    = 0.0,
                          fZ    = 0.0,
                                  oolen = 1.0;
    int x            =    0,
                          y            =    0,
                                            z            =    0,
                                                              mapSize      = size/(currentLevel + 1),
                                                                             halfMapSize  = mapSize/2;
                                                                             
    GLubyte *dataContainer = new GLubyte[mapSize*mapSize*3];
    
    for (y=0;y<mapSize;y++)
    {
      for (z=0;z<mapSize;z++)
      {
        fX    = (float)halfMapSize;
        fY    = (float)halfMapSize-y;
        fZ    = (float)halfMapSize-z;
        oolen = 1.0f/fastSquareRoot(fX*fX+fY*fY+fZ*fZ);
        
        fX*=oolen;
        fY*=oolen;
        fZ*=oolen;
        
        dataContainer[y*3*mapSize+z*3+0] = GLubyte((((fX)+1.0f)/2.0f)*255.0f);
        dataContainer[y*3*mapSize+z*3+1] = GLubyte((((fY)+1.0f)/2.0f)*255.0f);
        dataContainer[y*3*mapSize+z*3+2] = GLubyte((((fZ)+1.0f)/2.0f)*255.0f);
      }
    }
    
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + 0,
                 currentLevel, GL_RGB8, mapSize, mapSize,
                 0, GL_RGB, GL_UNSIGNED_BYTE, dataContainer);
                 
    for (y=0;y<mapSize;y++)
    {
      for (z=0;z<mapSize;z++)
      {
        fX    =(float)-halfMapSize;
        fY    =(float)halfMapSize-y;
        fZ    =(float)z-halfMapSize;
        oolen = 1.0f/fastSquareRoot(fX*fX+fY*fY+fZ*fZ);
        
        fX*=oolen;
        fY*=oolen;
        fZ*=oolen;
        dataContainer[y*3*mapSize+z*3+0] = GLubyte((((fX)+1.0f)/2.0f)*255.0f);
        dataContainer[y*3*mapSize+z*3+1] = GLubyte((((fY)+1.0f)/2.0f)*255.0f);
        dataContainer[y*3*mapSize+z*3+2] = GLubyte((((fZ)+1.0f)/2.0f)*255.0f);
      }
    }
    
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + 1,
                 currentLevel, GL_RGB8, mapSize, mapSize,
                 0, GL_RGB, GL_UNSIGNED_BYTE, dataContainer);
                 
    for (z=0;z<mapSize;z++)
    {
      for (x=0;x<mapSize;x++)
      {
        fX    = (float)x-halfMapSize;
        fY    = (float)halfMapSize;
        fZ    = (float)z-halfMapSize;
        oolen = 1.0f/fastSquareRoot(fX*fX+fY*fY+fZ*fZ);
        
        fX*=oolen;
        fY*=oolen;
        fZ*=oolen;
        dataContainer[z*3*mapSize+x*3+0] = GLubyte((((fX)+1.0f)/2.0f)*255.0f);
        dataContainer[z*3*mapSize+x*3+1] = GLubyte((((fY)+1.0f)/2.0f)*255.0f);
        dataContainer[z*3*mapSize+x*3+2] = GLubyte((((fZ)+1.0f)/2.0f)*255.0f);
      }
    }
    
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + 2,
                 currentLevel, GL_RGB8, mapSize, mapSize,
                 0, GL_RGB, GL_UNSIGNED_BYTE, dataContainer);
                 
    for (z=0;z<mapSize;z++)
    {
      for (x=0;x<mapSize;x++)
      {
        fX    = (float)x-halfMapSize;
        fY    = (float)-halfMapSize;
        fZ    = (float)halfMapSize-z;
        oolen = 1.0f/fastSquareRoot(fX*fX+fY*fY+fZ*fZ);
        
        fX*=oolen;
        fY*=oolen;
        fZ*=oolen;
        
        dataContainer[z*3*mapSize+x*3+0] = GLubyte((((fX)+1.0f)/2.0f)*255.0f);
        dataContainer[z*3*mapSize+x*3+1] = GLubyte((((fY)+1.0f)/2.0f)*255.0f);
        dataContainer[z*3*mapSize+x*3+2] = GLubyte((((fZ)+1.0f)/2.0f)*255.0f);
      }
    }
    
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + 3,
                 currentLevel, GL_RGB8, mapSize, mapSize,
                 0, GL_RGB, GL_UNSIGNED_BYTE, dataContainer);
                 
    for (y=0;y<mapSize;y++)
    {
      for (x=0;x<mapSize;x++)
      {
        fX    = (float)x-halfMapSize;
        fY    = (float)halfMapSize - y;
        fZ    = (float)halfMapSize;
        oolen = 1.0f/fastSquareRoot(fX*fX+fY*fY+fZ*fZ);
        
        fX*=oolen;
        fY*=oolen;
        fZ*=oolen;
        
        dataContainer[y*3*mapSize+x*3+0] = GLubyte((((fX)+1.0f)/2.0f)*255.0f);
        dataContainer[y*3*mapSize+x*3+1] = GLubyte((((fY)+1.0f)/2.0f)*255.0f);
        dataContainer[y*3*mapSize+x*3+2] = GLubyte((((fZ)+1.0f)/2.0f)*255.0f);
      }
    }
    
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + 4,
                 currentLevel, GL_RGB8, mapSize, mapSize,
                 0, GL_RGB, GL_UNSIGNED_BYTE, dataContainer);
                 
    for (y=0;y<mapSize;y++)
    {
      for (x=0;x<mapSize;x++)
      {
        fX    = (float)halfMapSize - x;
        fY    = (float)halfMapSize - y;
        fZ    = (float)-halfMapSize;
        oolen = 1.0f/fastSquareRoot(fX*fX+fY*fY+fZ*fZ);
        
        fX*=oolen;
        fY*=oolen;
        fZ*=oolen;
        
        dataContainer[y*3*mapSize+x*3+0] = GLubyte((((fX)+1.0f)/2.0f)*255.0f);
        dataContainer[y*3*mapSize+x*3+1] = GLubyte((((fY)+1.0f)/2.0f)*255.0f);
        dataContainer[y*3*mapSize+x*3+2] = GLubyte((((fZ)+1.0f)/2.0f)*255.0f);
      }
    }
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + 5,
                 currentLevel, GL_RGB8, mapSize, mapSize,
                 0, GL_RGB, GL_UNSIGNED_BYTE, dataContainer);
                 
    deleteArray(dataContainer);
  }
  this->height = size;
  this->width  = size;
  depth        = 1;
  return finalizeLoading(name);
}

bool Texture::loadTextureFace(const  Image &image,
                              GLuint target,
                              bool   mipmap)
{
  if (!id)
    return Logger::writeErrorLog("Invalid texture id, Texture::loadTextureFace");
    
  if ((this->target != GL_TEXTURE_2D) && (this->target != GL_TEXTURE_CUBE_MAP))
    return Logger::writeErrorLog("Unrecognized texture target, Texture::loadTextureFace");
    
  GLuint quickFix = 0;
  switch (target)
  {
    case GL_TEXTURE_2D:
      quickFix = GL_TEXTURE_2D;
      break;
      
    case GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB:
      quickFix = GL_TEXTURE_CUBE_MAP_ARB;
      break;
  }
  
  switch (image.getInternalFormat())
  {
    case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
      if (GLEE_ARB_texture_compression &&
          GLEE_EXT_texture_compression_s3tc)
      {
      
        if (GLEE_SGIS_generate_mipmap && mipmap)
        {
          glTexParameteri(quickFix, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
          glTexParameteri(quickFix, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
        }
        
        glCompressedTexImage2DARB(target,
                                  0,
                                  GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,
                                  image.getWidth(), image.getHeight(),
                                  0,
                                  ((image.getWidth() +3)/4)*((image.getHeight() +3)/4)*16,
                                  image.getDataBuffer());
      }
      else
      {
        destroy();
        return Logger::writeErrorLog("GL_EXT_texture_compression_s3tc not supported");
      }
      break;
      
    case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
      if (GLEE_ARB_texture_compression &&
          GLEE_EXT_texture_compression_s3tc)
      {
      
        if (GLEE_SGIS_generate_mipmap && mipmap)
        {
          glTexParameteri(quickFix, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
          glTexParameteri(quickFix, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
        }
        
        glCompressedTexImage2DARB(target,
                                  0,
                                  GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,
                                  image.getWidth(), image.getHeight(),
                                  0,
                                  ((image.getWidth() +3)/4)*((image.getHeight() +3)/4)*16,
                                  image.getDataBuffer());
      }
      else
      {
        destroy();
        return Logger::writeErrorLog("GL_EXT_texture_compression_s3tc not supported");
      }
      break;
      
    case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
      if (GLEE_ARB_texture_compression &&
          GLEE_EXT_texture_compression_s3tc)
      {
        if (GLEE_SGIS_generate_mipmap && mipmap)
        {
          glTexParameteri(quickFix, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
          glTexParameteri(quickFix, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
        }
        
        glCompressedTexImage2DARB(target,
                                  0,
                                  GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,
                                  image.getWidth(), image.getHeight(),
                                  0,
                                  ((image.getWidth() +3)/4)*((image.getHeight() +3)/4)*8,
                                  image.getDataBuffer());
      }
      else
      {
        destroy();
        return Logger::writeErrorLog("GL_EXT_texture_compression_s3tc not supported");
      }
      break;
      
    default:
      if (mipmap)
        gluBuild2DMipmaps(target,
                          image.getInternalFormat(),
                          image.getWidth(),
                          image.getHeight(),
                          image.getFormat(),
                          GL_UNSIGNED_BYTE,
                          image.getDataBuffer());
      else
        glTexImage2D(target, 0,  image.getInternalFormat(), image.getWidth(),
                     image.getHeight(), 0, image.getFormat(),
                     GL_UNSIGNED_BYTE, image.getDataBuffer());
                     
  }
  return true;
}