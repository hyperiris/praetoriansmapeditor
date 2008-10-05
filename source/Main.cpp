#include "Stage.h"

#pragma comment(linker, "/ENTRY:WinMainCRTStartup")
#ifdef _DEBUG
#pragma comment(linker, "/subsystem:console" )
#endif

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
  MSG msg;
  Stage stage(hInstance);
  
  if (!stage.initialize())
    return -1;
    
  while (stage.isActive())
  {
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
      if (msg.message == WM_QUIT)
        stage.setActive(false);
      else
      {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }
    }
    else
      stage.run();
  }
  
  return int(msg.wParam);
}
