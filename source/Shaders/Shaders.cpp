#include "Shaders.h"
#include "../Math/Tuple4.h"

Shaders::Shaders()
{
  linkedProcessor  = NULL;
}

Shaders::Shaders(const Shaders &copy)
{
  operator =(copy);
}

Shaders &Shaders::operator=(const Shaders &copy)
{
  if (this != &copy)
  {
    if (copy.linkedProcessor)
      linkedProcessor = &vertexProcessor;
    else
      linkedProcessor = NULL;
      
    vertexProcessor   = copy.vertexProcessor;
    fragmentProcessor = copy.fragmentProcessor;
  }
  return *this;
}

bool Shaders::loadXMLFile(const char * xmlFile,
                          const char *vertexNode,
                          const char *fragmentNode,
                          bool  link)
{
  const char* xmlPath    = MediaPathManager::lookUpMediaPath(xmlFile);
  XMLElement  *child     = NULL,
                           *subChild  = NULL;
  XMLStack     shaderStack;
  
  if (!xmlPath)
    return Logger::writeErrorLog(String("Couldn't locate file at -> ") + xmlFile);
    
  if (!shaderStack.loadXMLFile(xmlPath))
    return Logger::writeErrorLog(String("Invalid XML file -> ") + xmlFile);
    
  if (!(child = shaderStack.getChildByName("Shaders")))
    return  Logger::writeErrorLog("Need a Shaders root in the XML shaders file");
    
  xmlShaderPath = xmlPath;
  
  return loadXMLSettings(child, vertexNode, fragmentNode, link);
}

bool Shaders::loadXMLSettings(XMLElement *child,
                              const char *vertexNodeDesc,
                              const char *fragmentNodeDesc,
                              bool  link)
{
  if (!child)
    return  Logger::writeErrorLog("NULL Shaders node");
    
  XMLElement *linker  = child->getChildByName("linkProcessors");
  link                = linker ? (linker->getValue() == "true") : link;
  
  String      shaderName;
  int         success    = 1,
                           programID  = 0;
  destroy();
  
  XMLElement *fragmentNode = retrieveProcessorNode(child, fragmentNodeDesc, false),
                             *vertexNode   = retrieveProcessorNode(child, vertexNodeDesc,   true);
                             
  if (vertexNode)
  {
    int vertProType = estimateProcessorType(vertexNode, true);
    
    switch (vertProType)
    {
      case VERTEX_PROGRAM:
      case VERTEX_PROGRAM_NV:
        success    *= loadProgramCode(vertexNode, vertProType);
        shaderName  = xmlShaderPath;
        shaderName += (vertProType == VERTEX_PROGRAM) ? " VERTEX PROGRAM" :
                      " VERTEX PROGRAM NV";
        vertexProcessor.setName(shaderName);
        break;
        
      case VERTEX_SHADER:
        if (!link)
          programID = glCreateProgramObjectARB();
          
        success    *= loadShadersCode(vertexNode, VERTEX_SHADER);
        
        shaderName  = xmlShaderPath;
        shaderName += " VERTEX SHADER";
        vertexProcessor.setName(shaderName);
        if (!link)
        {
          if (success)
          {
            glAttachObjectARB(programID, vertexProcessor.getShaderID());
            glDeleteObjectARB(vertexProcessor.getShaderID());
            vertexProcessor.setShaderID(programID);
          }
          else
          {
            glDeleteObjectARB(programID);
            programID = 0;
          }
        }
        break;
    }
  }
  
  if (fragmentNode)
  {
    int fragProType = estimateProcessorType(fragmentNode, false);
    switch (fragProType)
    {
      case FRAGMENT_PROGRAM:
      case FRAGMENT_PROGRAM_NV:
        success    *= loadProgramCode(fragmentNode, fragProType);
        shaderName  = xmlShaderPath;
        shaderName += (fragProType == FRAGMENT_PROGRAM) ? " FRAGMENT PROGRAM" :
                      " FRAGMENT PROGRAM NV";
        fragmentProcessor.setName(shaderName);
        break;
        
      case FRAGMENT_SHADER:
        if (!link && !programID)
          programID = glCreateProgramObjectARB();
          
        success    *= loadShadersCode(fragmentNode, FRAGMENT_SHADER);
        shaderName  = xmlShaderPath;
        shaderName += " FRAGMENT SHADER";
        fragmentProcessor.setName(shaderName);
        
        if (!link)
        {
          if (success)
          {
            glAttachObjectARB(programID, fragmentProcessor.getShaderID());
            glDeleteObjectARB(fragmentProcessor.getShaderID());
            fragmentProcessor.setShaderID(programID);
          }
          else
          {
            glDeleteObjectARB(programID);
            programID = 0;
          }
        }
        break;
    }
  }
  
  if (link && vertexProcessor.link(fragmentProcessor))
  {
    Logger::writeInfoLog(String("Successfully linked the shaders -> ") + xmlShaderPath);
    linkedProcessor = &vertexProcessor;
  }
  
  if (!success)
    return Logger::writeErrorLog(String("Error in shaders -> ") + xmlShaderPath);
    
  if (programID)
  {
    if (success)
      glLinkProgramARB(programID);
    else
      glDeleteObjectARB(programID);
  }
  
  return (success != 0);
}

