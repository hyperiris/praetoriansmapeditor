#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include "../Window/Window.h"
#include "../Tools/Logger.h"
class Window;

class MouseTracker
{
  public:
    MouseTracker();
    MouseTracker(const MouseTracker &copy);
    MouseTracker &operator=(const MouseTracker &copy);
    
    static int  getButtonIndex(WPARAM eventIndex);
    void        setButtonState(WPARAM eventIndex, bool state);
    int         getActiveButtonIndex();
    
    bool buttonActive();
    void resetButtons();
    
    bool middleMouseOn,
    rightMouseOn,
    leftMouseOn;
};

class WindowInfo
{
  public:
    WindowInfo(Window *ptr = NULL);
    WindowInfo(const WindowInfo &copy);
    WindowInfo &operator=(const WindowInfo &copy);
    MouseTracker  tracker;
    Window       *windowPointer;
};

class WindowsManager
{
  public:
    static bool  validateDisplayMode(DisplayMode &mode);
    static bool  approveWindow(Window *windowPTR);
    static bool  removeWindow(Window *windowPTR);
    static bool  addWindow(Window *windowPTR);
    
    static WindowInfo *getWindowInfo(const HWND  windowHandle);
    static Window     *getWindow(const char* title);
    static Window     *getWindow(size_t index);
    
    static void    destroyAllWindows();
    static LRESULT CALLBACK windowEventsProcessor(HWND   hWnd  , UINT   wMsg,
        WPARAM wParam, LPARAM lParam);
  private:
    static void enumerateDisplayModes();
    
    static vector<WindowInfo>  windowsList;
    static vector<DisplayMode> displayModes;
    static bool                enumerated;
};

#endif