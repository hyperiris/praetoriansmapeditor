#ifndef GL_CAPS_H
#define GL_CAPS_H
#include "../Tools/Logger.h"

class GLCaps : public IOXMLObject
{
  public:
    GLCaps(int  accumulationArg =  0,
           int  samples         =  0,
           int  stencil         =  0,
           int  alpha           =  0,
           int  color           = 32,
           int  depth           = 24,
           bool doubleBuffer    = true,
           bool vsyncEnabled    = false);
           
           
    GLCaps(const GLCaps &copy);
    GLCaps &operator=(const GLCaps &copy);
    
    bool operator==(const GLCaps &compare);
    bool operator!=(const GLCaps &compare);
    
    virtual bool loadXMLSettings(XMLElement *element);
    virtual bool exportXMLSettings(ofstream &xmlFile);
    
    void  set(int  accumulationArg,
              int  samples,
              int  stencil,
              int  alpha,
              int  color,
              int  depth ,
              bool doubleBuffer,
              bool vsyncEnabled);
              
    void  setAccumulation(int accumulationArg);
    const int  getAccumulation() const;
    
    void setMultiSampleLevel(int samplesArg);
    const int  getMultiSampleLevel() const;
    
    void setStencilBits(int stencilBitsArg);
    const int  getStencilBits() const;
    
    void setColorBits(int colorBitsArg);
    const int  getColorBits() const;
    
    void setAlphaBits(int alphaArg);
    const int  getAlphaBits() const;
    
    void setDepthBits(int depthArg);
    const int  getDepthBits() const;
    
    void setVSyncFlag(bool on);
    const bool getVSyncFlag() const;
    
    void setDoubleBufferFlag(bool on);
    const bool getDoubleBufferFlag() const;
    
    const String toString() const;
    
    static const GLCaps    &getMaxCaps();
    static void  adjustCaps(GLCaps &caps);
    
  private:
    static GLCaps maxCaps;
    static bool   initialize();
    
    bool   doubleBuffer,
    vsync;
    
    int    accumulation,
    samples,
    stencil,
    alpha,
    color,
    depth;
};
#endif