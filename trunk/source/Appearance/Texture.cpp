#include "Texture.h"
#include "../Renderer/Renderer.h"

Texture::Texture(GLuint target_) : IOXMLObject("Texture")
{
  currUnit       = -1;
  pBuffer        = 0;
  target         = target_;
  height         = 0;
  width          = 0;
  depth          = 0;
  aniso          = 0;
  id             = 0;
}

Texture::Texture(const Texture & texture) : IOXMLObject("Texture")
{
  this->operator=(texture);
}

Texture &Texture::operator= (const Texture & copy)
{
  if (this != &copy)
  {
    setID(copy.id);
    
    textureCoordGenerator = copy.textureCoordGenerator;
    transform    = copy.transform;
    currUnit     = copy.currUnit;
    pBuffer      = copy.pBuffer;
    target       = copy.target;
    height       = copy.height;
    width        = copy.width;
    depth        = copy.depth;
    aniso        = copy.aniso;
  }
  return *this;
}

bool Texture::checkForRepeat(const char* string)
{
  TextureInfo *textureInfo = 0;
  
  if (textureInfo = TexturesManager::getTextureInfo(string))
    setID(textureInfo->getMedia());
    
  return (textureInfo != 0);
}

bool Texture::finalizeLoading(const char* string)
{
  if (!id)
    return Logger::writeErrorLog(String("Failed to load texture <") + string + ">");
  else
  {
    TextureInfo *textureInfo = new TextureInfo(string, id);
    TexturesManager::addTextureInfo(textureInfo);
  }
  return true;
}

void Texture::doTransform()
{
  textureCoordGenerator.startGeneration();
  
  glMatrixMode(GL_TEXTURE);
  transform.doTransform();
  glMatrixMode(GL_MODELVIEW);
}

void Texture::undoTransform()
{
  textureCoordGenerator.stopGeneration();
  
  glMatrixMode(GL_TEXTURE);
  transform.undoTransform();
  glMatrixMode(GL_MODELVIEW);
}

void Texture::setTextureCoordinatesGenerator(TextureCoordGenerator &texGenerator)
{
  textureCoordGenerator = texGenerator;
}

void Texture::associateWithPBuffer(GLPBuffer *buffer)
{
  pBuffer = buffer;
}

const bool Texture::activate(int unit)
{
  if (!id)
    return false;
    
  if (unit > -1)
    glActiveTextureARB(GL_TEXTURE0_ARB + unit);
    
  glEnable(target);
  glBindTexture(target, id);
  
  if (pBuffer)
    wglBindTexImageARB(pBuffer->getPBufferHandle(), WGL_FRONT_LEFT_ARB);
    
  doTransform();
  currUnit = unit;
  return true;
}

void Texture::copyCurrentBuffer(int newTarget)
{
  if (!id)
    return;
    
  newTarget = (newTarget == -1) ? target : newTarget;
  activate();
  glCopyTexSubImage2D(newTarget, 0, 0, 0, 0, 0, width, height);
  deactivate();
}

const bool Texture::deactivate()
{
  if (id > 0)
  {
    if (currUnit > -1)
      glActiveTextureARB(GL_TEXTURE0_ARB + currUnit);
      
    undoTransform();
    glDisable(target);
    return true;
  }
  return false;
}

void Texture::setID(GLuint texID)
{
  static int unknown = 1;
  
  if (!texID)
  {
    Logger::writeErrorLog("invalid texID, Texture::setID(GLuint texID)");
    return;
  }
  
  TextureInfo *newTextureInfo = TexturesManager::getTextureInfo(texID);
  TextureInfo *oldTextureInfo = TexturesManager::getTextureInfo(id);
  
  if (newTextureInfo)
    newTextureInfo->increaseUserCount();
  else
  {
    newTextureInfo = new TextureInfo(String("Unknown texture #") + unknown, texID);
    unknown++;
  }
  
  if (oldTextureInfo)
    oldTextureInfo->decreaseUserCount();
    
  id = texID;
  
  GLint info;
  
  activate();
  glGetTexLevelParameteriv(target, 0, GL_TEXTURE_WIDTH , &info);
  width  = info;
  glGetTexLevelParameteriv(target, 0, GL_TEXTURE_HEIGHT, &info);
  height = info;
  deactivate();
}

const GLuint Texture::getID() const
{
  return id;
}

