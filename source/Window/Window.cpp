#include "Window.h"
#include "../Managers/ManagersUtils.h"
#include "../Renderer/Renderer.h"
#include "../Tools/Timer.h"
#include "../resources/resource.h"

Window::Window(HINSTANCE currAppInstance) : IOXMLObject("Window")
{
  inputEventListener  = NULL;
  
  snapshotScheduled   = false;
  visible             = false;
  
  renderingContext    = NULL;
  deviceContext       = NULL;
  windowHandle        = NULL;
  hInstance           = currAppInstance;
  
  position.length     = sizeof(WINDOWPLACEMENT);
  fullscreen          = true;
}

bool Window::create(const  char *titleArg,
                    DisplayMode &displayMode,
                    GLCaps      &caps)
{
  Timer::intialize();
  
  WNDCLASSEX windowsClass;
  GLCaps    userCaps = caps;
  DWORD     dwExStyle;
  DWORD     dwStyle;
  int       pixelFormat, height, width, x, y;
  
  GLCaps::adjustCaps(caps);
  
  Logger::writeInfoLog("Creating GL Window");
  Logger::writeInfoLog(displayMode.toString());
  
  fullscreen = displayMode.isFullscreen();
  dwExStyle  = fullscreen ? WS_EX_APPWINDOW : WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
  dwStyle    = fullscreen ? WS_POPUP        : WS_OVERLAPPEDWINDOW;
  height     = displayMode.getHeight();
  width      = displayMode.getWidth();
  title      = titleArg;
  x          = displayMode.getXPosition();
  y          = displayMode.getYPosition();
  
  if (displayMode.isCentered())
  {
    HDC screenContext = GetDC(NULL);
    x = (GetDeviceCaps(screenContext, HORZRES) - width) / 2;
    y = (GetDeviceCaps(screenContext, VERTRES) - height) / 2;
    x = x < 0 ? 0 : x;
    y = y < 0 ? 0 : y;
    ReleaseDC(NULL, screenContext);
  }
  
  appHandle                     = GetModuleHandle(NULL);
  
  windowsClass.cbSize           = sizeof(WNDCLASSEX);
  windowsClass.style            = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
  windowsClass.lpfnWndProc      = (WNDPROC)WindowsManager::windowEventsProcessor;
  windowsClass.cbClsExtra       = 0;                                    // No Extra Window Data
  windowsClass.cbWndExtra       = 0;                                    // No Extra Window Data
  windowsClass.hInstance        = appHandle;                            // Set The Instance
  windowsClass.hIcon            = LoadIcon(appHandle, MAKEINTRESOURCE(IDI_BIG));
  windowsClass.hIconSm          = LoadIcon(appHandle, MAKEINTRESOURCE(IDI_SMALL));
  windowsClass.hCursor          = LoadCursor(NULL, IDC_ARROW);          // Load The Arrow Pointer
  windowsClass.hbrBackground    = NULL;                                 // No Background Required For GL
  windowsClass.lpszMenuName     = NULL;                                 // We Don't Want A Menu
  windowsClass.lpszClassName    = title;                                // Set The Class Name
  
  if (!RegisterClassEx(&windowsClass)) // Attempt To Register The Window Class
    Logger::writeErrorLog("Failed To Register The Window Class");
    
  if (fullscreen)
  {
    if (WindowsManager::validateDisplayMode(displayMode))
    {
      DEVMODE dmScreenSettings;
      ZeroMemory(&dmScreenSettings, sizeof(dmScreenSettings));
      dmScreenSettings.dmDisplayFrequency = displayMode.getRefreshRate();
      dmScreenSettings.dmBitsPerPel       = displayMode.getColorDepth();
      dmScreenSettings.dmPelsHeight       = displayMode.getHeight();
      dmScreenSettings.dmPelsWidth        = displayMode.getWidth();
      dmScreenSettings.dmFields           = DM_BITSPERPEL | DM_PELSWIDTH |
                                            DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;
      dmScreenSettings.dmSize             = sizeof(dmScreenSettings);
      
      if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
        return Logger::writeErrorLog("Can't create a Fullscreen window");
    }
    else
      return Logger::writeErrorLog("Can't create a Fullscreen window");
  }
  
  if (!(windowHandle = CreateWindowEx(dwExStyle,                        // Extended Style For The Window
                                      title,                              // Class Name
                                      title,                              // Window Title
                                      dwStyle |                           // Defined Window Style
                                      WS_CLIPSIBLINGS | WS_CLIPCHILDREN,  // Required Window Style
                                      x, y,                               // Window Position
                                      width,
                                      height,
                                      NULL,                               // No Parent Window
                                      NULL,                               // No Menu
                                      appHandle,                          // Instance
                                      NULL)))                             // Dont Pass Anything To WM_CREATE
  {
    destroy();
    return Logger::writeErrorLog("Can't Create A GL Device Context");
  }
  
  if (!(deviceContext =  GetDC(windowHandle)))
  {
    destroy();
    return Logger::writeErrorLog("Can't Create A GL Device Context");
  }
  
  PIXELFORMATDESCRIPTOR pfd =
  {
    sizeof(PIXELFORMATDESCRIPTOR),
    1,
    PFD_DRAW_TO_WINDOW |
    PFD_SUPPORT_OPENGL |
    PFD_DOUBLEBUFFER,
    PFD_TYPE_RGBA,
    caps.getColorBits(),
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    caps.getDepthBits(),
    0, 0,
    PFD_MAIN_PLANE,
    0,
    0, 0, 0
  };
  
  if (!(pixelFormat = ChoosePixelFormat(deviceContext, &pfd)))
  {
    destroy();
    return Logger::writeErrorLog("Can't Find A Suitable PixelFormat");
  }
  
  if (!SetPixelFormat(deviceContext, pixelFormat, 0))
  {
    destroy();
    return  Logger::writeErrorLog("Can't Set The PixelFormat");
  }
  
  if (!(renderingContext = wglCreateContext(deviceContext)))
  {
    destroy();
    return Logger::writeErrorLog("Can't Create A GL Rendering Context");
  }
  
  if (!wglMakeCurrent(deviceContext, renderingContext))       // Try To Activate The Rendering Context
  {
    destroy();
    return Logger::writeErrorLog("Can't Activate The GL Rendering Context");
  }
  
  SetForegroundWindow(windowHandle);          // Slightly Higher Priority
  SetFocus(windowHandle);                     // Sets Keyboard Focus To The Window
  UpdateWindow(windowHandle);
  
  WindowsManager::addWindow(this);
  
  if (GLEE_WGL_EXT_swap_control)
    wglSwapIntervalEXT(caps.getVSyncFlag());
    
  renderingCaps  = caps;
  rectangle.set(x, y, width, height);
  return true;
}

