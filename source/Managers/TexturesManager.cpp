#include "TexturesManager.h"
#include "../Tools/Logger.h"
#include <windows.h>
#include <gl/glu.h>
#include <gl/glut.h>

ArrayPtr <TextureInfo> TexturesManager::textureCollection;

bool TexturesManager::addTextureInfo(TextureInfo *textureInfo)
{
  flushUnusedTextures();
  
  if (!textureInfo)
    return false;
    
  textureCollection.append(textureInfo);
  
  return true;
}

TextureInfo *TexturesManager::getTextureInfo(const char* texturePath)
{
  flushUnusedTextures();
  
  if (!texturePath || textureCollection.isEmpty())
    return 0;
    
  for (size_t i = 0; i < textureCollection.length(); i++)
    if (textureCollection(i)->getMediaPath() == texturePath)
      return textureCollection(i);
      
  return 0;
}

TextureInfo *TexturesManager::getTextureInfo(unsigned int textureID)
{
  if (!textureID || textureCollection.isEmpty())
    return 0;
    
  for (size_t i = 0; i < textureCollection.length(); i++)
    if (textureCollection(i)->getMedia() == textureID)
      return textureCollection(i);
      
  return 0;
}

void TexturesManager::flushUnusedTextures()
{
  ArrayPtr <TextureInfo> validTextures;
  size_t i;
  unsigned int textureID;
  
  for (i = 0; i < textureCollection.length(); i++)
    if (textureCollection(i)->getUserCount() > 0)
      validTextures.append(textureCollection(i));
    else
    {
      textureID = textureCollection(i)->getMedia();
      glDeleteTextures(1, &textureID);
      deleteObject(textureCollection(i));
    }
    
  textureCollection.clear();
  textureCollection = validTextures;
}

void TexturesManager::flushAllTextures()
{
  unsigned int textureID = 0;
  size_t i;
  
  for (i = 0; i < textureCollection.length(); i++)
  {
    textureID = textureCollection(i)->getMedia();
    glDeleteTextures(1, &textureID);
    deleteObject(textureCollection(i));
  }
  
  textureCollection.clear();
  if (i) Logger::writeInfoLog(String("Flushed ") + int(i) + String(" active textures"));
}

void TexturesManager::printTexturesInfo()
{
  if (textureCollection.isEmpty())
    cout << "This Manager contains no Textures as of yet." << endl;
  else
  {
    cout << "Textures list: " << endl;
    for (size_t i = 0; i < textureCollection.length(); i++)
      cout <<  "<users = \"" << textureCollection(i)->getUserCount() << "\" "
           <<  " path  = \"" << textureCollection(i)->getMediaPath() << "\" "
           <<  " id    = \"" << textureCollection(i)->getMedia()     << "\" />" << endl;
  }
}
