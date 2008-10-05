#include "SwapCommand.h"
#include "../Managers/SceneManager.h"

void SwapCommand::execute()
{
  if (target.getLength())
    SceneManager::setTransposeScene(target);
    
  SceneManager::transposeScene();
}
