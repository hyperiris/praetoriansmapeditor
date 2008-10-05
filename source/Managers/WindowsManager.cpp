#include "ManagersUtils.h"

vector<DisplayMode> WindowsManager::displayModes;
vector<WindowInfo>  WindowsManager::windowsList;
bool                WindowsManager::enumerated = false;

bool WindowsManager::validateDisplayMode(DisplayMode &mode)
{
  enumerateDisplayModes();
  
  size_t index       = 0;
  int    refreshRate = 0;
  for (index = 0; index < displayModes.size(); index++)
    if (mode.getColorDepth() == displayModes[index].getColorDepth() &&
        mode.getHeight()     == displayModes[index].getHeight()     &&
        mode.getWidth()      == displayModes[index].getWidth())
    {
      refreshRate = refreshRate > displayModes[index].getRefreshRate() ?
                    refreshRate : displayModes[index].getRefreshRate();
    }
    
  if (refreshRate)
  {
    Logger::writeInfoLog("DisplayMode approved for fullscreen window");
    mode.setRefreshRate(refreshRate);
    return true;
  }
  
  return Logger::writeErrorLog("Unable to find a fullscreen display mode matching the request");
}

void WindowsManager::enumerateDisplayModes()
{
  if (enumerated)
    return;
    
  int j = 0;
  
  DEVMODE deviceMode;
  
  ZeroMemory(&deviceMode, sizeof(DEVMODE));
  deviceMode.dmSize = sizeof(DEVMODE);
  
  while (EnumDisplaySettings(NULL, j++, &deviceMode))
    if (deviceMode.dmBitsPerPel > 8 && ChangeDisplaySettings(&deviceMode, CDS_FULLSCREEN | CDS_TEST) == DISP_CHANGE_SUCCESSFUL)
    {
      DisplayMode mode(0, 0, deviceMode.dmPelsWidth, deviceMode.dmPelsHeight, deviceMode.dmBitsPerPel);
      mode.setRefreshRate(deviceMode.dmDisplayFrequency);
      displayModes.push_back(mode);
    }
  enumerated = true;
}

bool WindowsManager::approveWindow(Window *windowPTR)
{
  return true;
}

bool WindowsManager::addWindow(Window *windowPTR)
{
  if (!windowPTR) return false;
  for (size_t i = 0; i < windowsList.size(); i++)
    if (windowsList[i].windowPointer == windowPTR)
      return false;
      
  windowsList.push_back(WindowInfo(windowPTR));
  return true;
}

bool WindowsManager::removeWindow(Window *windowPTR)
{
  if (!windowPTR)
    return false;
    
  WindowInfo *info = getWindowInfo(windowPTR->getHandle());
  vector<WindowInfo> windowsNewList;
  
  if (!info)
    return false;
    
  for (size_t i = 0; i < windowsList.size(); i++)
    if (windowsList[i].windowPointer != info->windowPointer)
      windowsNewList.push_back(windowsList[i]);
    else
      Logger::writeImmidiateInfoLog(String("Removing window from manager -> ") + info->windowPointer->getTitle());
      
  windowsList.clear();
  
  for (size_t i = 0; i < windowsNewList.size(); i++)
    windowsList.push_back(windowsNewList[i]);
    
  return true;
}

Window *WindowsManager::getWindow(const char* title)
{
  for (size_t i = 0; i < windowsList.size(); i++)
    if (windowsList[i].windowPointer->getTitle() == title)
      return windowsList[i].windowPointer;
  return NULL;
}

Window *WindowsManager::getWindow(size_t index)
{
  return (index >= windowsList.size()) ? NULL : windowsList[index].windowPointer;
}

WindowInfo *WindowsManager::getWindowInfo(const HWND  windowHandle)
{
  for (size_t i = 0; i < windowsList.size(); i++)
    if (windowsList[i].windowPointer->getHandle() == windowHandle)
      return &windowsList[i];
  return NULL;
}

void  WindowsManager::destroyAllWindows()
{
  for (size_t i = 0; i < windowsList.size(); i++)
    windowsList[i].windowPointer->destroy();
    
  windowsList.clear();
}