bool Shaders::loadShaderFromMemory(ShaderAttributes *shader,
                                   const char       *shaderCode,
                                   int               shaderType)
{
  GLuint shaderTrueType = 0,
                          shaderID       = 0;
                          
  if (!shaderCode || !shader)
    return Logger::writeErrorLog("NULL shader or code string");
    
  switch (shaderType)
  {
    case VERTEX_PROGRAM:
      if (!GLEE_ARB_vertex_program)
        return Logger::writeErrorLog("GL_ARB_vertex_program is not supported");
      shaderTrueType = GL_VERTEX_PROGRAM_ARB;
      break;
      
    case FRAGMENT_PROGRAM:
      if (!GLEE_ARB_fragment_program)
        return Logger::writeErrorLog("GL_ARB_fragment_program is not supported");
      shaderTrueType = GL_FRAGMENT_PROGRAM_ARB;
      break;
      
    case VERTEX_SHADER:
      if (!GLEE_ARB_vertex_shader)
        return Logger::writeErrorLog("GL_ARB_vertex_shader is not supported");
      shaderTrueType = GL_VERTEX_SHADER_ARB;
      break;
      
    case FRAGMENT_SHADER:
      if (!GLEE_ARB_fragment_shader)
        return Logger::writeErrorLog("GL_ARB_fragment_shader is not supported");
      shaderTrueType = GL_FRAGMENT_SHADER_ARB;
      break;
    default:
      return Logger::writeErrorLog("Unknown or supported shader type");
  }
  
  String error;
  int    length    = GLint(strlen(shaderCode)),
                     errorLog  = GL_FALSE;
                     
  switch (shaderType)
  {
    case VERTEX_PROGRAM:
    case FRAGMENT_PROGRAM:
      glGenProgramsARB(1, &shaderID);
      glBindProgramARB(shaderTrueType, shaderID);
      glProgramStringARB(shaderTrueType, GL_PROGRAM_FORMAT_ASCII_ARB,
                         length, shaderCode);
                         
      error = (char *)glGetString(GL_PROGRAM_ERROR_STRING_ARB);
      
      if (error.getLength() || !shaderID)
        return Logger::writeErrorLog(error);
      break;
      
    case VERTEX_SHADER:
    case FRAGMENT_SHADER:
      shaderID   = glCreateShaderObjectARB(shaderTrueType);
      glShaderSourceARB(shaderID, 1, (const GLcharARB **)&shaderCode, &length);
      glCompileShaderARB(shaderID);
      
      ShaderAttributes::printObjectInfoLog(" ", shaderID);
      glGetObjectParameterivARB(shaderID, GL_OBJECT_COMPILE_STATUS_ARB, &errorLog);
      if (errorLog == GL_FALSE)
        return Logger::writeErrorLog("Error detected in shader");
      break;
  }
  
  shader->setShaderID(shaderID);
  shader->setShaderType(shaderType);
  
  return true;
}