TextureCoordGenerator &Texture::getTextureCoordinatesGenerator()
{
  return textureCoordGenerator;
}

void Texture::setTarget(GLuint target_)
{
  target = target_;
}

const GLuint  Texture::getTarget() const
{
  return target;
}

void Texture::setTransform(const Transform3D &transform_)
{
  transform = transform_;
}

Transform3D &Texture::getTransform()
{
  return transform;
}

const GLuint Texture::getHeight() const
{
  return height;
}
const GLuint Texture::getWidth()  const
{
  return width;
}
const GLuint Texture::getDepth()  const
{
  return depth;
}

bool Texture::loadXMLSettings(XMLElement *element)
{
  if (!isSuitable(element))
    return false;
    
  XMLElement  *child     = 0,
                           *subChild  = 0;
                           
  String       path;
  int          aniso     =        0,
                                  clampS    = GL_CLAMP,
                                              clampT    = GL_CLAMP,
                                                          clampR    = GL_CLAMP,
                                                                      magFilter = GL_LINEAR,
                                                                                  minFilter = GL_LINEAR;
  bool         mipmap    = false,
                           result    = false;
                           
  if (child = element->getChildByName("description"))
    return IOXMLObject::loadXMLFile(child->getValuec());
    
  for (size_t i = 0; i < element->getChildrenCount(); i++)
  {
    if (!(child = element->getChild(i)))
      continue;
      
    const String &childName = child->getName();
    
    if (childName == "path")
    {
      path = child->getValuec();
      continue;
    }
    
    if (childName == "type")
    {
      target =  getXMLType(child->getValue());
      continue;
    }
    
    if (childName == "mipmap")
    {
      mipmap = (child->getValue() == "true");
      continue;
    }
    
    if (childName == "Wrap")
    {
      if ((subChild = child->getChildByName("s")) ||
          (subChild = child->getChildByName("u")))
        clampS  = getXMLWrapMode(subChild->getValue());
        
      if ((subChild = child->getChildByName("t")) ||
          (subChild = child->getChildByName("v")))
        clampT  = getXMLWrapMode(subChild->getValue());
        
      if ((subChild = child->getChildByName("r")) ||
          (subChild = child->getChildByName("w")))
        clampR  = getXMLWrapMode(subChild->getValue());
      continue;
    }
    
    if (childName == "Filter")
    {
      if (subChild = child->getChildByName("mag"))
        magFilter  = getXMLMagFilter(subChild->getValue());
        
      if (subChild = child->getChildByName("min"))
        minFilter  = getXMLMinFilter(subChild->getValue());
        
      if (subChild = child->getChildByName("aniso"))
        aniso  =  subChild->getValuei();
      continue;
    }
    
    if (childName == "TextureCoordGenerator")
    {
      textureCoordGenerator.loadXMLSettings(child);
      continue;
    }
  }
  
  result = (target == GL_TEXTURE_2D) ? load2D(path, clampS, clampT, magFilter, minFilter, mipmap) :
           (target == GL_TEXTURE_CUBE_MAP) ? loadCube(path, clampS, clampT, magFilter, minFilter, mipmap) :
           Logger::writeErrorLog("Unsupported texture type");
  if (result)
    setAnisoLevel(aniso);
  return result;
}

int Texture::getXMLMagFilter(const String &value)
{
  if (value.getLength())
  {
    if (value == "NEAREST") return GL_NEAREST;
  }
  return GL_LINEAR;
}

int Texture::getXMLMinFilter(const String &value)
{
  if (value.getLength())
  {
    if (value == "NEAREST_MIPMAP_NEAREST") return GL_NEAREST_MIPMAP_NEAREST;
    if (value == "LINEAR_MIPMAP_NEAREST")  return GL_LINEAR_MIPMAP_NEAREST;
    if (value == "NEAREST_MIPMAP_LINEAR")  return GL_NEAREST_MIPMAP_LINEAR;
    if (value == "LINEAR_MIPMAP_LINEAR")   return GL_LINEAR_MIPMAP_LINEAR;
    if (value == "NEAREST")                return GL_NEAREST;
  }
  return GL_LINEAR;
}

int Texture::getXMLWrapMode(const String &value)
{
  if (value.getLength())
  {
    if (value == "CLAMP_TO_BORDER") return GL_CLAMP_TO_BORDER;
    if (value == "CLAMP_TO_EDGE")   return GL_CLAMP_TO_EDGE;
    if (value == "REPEAT")          return GL_REPEAT;
  }
  return GL_CLAMP;
}

