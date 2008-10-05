#include "ShaderAttributes.h"

ShaderAttributes::ShaderAttributes(const char *name)
{
  clear();
  setName(!name ? "ShaderAttributes" : name);
}

ShaderAttributes::ShaderAttributes(const ShaderAttributes &copy)
{
  operator =(copy);
}

ShaderAttributes &ShaderAttributes::operator=(const ShaderAttributes &copy)
{
  if (this != &copy)
  {
    clear();
    
    localAttributes = copy.localAttributes;
    shaderType      = copy.shaderType;
    shaderID        = copy.shaderID;
    name            = copy.name;
    this->operator +=(copy);
  }
  return *this;
}

void  ShaderAttributes::operator +=(const ShaderAttributes &copy)
{
  size_t i = 0;
  
  for (i = 0; i < copy.attributes1f.size(); i++)
    addAttributes1f(copy.attributes1f[i]);
    
  for (i = 0; i < copy.attributes1i.size(); i++)
    addAttributes1i(copy.attributes1i[i]);
    
  for (i = 0; i < copy.attributes2f.size(); i++)
    addAttributes2f(copy.attributes2f[i]);
    
  for (i = 0; i < copy.attributes2i.size(); i++)
    addAttributes2i(copy.attributes2i[i]);
    
  for (i = 0; i < copy.attributes3f.size(); i++)
    addAttributes3f(copy.attributes3f[i]);
    
  for (i = 0; i < copy.attributes3i.size(); i++)
    addAttributes3i(copy.attributes3i[i]);
    
  for (i = 0; i < copy.attributes4f.size(); i++)
    addAttributes4f(copy.attributes4f[i]);
    
  for (i = 0; i < copy.attributes4i.size(); i++)
    addAttributes4i(copy.attributes4i[i]);
    
  for (i = 0; i < copy.attributes16f.size(); i++)
    addAttributes16f(copy.attributes16f[i]);
}

bool  ShaderAttributes::link(ShaderAttributes &secondPart)
{
  if (!shaderID)
    return Logger::writeErrorLog(String("Shader 1 has an invalid ID -> ") + name);
    
  if (!secondPart.shaderID)
    return Logger::writeErrorLog(String("Shader2 has an invalid ID -> ") + secondPart.getName());
    
  switch (shaderType)
  {
    case UNDEFINED:
    case VERTEX_PROGRAM:
    case FRAGMENT_PROGRAM:
    case LINKED_SHADER:
      return Logger::writeErrorLog(String("Shader1 is incompatible, unable to link -> ") + name);
  }
  
  switch (secondPart.shaderType)
  {
    case UNDEFINED:
    case VERTEX_PROGRAM:
    case FRAGMENT_PROGRAM:
    case LINKED_SHADER:
      return Logger::writeErrorLog(String("Shader2 is incompatible, unable to link -> ") + secondPart.getName());
  }
  
  GLuint linkedShader = glCreateProgramObjectARB();
  GLint  errorLog     = GL_FALSE;
  
  glAttachObjectARB(linkedShader, shaderID);
  glAttachObjectARB(linkedShader, secondPart.shaderID);
  glLinkProgramARB(linkedShader);
  
  printObjectInfoLog(NULL, linkedShader);
  glGetObjectParameterivARB(linkedShader, GL_OBJECT_LINK_STATUS_ARB,  &errorLog);
  
  if (errorLog == GL_FALSE)
    return Logger::writeErrorLog(String("Error while linking shaders <") + name + ">and <"
                                 +  secondPart.getName() + ">");
  else
  {
    glDeleteObjectARB(shaderID);
    glDeleteObjectARB(secondPart.shaderID);
  }
  
  shaderID   = linkedShader;
  shaderType = LINKED_SHADER;
  this->operator +=(secondPart);
  secondPart.clear();
  
  return true;
}

void ShaderAttributes::printObjectInfoLog(const char* file, int ID)
{
  GLint bufferSize = 0,
                     dummy      = 0;
                     
  glGetObjectParameterivARB(ID, GL_OBJECT_INFO_LOG_LENGTH_ARB, &bufferSize);
  
  if (bufferSize  <= 1)
    return;
    
  char* logBuffer = new char[bufferSize];
  
  glGetInfoLogARB(ID, bufferSize, &dummy, logBuffer);
  
  String log;
  
  if (file)
    log +="Info Log of Shader Object:\n";
  else
    log +="Shader Program Info Log:\n" ;
    
  log += logBuffer;
  
  Logger::writeErrorLog(log);
}

