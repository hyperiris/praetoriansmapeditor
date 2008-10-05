#ifndef IMAGE_H
#define IMAGE_H

#include "../Tools/Logger.h"

class Image
{
  private:
    String    path;
    GLubyte  *dataBuffer;
    GLuint    internalFormat,
    components,
    format;
    
    GLushort  height,
    width,
    depth;
    
    bool loadDDS(const char *filename);
    
    bool loadTGA(const char* filename);
    bool loadUncompressed8BitTGA(const char * filename);
    bool loadCompressedTrueColorTGA(const char * filename);
    bool loadUncompressedTrueColorTGA(const char * filename);
    
    bool loadJPG(const char* filename);
    int  decodeJPG();
    void getJPGInfo();
    int  decodeScanJPG();
    
  protected:
    static bool isJPEG(const char* filename)
    {
      return findString(filename, "jpg") ||
             findString(filename, "JPG") ||
             findString(filename, "jpeg") ||
             findString(filename, "JPEG");
    }
    
    static bool isTGA(const char* filename)
    {
      return findString(filename, "tga")||
             findString(filename, "TGA")||
             findString(filename, "targa")||
             findString(filename, "TARGA");
    }
    
    static bool isPNG(const char* filename)
    {
      return findString(filename, "png")||
             findString(filename, "PNG");
    }
    
    static bool isDDS(const char* filename)
    {
      return findString(filename, "dds")||
             findString(filename, "DDS");
    }
    
    static bool findString(const char *source, const char *target)
    {
      if (source)
      {
        const char *str = source;
        int len = int(strlen(target));
        while ((str = strstr(str, target)))
        {
          str += len;
          if (*str == ' ' || *str == '\0')
            return true;
        }
      }
      return false;
    }
    
    virtual bool loadSupportedFormat(const char* path_);
    
  public:
    Image(const char* path = NULL);
    Image(const Image &image);
    Image &operator=(const Image &image);
    virtual ~Image();
    
    static bool isDDSVolume(const char* filename);
    static bool isDDSCube(const char* filename);
    
    void setHeight(unsigned short);
    void setWidth(unsigned short);
    void setDepth(unsigned short);
    
    void setFormat(GLuint);
    void setDataBuffer(const GLubyte*);
    void flipVertically();
    void setInternalFormat(GLuint);
    void setComponentsCount(GLuint);
    void setPath(const char *p);
    
    bool load(const char*);
    
    const String  &getPath()            const;
    const GLuint   getComponentsCount() const;
    const GLuint   getInternalFormat()  const;
    const GLubyte* getDataBuffer()      const;
    const GLuint   getFormat()          const;
    
    const GLushort getHeight() const;
    const GLushort getWidth()  const;
    const GLushort getDepth()  const;
};

#endif
