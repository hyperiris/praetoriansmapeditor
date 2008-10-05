#include "MediaPathManager.h"
using namespace std;

vector<String> MediaPathManager::pathStack;
String MediaPathManager::rpath;

const char* MediaPathManager::lookUpMediaPath(const char *path)
{
  if (!path)
    return NULL;
    
  ifstream test;
  string buffer;
  
  test.open(path);
  if (test.is_open())
  {
    test.close();
    return path;
  }
  
  for (size_t i = 0; i < pathStack.size(); i++)
  {
    buffer  = pathStack[i];
    buffer += path;
    
    test.open(buffer.data());
    if (test.is_open())
    {
      test.close();
      rpath = buffer.c_str();
      return rpath;
    }
  }
  
  return NULL;
}

bool MediaPathManager::registerPath(XMLElement *mediaPathNode)
{
  XMLElement *child;
  
  if (mediaPathNode)
    if (mediaPathNode->getName() == "MediaFolder")
      if (child = mediaPathNode->getChildByName("path"))
        return  registerPath(child->getValuec());
        
  return false;
}

bool MediaPathManager::registerPath(const char *path)
{
  if (!path || !strlen(path))
    return Logger::writeErrorLog("Failed to register data path -> NULL");
    
  for (size_t i = 0; i < pathStack.size(); i++)
    if (pathStack[i] == path)
      return true;
      
  String stringBuffer = path;
  
  Logger::writeInfoLog(String("Registering data path -> ") + path);
  pathStack.push_back(stringBuffer);
  return true;
}

const char*  MediaPathManager::getPath(unsigned int index)
{
  if (index < pathStack.size())
    return pathStack[index];
  return NULL;
}

int MediaPathManager::getPathCount()
{
  return int(pathStack.size());
}

void  MediaPathManager::printAllPaths()
{
  cout << "List of registred Media Paths: \n";
  
  for (size_t i = 0; i < pathStack.size(); i++)
  {
    if (pathStack[i])
    {
      cout << int(i) << "-" << pathStack[i] << endl;
    }
  }
  
  cout << endl;
}