const GLCaps  &Window::getRenderingCaps() const
{
  return renderingCaps;
}

bool Window::loadXMLSettings(XMLElement *windowInfoNode)
{
  if (!isSuitable(windowInfoNode))
    return false;
    
  DisplayMode  displayMode;
  XMLElement  *child;
  String       title;
  GLCaps       caps;
  
  bool gotDisplayMode = false, gotGLCaps = false;
  
  if (child = windowInfoNode->getChildByName("DisplayMode"))
    gotDisplayMode = displayMode.loadXMLSettings(child);
    
  if (child = windowInfoNode->getChildByName("GLCaps"))
    gotGLCaps  = caps.loadXMLSettings(child);
    
  if (child = windowInfoNode->getChildByName("title"))
    title  = child->getValue();
  else
    title  = "Default Title";
    
  return create(title, displayMode, caps);
}

bool Window::create(const char* title,
                    bool  fullscreen,
                    bool  centered,
                    int   x,
                    int   y,
                    int   width,
                    int   height,
                    int   colorDepth,
                    int   colorBits,
                    int   depth,
                    int   stencil,
                    int   alpha,
                    int   accumulation,
                    int   samples)
{
  return create(title,
                DisplayMode(x, y, width, height, colorDepth, fullscreen, centered),
                GLCaps(accumulation, samples, stencil, alpha, colorBits, depth));
}

bool Window::exportXMLSettings(ofstream &xmlFile)
{
  return Logger::writeErrorLog("Window not exportable at this time");
}

void Window::setAppInstance(HINSTANCE instance)
{
  hInstance = instance;
}

void Window::updatePosition()
{
  GetWindowPlacement(windowHandle, &position);
  rectangle.x = position.rcNormalPosition.left;
  rectangle.y = position.rcNormalPosition.top;
}

void Window::updateDimensions(LPARAM param)
{
  rectangle.z = clamp(int(LOWORD(param)), 1, 2048);
  rectangle.w = clamp(int(HIWORD(param)), 1, 2048);
}

const HGLRC Window::getRenderingContext() const
{
  return renderingContext;
}
const HDC   Window::getDeviceContext()    const
{
  return deviceContext;
}
const HWND  Window::getHandle()           const
{
  return windowHandle;
}