LRESULT CALLBACK WindowsManager::windowEventsProcessor(HWND   hWnd  , UINT   wMsg,
    WPARAM wParam, LPARAM lParam)
{
  InputEventListener *listener     = NULL;
  MouseTracker       *mouseTracker = NULL;
  WindowInfo         *info         = WindowsManager::getWindowInfo(hWnd);
  Window             *winPTR       = NULL;
  
  if (info && (winPTR = info->windowPointer))
  {
    mouseTracker  = &info->tracker;
    listener      = winPTR->getInputEventListener();
    
    int highBits = HIWORD(lParam),
                   lowBits  = LOWORD(lParam),
                              inverse  = winPTR->getHeight() -  highBits,
                                         mouseX   = lowBits   > 2048 ? 0 : lowBits,
                                                    mouseY   = highBits  > 2048 ? 0 : highBits;
                                                    
    if (listener)
    {
      switch (wMsg)
      {
        case 522:
          listener->mouseScrolled(MouseEvent(BUTTON1, mouseX, mouseY, inverse,
                                             (HIWORD(wParam) > 60000) ? SCROLLING_DOWN : SCROLLING_UP));
          break;
          
        case WM_LBUTTONDBLCLK:
        case WM_RBUTTONDBLCLK:
        case WM_MBUTTONDBLCLK:
          listener->mouseDoubleClicked(MouseEvent(MouseTracker::getButtonIndex(wMsg),
                                                  lowBits, highBits, inverse));
          break;
          
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
          ReleaseCapture();
          mouseTracker->setButtonState(wMsg, false);
          listener->mouseReleased(MouseEvent(MouseTracker::getButtonIndex(wMsg),
                                             mouseX, mouseY, inverse));
          break;
          
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
          SetCapture(winPTR->getHandle());
          mouseTracker->setButtonState(wMsg, true);
          listener->mouseClicked(MouseEvent(MouseTracker::getButtonIndex(wMsg),
                                            mouseX, mouseY, inverse));
          break;
          
        case WM_MOUSEMOVE:
          if (mouseTracker->buttonActive())
            listener->mouseDragged(MouseEvent(mouseTracker->getActiveButtonIndex(),
                                              mouseX, mouseY, inverse));
          else
            listener->mouseMoved(MouseEvent(NOBUTTON, mouseX, mouseY, inverse));
          break;
          
        case WM_NCMOUSELEAVE:
          mouseTracker->resetButtons();
          listener->mouseExit(MouseEvent(NOBUTTON, mouseX, mouseY, inverse));
          break;
          
        case WM_KEYDOWN:
        {
          listener->keyPressed(KeyEvent(int(wParam)));
          break;
        }
        
        case WM_KEYUP:
        {
          listener->keyReleased(KeyEvent(int(wParam)));
          break;
        }
      }
    }
    
    switch (wMsg)
    {
      case WM_ACTIVATE:
      {
        if (listener)
        {
          if (!highBits)
            listener->windowMaximized(WindowEvent(winPTR->getX(),   winPTR->getY(),
                                                  winPTR->getWidth(), winPTR->getHeight()));
          else
            listener->windowMinimized(WindowEvent(winPTR->getX(),     winPTR->getY(),
                                                  winPTR->getWidth(), winPTR->getHeight()));
        }
        return 0;
      }
      
      case WM_SYSCOMMAND:
      {
        switch (wParam)
        {
          case SC_SCREENSAVE:
          case SC_MONITORPOWER:
            return 0;
        }
        break;
      }
      
      case WM_CLOSE:
      {
        PostQuitMessage(0);
        if (listener)
          listener->windowClosing(WindowEvent(winPTR->getX(),     winPTR->getY(),
                                              winPTR->getWidth(), winPTR->getHeight()));
        return 0;
      }
      
      case WM_CREATE:
      case WM_SIZE:
      {
        winPTR->updatePosition();
        winPTR->updateDimensions(lParam);
        if (listener)
          listener->windowSizeChanged(WindowEvent(winPTR->getX(),winPTR->getY(), lowBits, highBits));
        return 0;
      }
      
      case WM_WINDOWPOSCHANGING:
      case WM_WINDOWPOSCHANGED:
      {
        winPTR->updatePosition();
        if (listener)
          listener->windowPositionChanged(WindowEvent(winPTR->getX(),     winPTR->getY(),
                                          winPTR->getWidth(), winPTR->getHeight()));
        break;
      }
    }
  }
  return DefWindowProc(hWnd,wMsg,wParam,lParam);
}

MouseTracker::MouseTracker()
{
  resetButtons();
}

MouseTracker::MouseTracker(const MouseTracker &copy)
{
  this->operator =(copy);
}

MouseTracker &MouseTracker::operator=(const MouseTracker &copy)
{
  if (this != &copy)
  {
    middleMouseOn = copy.middleMouseOn;
    rightMouseOn  = copy.rightMouseOn;
    leftMouseOn   = copy.leftMouseOn;
  }
  return *this;
}

void MouseTracker::setButtonState(WPARAM eventIndex, bool state)
{
  switch (eventIndex)
  {
    case WM_LBUTTONUP:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONDBLCLK:
      leftMouseOn = state;
      break;
      
    case WM_RBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONDBLCLK:
      rightMouseOn = state;
      break;
      
    case WM_MBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONDBLCLK:
      middleMouseOn = state;
      break;
  }
}

int MouseTracker::getButtonIndex(WPARAM eventIndex)
{
  int buttonIndex = NOBUTTON;
  
  switch (eventIndex)
  {
    case WM_LBUTTONUP:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONDBLCLK:
      buttonIndex = BUTTON1;
      break;
      
    case WM_RBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONDBLCLK:
      buttonIndex = BUTTON2;
      break;
      
    case WM_MBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONDBLCLK:
      buttonIndex = BUTTON3;
      break;
  }
  return buttonIndex;
}

int MouseTracker::getActiveButtonIndex()
{
  return leftMouseOn   ? BUTTON1 :
         rightMouseOn  ? BUTTON2 :
         middleMouseOn ? BUTTON3 : NOBUTTON;
}

bool MouseTracker::buttonActive()
{
  return leftMouseOn  ||
         rightMouseOn ||
         middleMouseOn;
}

void MouseTracker::resetButtons()
{
  middleMouseOn = false;
  rightMouseOn  = false;
  leftMouseOn   = false;
}

WindowInfo::WindowInfo(Window *ptr)
{
  windowPointer = ptr;
}

WindowInfo::WindowInfo(const WindowInfo &copy)
{
  this->operator =(copy);
}

WindowInfo &WindowInfo::operator=(const WindowInfo &copy)
{
  if (this != &copy)
  {
    windowPointer = copy.windowPointer;
    tracker       = copy.tracker;
  }
  return *this;
}