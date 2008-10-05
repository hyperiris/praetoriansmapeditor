#include "ForceUpdateCommand.h"
#include "../Managers/SceneManager.h"

ForceUpdateCommand::ForceUpdateCommand()
{
  winUpdate = false;
}

void ForceUpdateCommand::execute()
{
  SceneManager::updateStage(winUpdate);
}

void ForceUpdateCommand::setWinUpdate(bool update)
{
  winUpdate = update;
}

ForceUpdateCommand::~ForceUpdateCommand()
{
}
