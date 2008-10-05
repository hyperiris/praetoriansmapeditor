#include "SwitchCommand.h"
#include "../Managers/SceneManager.h"

void SwitchCommand::execute()
{
  SceneManager::switchScene(target);
}
