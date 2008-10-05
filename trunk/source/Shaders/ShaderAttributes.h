#ifndef SHADER_ATTRIBUTES_H
#define SHADER_ATTRIBUTES_H

#include "ShaderElements.h"
#include "../Tools/Logger.h"

enum
{
  UNDEFINED,
  VERTEX_PROGRAM,
  FRAGMENT_PROGRAM,
  VERTEX_PROGRAM_NV,
  FRAGMENT_PROGRAM_NV,
  VERTEX_SHADER,
  FRAGMENT_SHADER,
  LINKED_SHADER,
};

typedef vector<ShaderElements1f> Attributes1f;
typedef vector<ShaderElements1i> Attributes1i;

typedef vector<ShaderElements2f> Attributes2f;
typedef vector<ShaderElements2i> Attributes2i;

typedef vector<ShaderElements3f> Attributes3f;
typedef vector<ShaderElements3i> Attributes3i;

typedef vector<ShaderElements4f> Attributes4f;
typedef vector<ShaderElements4i> Attributes4i;

typedef vector<ShaderElements16f> Attributes16f;

class ShaderAttributes : public NamedObject
{
  public:
    ShaderAttributes(const char *name = NULL);
    ShaderAttributes(const ShaderAttributes &copy);
    ShaderAttributes &operator = (const ShaderAttributes &copy);
    void              operator+= (const ShaderAttributes &copy);
    
    static void printObjectInfoLog(const char* file, int ID);
    
    bool link(ShaderAttributes &secondPart);
    
    bool addAttributes1f(const ShaderElements1f &attr);
    bool addAttributes2f(const ShaderElements2f &attr);
    bool addAttributes3f(const ShaderElements3f &attr);
    bool addAttributes4f(const ShaderElements4f &attr);
    bool addAttributes16f(const ShaderElements16f &attr);
    
    bool addAttributes1i(const ShaderElements1i  &attr);
    bool addAttributes2i(const ShaderElements2i  &attr);
    bool addAttributes3i(const ShaderElements3i  &attr);
    bool addAttributes4i(const ShaderElements4i  &attr);
    
    ShaderElements1f  *getAttributes1f(GLuint i);
    ShaderElements2f  *getAttributes2f(GLuint i);
    ShaderElements3f  *getAttributes3f(GLuint i);
    ShaderElements4f  *getAttributes4f(GLuint i);
    ShaderElements16f *getAttributes16f(GLuint i);
    
    ShaderElements1i  *getAttributes1i(GLuint i);
    ShaderElements2i  *getAttributes2i(GLuint i);
    ShaderElements3i  *getAttributes3i(GLuint i);
    ShaderElements4i  *getAttributes4i(GLuint i);
    
    ShaderElements1f  *getAttributes1f(const char *name);
    ShaderElements2f  *getAttributes2f(const char *name);
    ShaderElements3f  *getAttributes3f(const char *name);
    ShaderElements4f  *getAttributes4f(const char *name);
    ShaderElements16f *getAttributes16f(const char *name);
    
    ShaderElements1i  *getAttributes1i(const char *name);
    ShaderElements2i  *getAttributes2i(const char *name);
    ShaderElements3i  *getAttributes3i(const char *name);
    ShaderElements4i  *getAttributes4i(const char *name);
    
    void   sendAttributes();
    
    void   setShaderID(GLuint id);
    const  GLuint getShaderID();
    
    const  bool deactivate();
    const  bool activate();
    
    const  GLuint getShaderTrueType();
    void   setShaderType(GLuint type);
    const  GLuint getShaderType();
    
    void   setLocal(bool local);
    void   clear();
    
  private:
    int   getUniformLocation(const char* name);
    int   checkGLError(int location, const char* name);
    
    void sendAttributes1f();
    void sendAttributes2f();
    void sendAttributes3f();
    void sendAttributes4f();
    void sendAttributes16f();
    
    void sendAttributes1i();
    void sendAttributes2i();
    void sendAttributes3i();
    void sendAttributes4i();
    
    Attributes1f  attributes1f;
    Attributes1i  attributes1i;
    
    Attributes2f  attributes2f;
    Attributes2i  attributes2i;
    
    Attributes3f  attributes3f;
    Attributes3i  attributes3i;
    
    Attributes4f  attributes4f;
    Attributes4i  attributes4i;
    
    Attributes16f attributes16f;
    
    bool          localAttributes;
    GLuint        shaderType,
    shaderID,
    location;
};
#endif