int Texture::getXMLType(const String &value)
{
  if (value.getLength())
  {
    if (value == "TEXTURE_1D")       return GL_TEXTURE_1D;
    if (value == "TEXTURE_3D")       return GL_TEXTURE_3D;
    if (value == "TEXTURE_CUBE_MAP") return GL_TEXTURE_CUBE_MAP;
  }
  return GL_TEXTURE_2D;
}

int Texture::getValidWrapMode(int clamp)
{
  return (clamp == GL_CLAMP) ? GL_CLAMP           :
         (clamp == GL_REPEAT) ? GL_REPEAT          :
         (clamp == GL_CLAMP_TO_EDGE) ? GL_CLAMP_TO_EDGE   :
         (clamp == GL_CLAMP_TO_BORDER) ? GL_CLAMP_TO_BORDER : GL_REPEAT;
}

int Texture::getValidMagFilter(int filter)
{
  return (filter == GL_NEAREST) ? GL_NEAREST  : GL_LINEAR;
}

int Texture::getValidMinFilter(int filter)
{
  return (filter == GL_NEAREST) ? GL_NEAREST                :
         (filter == GL_LINEAR_MIPMAP_LINEAR) ? GL_LINEAR_MIPMAP_LINEAR   :
         (filter == GL_NEAREST_MIPMAP_LINEAR) ? GL_NEAREST_MIPMAP_LINEAR  :
         (filter == GL_LINEAR_MIPMAP_NEAREST) ? GL_LINEAR_MIPMAP_NEAREST  :
         (filter == GL_NEAREST_MIPMAP_NEAREST) ? GL_NEAREST_MIPMAP_NEAREST : GL_LINEAR;
}

bool Texture::exportXMLSettings(ofstream &xmlFile)
{
  if (!xmlFile.is_open())
    return Logger::writeErrorLog("Cannot export Texture to XML: file not ready.");
    
  if (!id)
    return Logger::writeErrorLog("Cannot export Texture to XML: invalid texture handle.");
    
  TextureInfo *textureInfo  = TexturesManager::getTextureInfo(id);
  
  if (!textureInfo)
    return Logger::writeErrorLog("Cannot export Texture to XML: cannot locate a valid MediaInfo.");
    
  xmlFile << "  <Texture  type        = \""
  << ((target == GL_TEXTURE_2D) ? "TEXTURE_2D\"\n" :
      (target == GL_TEXTURE_1D) ? "TEXTURE_1D\"\n" :
      (target == GL_TEXTURE_3D) ? "TEXTURE_3D\"\n" : "TEXTURE_CUBE_MAP\"\n")
  << "            path        = \"" << textureInfo->getMediaPath() << "\"\n"
  << "            mipmap      = \"true\" > \n"
  << "    <Wrap   s           = \"REPEAT\" \n"
  << ((target == GL_TEXTURE_3D)  ? "          r           = \"REPEAT\" \n" : "")
  << "            t           = \"REPEAT\" />\n"
  << "    <Filter mag         = \"LINEAR\" \n"
  << "            min         = \"LINEAR_MIPMAP_LINEAR\"\n"
  << "            aniso       =  " << aniso << "\" /> \n";
  
  textureCoordGenerator.exportXMLSettings(xmlFile);
  
  xmlFile << "  </Texture>\n";
  return true;
}

void Texture::setAnisoLevel(int level)
{
  if (!id || !Renderer::getMaxAnisotropicFilter() || !level)
    return;
    
  aniso = clamp(getClosestPowerOfTwo(level), 0, Renderer::getMaxAnisotropicFilter());
  activate();
  glTexParameteri(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, level);
  deactivate();
}

GLuint Texture::getAnisoLevel()
{
  return aniso;
}

void Texture::destroy()
{
  TextureInfo *textureInfo = TexturesManager::getTextureInfo(id);
  
  if (textureInfo)
    textureInfo->decreaseUserCount();
  else
    glDeleteTextures(1, &id);
    
  TexturesManager::flushUnusedTextures();
  
  height         = 0;
  width          = 0;
  depth          = 0;
  id             = 0;
}

Texture::~Texture()
{
  destroy();
}