void  ShaderAttributes::setShaderID(GLuint id)
{
  shaderID = id;
}

const  GLuint ShaderAttributes::getShaderID()
{
  return shaderID;
}

void   ShaderAttributes::setShaderType(GLuint typeArg)
{
  switch (typeArg)
  {
    case VERTEX_PROGRAM:
    case FRAGMENT_PROGRAM:
    case VERTEX_PROGRAM_NV:
    case FRAGMENT_PROGRAM_NV:
    case VERTEX_SHADER:
    case FRAGMENT_SHADER:
      shaderType = typeArg;
      break;
    default:
      shaderType = UNDEFINED;
  }
}

const  GLuint ShaderAttributes::getShaderTrueType()
{
  switch (shaderType)
  {
    case VERTEX_PROGRAM:
      return GL_VERTEX_PROGRAM_ARB;
      break;
    case FRAGMENT_PROGRAM:
      return GL_FRAGMENT_PROGRAM_ARB;
      break;
    case VERTEX_SHADER:
      return GL_VERTEX_SHADER_ARB;
      break;
    case FRAGMENT_SHADER:
      return GL_FRAGMENT_SHADER_ARB;
      break;
    case LINKED_SHADER:
      return GL_FRAGMENT_SHADER_ARB;
      break;
  }
  return 0;
}

const GLuint ShaderAttributes::getShaderType()
{
  return shaderType;
}

const bool ShaderAttributes::activate()
{
  GLuint shaderTarget = getShaderTrueType();
  
  if (!shaderID || !shaderTarget)
    return false;
    
  switch (shaderType)
  {
    case VERTEX_PROGRAM:
    case FRAGMENT_PROGRAM:
      glEnable(shaderTarget);
      glBindProgramARB(shaderTarget, shaderID);
      break;
      
    case VERTEX_SHADER:
    case FRAGMENT_SHADER:
    case LINKED_SHADER:
      glUseProgramObjectARB(shaderID);
      break;
    default:
      return false;
  }
  sendAttributes();
  return true;
}

const bool ShaderAttributes::deactivate()
{
  GLuint shaderTarget = getShaderTrueType();
  if (!shaderID || !shaderTarget)
    return false;
    
  switch (shaderType)
  {
    case VERTEX_PROGRAM:
    case FRAGMENT_PROGRAM:
      glDisable(shaderTarget);
      break;
      
    case VERTEX_SHADER:
    case FRAGMENT_SHADER:
    case LINKED_SHADER:
      glUseProgramObjectARB(0);
      break;
    default:
      return false;
  }
  return true;
}

void ShaderAttributes::sendAttributes()
{
  if (shaderType == UNDEFINED)
    return;
    
  GLuint shaderTarget = getShaderTrueType();
  
  if (shaderType == LINKED_SHADER ||
      shaderType == VERTEX_SHADER ||
      shaderType == FRAGMENT_SHADER)
  {
    sendAttributes1f();
    sendAttributes1i();
    sendAttributes2f();
    sendAttributes2i();
    sendAttributes3f();
    sendAttributes3i();
    sendAttributes4f();
    sendAttributes4i();
    sendAttributes16f();
  }
  
  if (shaderType == VERTEX_PROGRAM ||
      shaderType == FRAGMENT_PROGRAM)
  {
    for (size_t i = 0; i < attributes4f.size(); i++)
      if (attributes4f[i].locals())
        glProgramLocalParameter4fvARB(shaderTarget, int(i), attributes4f[i]);
      else
        glProgramEnvParameter4fvARB(shaderTarget,   int(i), attributes4f[i]);
  }
}

void ShaderAttributes::sendAttributes1i()
{
  for (size_t i = 0; i < attributes1i.size(); i++)
    if (getUniformLocation(attributes1i[i].getName()) != -1)
      glUniform1ivARB(location, 1, attributes1i[i].getElements());
      
}

void ShaderAttributes::sendAttributes2i()
{
  for (size_t i = 0; i < attributes2i.size(); i++)
    if (getUniformLocation(attributes2i[i].getName()) != -1)
      glUniform2ivARB(location, 1, attributes2i[i].getElements());
}