bool Shaders::loadShadersCode(XMLElement *node, int shaderType)
{
  ShaderAttributes *shader = (shaderType == FRAGMENT_SHADER) ?
                             &fragmentProcessor :
                             (shaderType == VERTEX_SHADER)   ?
                             &vertexProcessor   : NULL;
                             
  if (!node)
    return false;
    
  if (!shader)
    return Logger::writeErrorLog(String("Unrecognized Shader type ->") + node->getName());
    
  XMLElement   *uniform          = NULL,
                                   *child            = NULL;
                                   
  GLubyte      *shaderCode       = NULL;
  
  Tuple4f       floatUniforms;
  Tuple4i       intUniforms;
  bool          floatUniform       = false;
  int           uniformSize        = 0;
  
  if (!GLEE_ARB_fragment_shader || !GLEE_ARB_vertex_shader)
    return Logger::writeErrorLog("GL_ARB_fragment_shader & GL_ARB_vertex_shader are not supported");
    
  shader->setShaderType(shaderType);
  
  for (size_t c = 0; c < node->getChildrenCount(); c++)
  {
    if (!(child =  node->getChild(c)))
      continue;
      
    const String &name = child->getName();
    
    if (name  == "RawData")
    {
      shaderCode = (GLubyte*)child->rawData.data;
      if (!loadShaderFromMemory(shader, (const char*)shaderCode, shaderType))
        return false;
    }
    
    if (name == "Uniform")
    {
      if (!(uniform = child->getChildByName("name")))
        continue;
        
      const String &  name = uniform->getValue();
      
      if (!(uniform = child->getChildByName("size"))) continue;
      uniformSize = uniform->getValuei();
      
      if (!(uniform = child->getChildByName("type"))) continue;
      floatUniform = (uniform->getValue() == "float");
      
      if (floatUniform)
      {
        floatUniforms.set(0.0f, 0.0f, 0.0f, 0.0f);
        XMLElement::loadRX_GY_BZ_AWf(*child, floatUniforms);
        switch (uniformSize)
        {
          case 1:
            shader->addAttributes1f(ShaderElements1f(name, floatUniforms));
            break;
          case 2:
            shader->addAttributes2f(ShaderElements2f(name, floatUniforms));
            break;
          case 3:
            shader->addAttributes3f(ShaderElements3f(name, floatUniforms));
            break;
          case 4:
            shader->addAttributes4f(ShaderElements4f(name, floatUniforms));
            break;
          case 16:
            shader->addAttributes16f(ShaderElements16f(name, NULL));
            break;
          default:
            Logger::writeErrorLog("Size of shader's uniform unspecified or not valid");
        }
      }
      else
      {
        intUniforms.set(0, 0, 0, 0);
        XMLElement::loadRX_GY_BZ_AWi(*child, intUniforms);
        
        switch (uniformSize)
        {
          case 1:
            shader->addAttributes1i(ShaderElements1i(name, intUniforms));
            break;
          case 2:
            shader->addAttributes2i(ShaderElements2i(name, intUniforms));
            break;
          case 3:
            shader->addAttributes3i(ShaderElements3i(name, intUniforms));
            break;
          case 4:
            shader->addAttributes4i(ShaderElements4i(name, intUniforms));
            break;
          default:
            return Logger::writeErrorLog("Size of shader's uniform unspecified");
        }
      }
    }
  }
  
  return true;
}