const   Tuple4i &Window::getRectangle() const
{
  return rectangle;
}
Tuple2i Window::getDimensions()         const
{
  return Tuple2i(rectangle.z, rectangle.w);
}
Tuple2i Window::getPosition()           const
{
  return Tuple2i(rectangle.x, rectangle.y);
}

int     Window::getHeight() const
{
  return rectangle.w;
}
int     Window::getWidth()  const
{
  return rectangle.z;
}
int     Window::getX()      const
{
  return rectangle.x;
}
int     Window::getY()      const
{
  return rectangle.y;
}

void Window::makeCurrent()
{
  wglMakeCurrent(deviceContext, renderingContext);
}

void Window::update(float tick)
{
  SwapBuffers(deviceContext);
  
  if (snapshotScheduled)
  {
    snapshotScheduled = false;
    takeSnapshot(0);
  }
}

String Window::getTitle() const
{
  return title;
}

void Window::setTitle(const char* newTitle)
{
  SetWindowText(windowHandle, newTitle);
  title = newTitle;
}

void Window::setVisible(bool show)
{
  ShowWindow(windowHandle, show ? SW_SHOW : SW_HIDE);
  visible = show;
}

void Window::scheduleSnapshot()
{
  snapshotScheduled = true;
}

bool Window::isFullscreen() const
{
  return fullscreen;
}

void Window::setInputEventListener(InputEventListener *listener)
{
  inputEventListener = listener;
}

InputEventListener   *Window::getInputEventListener()
{
  return inputEventListener;
}

void Window::takeSnapshot(int format)
{
  ofstream screenShotFile;
  ifstream existingFile;
  char     filename[64];
  int      screensCount = 0,
                          height       = rectangle.w,
                                         width        = rectangle.z;
                                         
  while (true)
  {
    sprintf_s(filename, 64, "screenShot%03d.tga", screensCount);
    existingFile.open(filename);
    if (existingFile.is_open())
    {
      screensCount++;
      existingFile.close();
    }
    else
      break;
  }
  
  screenShotFile.open(filename, ios::binary);
  Logger::writeInfoLog(String("Saving screenshot -> ") + filename);
  
  if (!screenShotFile.is_open())
  {
    Logger::writeErrorLog("Failed to write file");
    return;
  }
  
  GLubyte  TGAheader[12] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                           infoHeader[6],
                           *data = new GLubyte[4*width*height];
                           
  //read in the screen data
  glReadPixels(0, 0, width, height, GL_BGRA_EXT, GL_UNSIGNED_BYTE, data);
  
  //save header
  screenShotFile.write((char*)TGAheader, sizeof(TGAheader));
  
  //save info header
  infoHeader[0] = (width  & 0x00FF);
  infoHeader[1] = (width  & 0xFF00) >> 8;
  infoHeader[2] = (height & 0x00FF);
  infoHeader[3] = (height & 0xFF00) >> 8;
  infoHeader[4] = 32;
  infoHeader[5] = 0;
  
  //save info header
  screenShotFile.write((char*)infoHeader, sizeof(infoHeader));
  
  //save the image data
  screenShotFile.write((char*)data, 4*width*height);
  screenShotFile.close();
  
  deleteArray(data);
}

void Window::destroy()
{
  if (!WindowsManager::removeWindow(this))
    return;
    
  Logger::writeInfoLog("Destroying GL Window");
  
  if (fullscreen)
  {        // Are We In Fullscreen Mode?
    ChangeDisplaySettings(NULL, 0);          // If So Switch Back To The Desktop
    ShowCursor(true);                        // Show Mouse Pointer
  }
  
  if (renderingContext)
  {
    if (!wglDeleteContext(renderingContext)) // Are We Able To Delete The RC?
      Logger::writeErrorLog("Release rendering context failed");
      
    renderingContext = NULL;                 // Set RC To NULL
  }
  
  if (deviceContext && !ReleaseDC(windowHandle, deviceContext))
  {
    Logger::writeErrorLog("Release device context failed");
    deviceContext = NULL;                                       // Set DC To NULL
  }
  
  if (windowHandle && !DestroyWindow(windowHandle))             // Are We Able To Destroy The Window?
  {
    Logger::writeErrorLog("Could not release window handle");
    windowHandle = NULL;                                        // Set hWnd To NULL
  }
  
  if (!UnregisterClass(title, appHandle))
  {
    Logger::writeErrorLog("Could not unregister class");
    appHandle = NULL;                                         // Set hInstance To NULL
  }
}

Window::~Window()
{
  destroy();
}