void ShaderAttributes::sendAttributes3i()
{
  for (size_t i = 0; i < attributes3i.size(); i++)
    if (getUniformLocation(attributes3i[i].getName()) != -1)
      glUniform3ivARB(location, 1, attributes3i[i].getElements());
}

void ShaderAttributes::sendAttributes4i()
{
  for (size_t i = 0; i < attributes4i.size(); i++)
    if (getUniformLocation(attributes4i[i].getName()) != -1)
      glUniform4ivARB(location, 1, attributes4i[i].getElements());
}

void ShaderAttributes::sendAttributes1f()
{
  for (size_t i = 0; i < attributes1f.size(); i++)
    if (getUniformLocation(attributes1f[i].getName()) != -1)
      glUniform1fARB(location, attributes1f[i].getElements()[0]);
}

void ShaderAttributes::sendAttributes2f()
{
  for (size_t i = 0; i < attributes2f.size(); i++)
    if (getUniformLocation(attributes2f[i].getName()) != -1)
      glUniform2fvARB(location, 1, attributes2f[i].getElements());
}

void ShaderAttributes::sendAttributes3f()
{
  for (size_t i = 0; i < attributes3f.size(); i++)
    if (getUniformLocation(attributes3f[i].getName()) != -1)
      glUniform3fvARB(location, 1, attributes3f[i].getElements());
}

void ShaderAttributes::sendAttributes4f()
{
  for (size_t i = 0; i < attributes4f.size(); i++)
    if (getUniformLocation(attributes4f[i].getName()) != -1)
      glUniform4fvARB(location, 1, attributes4f[i].getElements());
}

void ShaderAttributes::sendAttributes16f()
{
  for (size_t i = 0; i < attributes16f.size(); i++)
    if (getUniformLocation(attributes16f[i].getName()) != -1)
      glUniformMatrix4fv(location, 1, false, attributes16f[i].getElements());
}

bool ShaderAttributes::addAttributes1f(const ShaderElements1f &attr)
{
  if (getAttributes1f(attr.getName()))
    return false;
    
  attributes1f.push_back(attr);
  return true;
}

bool ShaderAttributes::addAttributes2f(const ShaderElements2f &attr)
{
  if (getAttributes2f(attr.getName()))
    return false;
    
  attributes2f.push_back(attr);
  return true;
}

bool ShaderAttributes::addAttributes3f(const ShaderElements3f &attr)
{
  if (getAttributes3f(attr.getName()))
    return false;
    
  attributes3f.push_back(attr);
  return true;
}

bool ShaderAttributes::addAttributes4f(const ShaderElements4f &attr)
{
  if (getAttributes4f(attr.getName()))
    return false;
    
  attributes4f.push_back(attr);
  return true;
}

bool ShaderAttributes::addAttributes16f(const ShaderElements16f &attr)
{
  if (getAttributes16f(attr.getName()))
    return false;
    
  attributes16f.push_back(attr);
  return true;
}

bool ShaderAttributes::addAttributes1i(const ShaderElements1i &attr)
{
  if (getAttributes1i(attr.getName()))
    return false;
    
  attributes1i.push_back(attr);
  return true;
}

bool ShaderAttributes::addAttributes2i(const ShaderElements2i &attr)
{
  if (getAttributes2i(attr.getName()))
    return false;
    
  attributes2i.push_back(attr);
  return true;
}

bool ShaderAttributes::addAttributes3i(const ShaderElements3i &attr)
{
  if (getAttributes3i(attr.getName()))
    return false;
    
  attributes3i.push_back(attr);
  return true;
}

bool ShaderAttributes::addAttributes4i(const ShaderElements4i &attr)
{
  if (getAttributes4i(attr.getName()))
    return false;
    
  attributes4i.push_back(attr);
  return true;
}

ShaderElements1f *ShaderAttributes::getAttributes1f(GLuint i)
{
  return (i >= attributes1f.size()) ? NULL : &attributes1f[i];
}

ShaderElements2f *ShaderAttributes::getAttributes2f(GLuint i)
{
  return (i >= attributes2f.size()) ? NULL : &attributes2f[i];
}
ShaderElements3f *ShaderAttributes::getAttributes3f(GLuint i)
{
  return (i >= attributes3f.size()) ? NULL : &attributes3f[i];
}

