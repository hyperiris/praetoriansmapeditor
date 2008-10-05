#ifndef TEXTURECOORDGENERATOR_H
#define TEXTURECOORDGENERATOR_H

#include "../Math/Tuple4.h"
#include "../Tools/Logger.h"

class TextureCoordGenerator : public IOXMLObject
{
  private:
    void     setMode(const char* mode);
    bool     planarMode();
    
    Tuple4f  planes[4];
    bool     enabled;
    int      format,
    mode;
  public:
    TextureCoordGenerator();
    TextureCoordGenerator(GLuint, GLuint);
    TextureCoordGenerator(const TextureCoordGenerator&);
    TextureCoordGenerator &operator= (const TextureCoordGenerator&);
    
    virtual bool loadXMLSettings(XMLElement *element);
    virtual bool exportXMLSettings(ofstream &xmlFile);
    
    void enableGeneration(bool);
    bool generationOn();
    
    void setPlane(int, const Tuple4f &);
    void setFormat(int);
    void setMode(int);
    
    void startGeneration();
    void stopGeneration();
    
    int  getGenerationFormat();
    int  getGenerationMode();
    
    const  Tuple4f &getPlane(int index) const;
};

#endif