bool Shaders::loadProgramCode(XMLElement *node, int shaderType)
{
  ShaderAttributes *shader = (shaderType == FRAGMENT_PROGRAM) ?
                             &fragmentProcessor :
                             (shaderType == VERTEX_PROGRAM)   ?
                             &vertexProcessor   : NULL;
                             
  if (!node)
    return false;
    
  if (!shader)
    return Logger::writeErrorLog(String("Unrecognized Shader type ->") + node->getName());
    
  XMLElement  *child    = NULL,
                          *subchild = NULL;
                          
  for (size_t i = 0; i < node->getChildrenCount(); i++)
  {
    child = node->getChild(i);
    const String &name = child->getName();
    
    if (name == "RawData")
    {
      if (!loadShaderFromMemory(shader, (const char*)child->rawData.data, shaderType))
        return false;
      continue;
    }
    
    if (name == "Attributes")
    {
      ShaderElements4f elements;
      Tuple4f          attributes;
      
      if (subchild = child->getChildByName("local"))
        elements.setLocal(subchild->getValue() == "true");
        
      if (subchild = child->getChildByName("name"))
        elements.setName(subchild->getValuec());
        
      XMLElement::loadRX_GY_BZ_AWf(*child, attributes);
      elements.setElements(attributes);
      shader->addAttributes4f(elements);
      continue;
    }
  }
  return (shader->getShaderID() != 0);
}

const bool Shaders::activate()
{
  if (linkedProcessor)
    return linkedProcessor->activate();
    
  bool active1  = fragmentProcessor.activate(),
                  active2  = vertexProcessor.activate();
  return active1 || active2;
}

const bool Shaders::deactivate()
{
  if (linkedProcessor)
    return linkedProcessor->deactivate();
    
  bool active1  = fragmentProcessor.deactivate(),
                  active2  = vertexProcessor.deactivate();
  return active1 || active2;
}

bool Shaders::updateElementsi(const char* name, int size, int   *elements)
{
  if (!size || !elements || !name)
    return false;
    
  bool vertexResult   = false,
                        fragmentResult = false;
  switch (size)
  {
    case 1:
      if (vertexResult   = (NULL != (attrib1i = vertexProcessor.getAttributes1i(name))))
        attrib1i->setElements(elements);
      if (fragmentResult = (NULL != (attrib1i = fragmentProcessor.getAttributes1i(name))))
        attrib1i->setElements(elements);
      break;
    case 2:
      if (vertexResult   = (NULL != (attrib2i = vertexProcessor.getAttributes2i(name))))
        attrib2i->setElements(elements);
      if (fragmentResult = (NULL != (attrib2i = fragmentProcessor.getAttributes2i(name))))
        attrib2i->setElements(elements);
      break;
    case 3:
      if (vertexResult   = (NULL != (attrib3i = vertexProcessor.getAttributes3i(name))))
        attrib3i->setElements(elements);
      if (fragmentResult = (NULL != (attrib3i = fragmentProcessor.getAttributes3i(name))))
        attrib3i->setElements(elements);
      break;
    case 4:
      if (vertexResult   = (NULL != (attrib4i = vertexProcessor.getAttributes4i(name))))
        attrib4i->setElements(elements);
      if (fragmentResult = (NULL != (attrib4i = fragmentProcessor.getAttributes4i(name))))
        attrib4i->setElements(elements);
      break;
  }
  
  return (fragmentResult || vertexResult);
}

