#include "TextureCoordGenerator.h"
#include "../Managers/MediaPathManager.h"
#include "../Math/Matrix4.h"

TextureCoordGenerator::TextureCoordGenerator() : IOXMLObject("TextureCoordGenerator")
{
  memcpy(planes, Matrix4f(), sizeof(Matrix4f));
  enabled = true;
  format  =   -1;
  mode    =   -1;
}

TextureCoordGenerator::TextureCoordGenerator(GLuint formatArg, GLuint modeArg)
    : IOXMLObject("TextureCoordGenerator")
{
  memcpy(planes, Matrix4f(), sizeof(Matrix4f));
  enabled = true;
  format  = formatArg;
  mode    = modeArg;
}

TextureCoordGenerator::TextureCoordGenerator(const TextureCoordGenerator& copy)
    : IOXMLObject("TextureCoordGenerator")
{
  this->operator =(copy);
}

TextureCoordGenerator &TextureCoordGenerator::operator = (const TextureCoordGenerator& copy)
{
  if (this != &copy)
  {
    memcpy(planes, copy.planes, sizeof(Tuple4f)*4);
    enabled = copy.enabled;
    format  = copy.format;
    mode    = copy.mode;
  }
  return *this;
}

bool TextureCoordGenerator::loadXMLSettings(XMLElement *element)
{
  if (!isSuitable(element))
    return false;
    
  XMLElement  *child      = NULL,
                            *subChild   = NULL;
  int          planeIndex = 0;
  
  if (child = element->getChildByName("description"))
    return IOXMLObject::loadXMLFile(child->getValuec());
    
  for (size_t i = 0; i < element->getChildrenCount(); i++)
  {
    if (!(child = element->getChild(i)))
      continue;
      
    const String &childName = child->getName();
    
    if (childName == "format")
    {
      setFormat(child->getValuei());
      continue;
    }
    
    if (childName == "mode")
    {
      setMode(child->getValuec());
      continue;
    }
    
    if (childName == "enabled")
    {
      enabled = (child->getValue() == "true");
      continue;
    }
    
    if (childName == "Plane" && (planeIndex <4))
    {
      XMLElement::loadRX_GY_BZ_AWf(*child, planes[planeIndex++]);
      continue;
    }
  }
  
  return true;
}

bool TextureCoordGenerator::exportXMLSettings(ofstream &xmlFile)
{
  if (!xmlFile.is_open())
    return Logger::writeErrorLog("Cannot export TextureCoordGenerator to XML: file not ready.");
    
  if (mode == -1 || format <= 0)
    return Logger::writeErrorLog("Cannot export TextureCoordGenerator to XML: invalid mode or format.");
    
  xmlFile << "  <TextureCoordGenerator enabled = \"" << (enabled? "true" : "false") << "\"\n"
  << "                         format  = \"" << format            << "\"\n"
  << "                         mode    = \""
  << ((mode == GL_REFLECTION_MAP) ? "REFLECTION_MAP\"" :
      (mode == GL_OBJECT_LINEAR) ? "OBJECT_LINEAR\""  :
      (mode == GL_EYE_LINEAR) ? "EYE_LINEAR\""     :
      (mode == GL_NORMAL_MAP) ? "NORMAL_MAP\""     : "SPHERE_MAP\"") << ">\n"
  << "    <Plane x = \"" << planes[0].x << "\" y = \""
  << planes[0].y << "\" z = \""
  << planes[0].z << "\" w = \""
  << planes[0].w << "\" />\n"
  << "    <Plane x = \"" << planes[1].x << "\" y = \""
  << planes[1].y << "\" z = \""
  << planes[1].z << "\" w = \""
  << planes[1].w << "\" />\n"
  << "    <Plane x = \"" << planes[2].x << "\" y = \""
  << planes[2].y << "\" z = \""
  << planes[2].z << "\" w = \""
  << planes[2].w << "\" />\n"
  << "    <Plane x = \"" << planes[3].x << "\" y = \""
  << planes[3].y << "\" z = \""
  << planes[3].z << "\" w = \""
  << planes[3].w << "\" />\n"
  << "  </TextureCoordGenerator>\n";
  return true;
}

void TextureCoordGenerator::setFormat(int formatArg)
{
  format = clamp(formatArg, 1, 4);
  if (format != formatArg)
    Logger::writeErrorLog("Generation format must be [1, 4], TextureCoordGenerator::setFormat()");
}

