#include "ChangeCommand.h"
#include "../Managers/SceneManager.h"

void ChangeCommand::execute()
{
  SceneManager::changeScene(target);
}
