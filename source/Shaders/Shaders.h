#ifndef SHADERS_H
#define SHADERS_H

#include "../Managers/MediaPathManager.h"
#include "ShaderAttributes.h"

class Shaders
{
  public:
    Shaders();
    Shaders(const Shaders &copy);
    Shaders &operator=(const Shaders &copy);
    ~Shaders();
    
    ShaderAttributes &getFragmentProcessor();
    ShaderAttributes &getVertexProcessor();
    ShaderAttributes *getLinkedProcessor();
    
    bool  loadXMLFile(const char * xmlPath,
                      const char * vertexNode   = NULL,
                      const char * fragmentNode = NULL,
                      bool  link = false);
                      
    bool   loadXMLSettings(XMLElement *node,
                           const char *vertexNode    = NULL,
                           const char *fragmentNode  = NULL,
                           bool        link = false);
                           
    static bool loadShaderFromMemory(ShaderAttributes *shader,
                                     const char       *shaderCode,
                                     int               shaderType);
                                     
    bool updateElementsi(const char* name, int size, int   *elements);
    bool updateElementsf(const char* name, int size, float *elements);
    
    const bool  isFunctional();
    const bool  deactivate();
    const bool  activate();
    
    void  destroy();
    
  private:
    static XMLElement *retrieveProcessorNode(XMLElement *node,
        const char *description,
        bool        vertexProcessor);
        
    static int         estimateProcessorType(XMLElement *node,
        bool        vertexProcessor);
        
    bool    loadProgramCode(XMLElement *node, int type);
    bool    loadShadersCode(XMLElement *node, int type);
    
    ShaderElements4i  *attrib4i;
    ShaderElements3i  *attrib3i;
    ShaderElements2i  *attrib2i;
    ShaderElements1i  *attrib1i;
    
    ShaderElements16f *attrib16f;
    ShaderElements4f  *attrib4f;
    ShaderElements3f  *attrib3f;
    ShaderElements2f  *attrib2f;
    ShaderElements1f  *attrib1f;
    String             xmlShaderPath;
  protected:
    ShaderAttributes *linkedProcessor,
    vertexProcessor,
    fragmentProcessor;
    
};
#endif