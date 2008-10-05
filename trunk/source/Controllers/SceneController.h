#ifndef SCENECONTROLLER
#define SCENECONTROLLER

#include "../Containers/ocavltree.h"

class OCString;
class XMLElement;
class Command;

class SceneController
{
  public:
    SceneController();
    ~SceneController();
    
  public:
    bool loadXMLSettings(XMLElement *element);
    void execute(const char* callbackString);
    void addCommand(Command* command);
    
  private:
    AVLTree <Command*> commands;
};

#endif
