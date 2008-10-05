#include "SceneController.h"
#include "../Parsers/XMLParser.h"
#include "../Commands/SwapCommand.h"
#include "../Commands/SwitchCommand.h"
#include "../Commands/ChangeCommand.h"
#include "../Commands/DeactivateCommand.h"
#include "../Commands/ForceUpdateCommand.h"
#include "../Containers/ocstring.h"
#include "../Tools/Logger.h"

SceneController::SceneController()
{}

bool SceneController::loadXMLSettings(XMLElement *element)
{
  XMLElement *child, *subChild;
  String type;
  Command *command;
  
  for (size_t i = 0; i < element->getChildrenCount(); i++)
  {
    child = element->getChild(i);
    
    if (child->getName() == "Command")
      if (subChild = child->getChildByName("type"))
      {
        type = subChild->getValuec();
        
        if (type == "switch")
        {
          command = new SwitchCommand;
          command->loadXMLSettings(child);
          addCommand(command);
          continue;
        }
        
        if (type == "change")
        {
          command = new ChangeCommand;
          command->loadXMLSettings(child);
          addCommand(command);
          continue;
        }
        
        if (type == "deactivate")
        {
          command = new DeactivateCommand;
          command->loadXMLSettings(child);
          addCommand(command);
          continue;
        }
        
        if (type == "update")
        {
          command = new ForceUpdateCommand;
          command->loadXMLSettings(child);
          addCommand(command);
          continue;
        }
        
        if (type == "swap")
        {
          command = new SwapCommand;
          command->loadXMLSettings(child);
          addCommand(command);
          continue;
        }
      }
  }
  
  return true;
}

void SceneController::addCommand(Command* command)
{
  commands.insertKeyAndValue(command->getSource(), command);
}

void SceneController::execute(const char* callbackString)
{
  Command* c;
  if (commands.findValue(callbackString, c))
    c->execute();
}

SceneController::~SceneController()
{
  AVLTreeIterator <Command*> iter(commands);
  while (iter.next())
    delete iter.value();
}