bool Shaders::updateElementsf(const char* name, int size, float *elements)
{
  if (!size || !elements || !name)
    return false;
    
  bool vertexResult   = false,
                        fragmentResult = false;
  switch (size)
  {
    case 1:
      if (vertexResult   = (NULL != (attrib1f = vertexProcessor.getAttributes1f(name))))
        attrib1f->setElements(elements);
      if (fragmentResult = (NULL != (attrib1f = fragmentProcessor.getAttributes1f(name))))
        attrib1f->setElements(elements);
      break;
    case 2:
      if (vertexResult   = (NULL != (attrib2f = vertexProcessor.getAttributes2f(name))))
        attrib2f->setElements(elements);
      if (fragmentResult = (NULL != (attrib2f = fragmentProcessor.getAttributes2f(name))))
        attrib2f->setElements(elements);
      break;
    case 3:
      if (vertexResult   = (NULL != (attrib3f = vertexProcessor.getAttributes3f(name))))
        attrib3f->setElements(elements);
      if (fragmentResult = (NULL != (attrib3f = fragmentProcessor.getAttributes3f(name))))
        attrib3f->setElements(elements);
      break;
    case 4:
      if (vertexResult   = (NULL != (attrib4f = vertexProcessor.getAttributes4f(name))))
        attrib4f->setElements(elements);
        
      if (fragmentResult = (NULL != (attrib4f = fragmentProcessor.getAttributes4f(name))))
        attrib4f->setElements(elements);
        
      break;
    case 16:
      if (vertexResult   = (NULL != (attrib16f = vertexProcessor.getAttributes16f(name))))
        attrib16f->setElements(elements);
      if (fragmentResult = (NULL != (attrib16f = fragmentProcessor.getAttributes16f(name))))
        attrib16f->setElements(elements);
      break;
  }
  return (fragmentResult || vertexResult);
}

const bool Shaders::isFunctional()
{
  return (vertexProcessor.getShaderID() || fragmentProcessor.getShaderID());
}

void Shaders::destroy()
{
  linkedProcessor = NULL;
  vertexProcessor.clear();
  fragmentProcessor.clear();
}

Shaders::~Shaders()
{
  destroy();
}

XMLElement *Shaders::retrieveProcessorNode(XMLElement *node, const char*description, bool vertex)
{
  if (!node)
  {
    Logger::writeErrorLog("NULL Shaders node, Shaders::retrieveProcessorNode");
    return NULL;
  }
  
  if (node->getName() != "Shaders")
  {
    Logger::writeErrorLog("Shaders node required, Shaders::retrieveProcessorNode");
    return NULL;
  }
  
  XMLElement *child = node->getChildByName(description);
  
  if (!child)
  {
    if (vertex)
    {
      if (child = node->getChildByName("vertexProcessor"))
        return node->getChildByName(child->getValue());
    }
    else
    {
      if (child = node->getChildByName("fragmentProcessor"))
        return node->getChildByName(child->getValue());
    }
  }
  return child;
}

int Shaders::estimateProcessorType(XMLElement *node,
                                   bool        vertexProcessor)
{
  if (!node)
    return UNDEFINED;
    
  XMLElement *rawData  = node->getChildByName("RawData");
  
  if (!rawData)
  {
    Logger::writeErrorLog("Processor node requires RawData element, Shaders::estimateProcessorType");
    return UNDEFINED;
  }
  
  const char *shaderData = rawData->rawData.data;
  
  if (!shaderData)
  {
    Logger::writeErrorLog("NULL Shader data, Shaders::estimateProcessorType");
    return UNDEFINED;
  }
  
  if (vertexProcessor)
  {
    if (strstr(shaderData, "main") || strstr(shaderData, "gl_Position"))
      return VERTEX_SHADER;
      
    if (strstr(shaderData, "!!ARBvp1.0"))
      return VERTEX_PROGRAM;
      
    if (strstr(shaderData, "!!VP1.0"))
      return VERTEX_PROGRAM_NV;
  }
  else
  {
    if (strstr(shaderData, "main") || strstr(shaderData, "gl_FragColor"))
      return FRAGMENT_SHADER;
      
    if (strstr(shaderData, "!!ARBfp1.0"))
      return FRAGMENT_PROGRAM;
      
    if (strstr(shaderData, "!!FP1.0"))
      return FRAGMENT_PROGRAM_NV;
  }
  
  return UNDEFINED;
}

ShaderAttributes *Shaders::getLinkedProcessor()
{
  return linkedProcessor;
}
ShaderAttributes &Shaders::getVertexProcessor()
{
  return vertexProcessor;
}
ShaderAttributes &Shaders::getFragmentProcessor()
{
  return fragmentProcessor;
}