ShaderElements4f *ShaderAttributes::getAttributes4f(GLuint i)
{
  return (i >= attributes4f.size()) ? NULL : &attributes4f[i];
}

ShaderElements16f *ShaderAttributes::getAttributes16f(GLuint i)
{
  return (i >= attributes16f.size()) ? NULL : &attributes16f[i];
}

ShaderElements1i *ShaderAttributes::getAttributes1i(GLuint i)
{
  return (i >= attributes1i.size()) ? NULL : &attributes1i[i];
}

ShaderElements2i *ShaderAttributes::getAttributes2i(GLuint i)
{
  return (i >= attributes2i.size()) ? NULL : &attributes2i[i];
}

ShaderElements3i *ShaderAttributes::getAttributes3i(GLuint i)
{
  return (i >= attributes3i.size()) ? NULL : &attributes3i[i];
}

ShaderElements4i *ShaderAttributes::getAttributes4i(GLuint i)
{
  return (i >= attributes4i.size()) ? NULL : &attributes4i[i];
}

ShaderElements1f *ShaderAttributes::getAttributes1f(const char *name)
{
  if (name)
  {
    for (size_t i = 0; i < attributes1f.size(); i++)
      if (attributes1f[i].getName() == name)
        return &attributes1f[i];
  }
  return NULL;
}

ShaderElements2f *ShaderAttributes::getAttributes2f(const char *name)
{
  if (name)
  {
    for (size_t i = 0; i < attributes2f.size(); i++)
      if (attributes2f[i].getName() == name)
        return &attributes2f[i];
  }
  return NULL;
}

ShaderElements3f *ShaderAttributes::getAttributes3f(const char *name)
{
  if (name)
  {
    for (size_t i = 0; i < attributes3f.size(); i++)
      if (attributes3f[i].getName() == name)
        return &attributes3f[i];
  }
  return NULL;
}

ShaderElements4f *ShaderAttributes::getAttributes4f(const char *name)
{
  if (name)
  {
    for (size_t i = 0; i < attributes4f.size(); i++)
      if (attributes4f[i].getName() == name)
        return &attributes4f[i];
  }
  return NULL;
}

ShaderElements16f *ShaderAttributes::getAttributes16f(const char *name)
{
  if (name)
  {
    for (size_t i = 0; i < attributes16f.size(); i++)
      if (attributes16f[i].getName() == name)
        return &attributes16f[i];
  }
  return NULL;
}

ShaderElements1i *ShaderAttributes::getAttributes1i(const char *name)
{
  if (name)
  {
    for (size_t i = 0; i < attributes1i.size(); i++)
      if (attributes1i[i].getName() == name)
        return &attributes1i[i];
  }
  return NULL;
}

ShaderElements2i *ShaderAttributes::getAttributes2i(const char *name)
{
  if (name)
  {
    for (size_t i = 0; i < attributes2i.size(); i++)
      if (attributes2i[i].getName() == name)
        return &attributes2i[i];
  }
  return NULL;
}

ShaderElements3i *ShaderAttributes::getAttributes3i(const char *name)
{
  if (name)
  {
    for (size_t i = 0; i < attributes3i.size(); i++)
      if (attributes3i[i].getName() == name)
        return &attributes3i[i];
  }
  return NULL;
}

ShaderElements4i *ShaderAttributes::getAttributes4i(const char *name)
{
  if (name)
  {
    for (size_t i = 0; i < attributes4i.size(); i++)
      if (attributes4i[i].getName() == name)
        return &attributes4i[i];
  }
  return NULL;
}

void ShaderAttributes::setLocal(bool localArg)
{
  localAttributes = localArg;
}

int ShaderAttributes::getUniformLocation(const char* name)
{
  location = glGetUniformLocationARB(shaderID, name);
  if (location == -1)
    Logger::writeFatalErrorLog(String("Error: couldn't loacte -> ") + name);
    
  return location;
}

void ShaderAttributes::clear()
{
  localAttributes = false;
  shaderType      = UNDEFINED;
  location        = -1;
  shaderID        = 0;
  
  attributes1f.clear();
  attributes1i.clear();
  
  attributes2f.clear();
  attributes2i.clear();
  
  attributes3f.clear();
  attributes3i.clear();
  
  attributes4f.clear();
  attributes4i.clear();
  
  attributes16f.clear();
}
