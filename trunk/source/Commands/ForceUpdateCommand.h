#ifndef FORCEUPDATECOMMAND
#define FORCEUPDATECOMMAND

#include "Command.h"

class ForceUpdateCommand : public Command
{
  public:
    ForceUpdateCommand();
    ~ForceUpdateCommand();
    
    void execute();
    void setWinUpdate(bool update);
    
  private:
    bool winUpdate;
};

#endif
