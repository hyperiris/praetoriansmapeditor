#ifndef TEXTURESMANAGER_H
#define TEXTURESMANAGER_H

#include "../Tools/MediaInfo.h"
#include "../Containers/ocarray.h"

typedef MediaInfo <unsigned int> TextureInfo;

class TexturesManager
{
  public:
    static bool        addTextureInfo(TextureInfo *textureInfo);
    static TextureInfo *getTextureInfo(const char* texturePath);
    static TextureInfo *getTextureInfo(unsigned int textureID);
    static void        flushUnusedTextures();
    
    static void printTexturesInfo();
    static void flushAllTextures();
    
  private:
    static ArrayPtr <TextureInfo> textureCollection;
    
};

#endif