void TextureCoordGenerator::setMode(const char* modeArg)
{
  if (!modeArg)
    return;
    
  if (!strcmp(modeArg, "REFLECTION_MAP"))
  {
    mode = GL_REFLECTION_MAP;
    return;
  }
  if (!strcmp(modeArg, "OBJECT_LINEAR"))
  {
    mode = GL_OBJECT_LINEAR;
    return;
  }
  if (!strcmp(modeArg, "EYE_LINEAR"))
  {
    mode = GL_EYE_LINEAR;
    return;
  }
  if (!strcmp(modeArg, "NORMAL_MAP"))
  {
    mode = GL_NORMAL_MAP;
    return;
  }
  if (!strcmp(modeArg, "SPHERE_MAP"))
  {
    mode = GL_SPHERE_MAP;
    return;
  }
  
  mode = -1;
  Logger::writeErrorLog("Unrecognized XML generation mode, TextureCoordGenerator::setMode()");
}

void TextureCoordGenerator::setMode(int modeArg)
{
  switch (modeArg)
  {
    case GL_REFLECTION_MAP:
    case GL_OBJECT_LINEAR:
    case GL_EYE_LINEAR:
    case GL_NORMAL_MAP:
    case GL_SPHERE_MAP:
      mode = modeArg;
      return;
  }
  mode = -1;
  Logger::writeErrorLog("Unrecognized generation mode, TextureCoordGenerator::setMode()");
}

void TextureCoordGenerator::setPlane(int index, const Tuple4f &t)
{
  if (index  < 0 || index > 3)
    return;
    
  planes[index] = t;
}

const  Tuple4f &TextureCoordGenerator::getPlane(int index) const
{
  return (index  > -1 && index < 4) ?  planes[index] : planes[0];
}

bool TextureCoordGenerator::planarMode()
{
  return (mode ==  GL_EYE_LINEAR) ||
         (mode ==  GL_OBJECT_LINEAR);
}

void TextureCoordGenerator::startGeneration()
{
  if (mode != -1 && enabled)
  {
    int plane = GL_EYE_LINEAR ? GL_EYE_PLANE : GL_OBJECT_PLANE;
    
    stopGeneration();
    switch (format)
    {
      case 1:
        glEnable(GL_TEXTURE_GEN_S);
        glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, mode);
        
        if (planarMode())
          glTexGenfv(GL_S, plane, planes[0]);
          
        break;
      case 2:
        glEnable(GL_TEXTURE_GEN_S);
        glEnable(GL_TEXTURE_GEN_T);
        glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, mode);
        glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, mode);
        
        if (planarMode())
        {
          glTexGenfv(GL_S, plane, planes[0]);
          glTexGenfv(GL_T, plane, planes[1]);
        }
        
        break;
      case 3:
        glEnable(GL_TEXTURE_GEN_S);
        glEnable(GL_TEXTURE_GEN_T);
        glEnable(GL_TEXTURE_GEN_R);
        
        glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, mode);
        glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, mode);
        glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, mode);
        
        if (planarMode())
        {
          glTexGenfv(GL_S, plane, planes[0]);
          glTexGenfv(GL_T, plane, planes[1]);
          glTexGenfv(GL_R, plane, planes[2]);
        }
        break;
      case 4:
        glEnable(GL_TEXTURE_GEN_S);
        glEnable(GL_TEXTURE_GEN_T);
        glEnable(GL_TEXTURE_GEN_R);
        glEnable(GL_TEXTURE_GEN_Q);
        glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, mode);
        glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, mode);
        glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, mode);
        glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, mode);
        
        if (planarMode())
        {
          glTexGenfv(GL_S, plane, planes[0]);
          glTexGenfv(GL_T, plane, planes[1]);
          glTexGenfv(GL_R, plane, planes[2]);
          glTexGenfv(GL_Q, plane, planes[3]);
        }
        break;
    }
  }
}

void TextureCoordGenerator::stopGeneration()
{
  glDisable(GL_TEXTURE_GEN_S);
  glDisable(GL_TEXTURE_GEN_T);
  glDisable(GL_TEXTURE_GEN_R);
  glDisable(GL_TEXTURE_GEN_Q);
}

void TextureCoordGenerator::enableGeneration(bool on)
{
  enabled = on;
}

bool TextureCoordGenerator::generationOn()
{
  return enabled;
}

