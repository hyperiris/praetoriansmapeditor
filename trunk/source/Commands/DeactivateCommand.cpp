#include "DeactivateCommand.h"
#include "../Managers/SceneManager.h"

void DeactivateCommand::execute()
{
  SceneManager::deactivateCurrentStage();
}
