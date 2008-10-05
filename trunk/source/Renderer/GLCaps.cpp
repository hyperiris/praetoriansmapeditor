#include "GLCaps.h"
#include "../Math/MathUtils.h"
#include "../Managers/WindowsManager.h"

GLCaps GLCaps::maxCaps;

GLCaps::GLCaps(int  accumulation,
               int  samples,
               int  stencil,
               int  alpha,
               int  color,
               int  depth,
               bool doubleBuffer,
               bool vsyncEnabled) : IOXMLObject("GLCaps")
{
  set(accumulation, samples, stencil, alpha,
      color, depth, doubleBuffer, vsyncEnabled);
}

GLCaps::GLCaps(const GLCaps &copy) : IOXMLObject("GLCaps")
{
  operator =(copy);
}

GLCaps &GLCaps::operator=(const GLCaps &copy)
{
  accumulation = copy.accumulation;
  doubleBuffer = copy.doubleBuffer;
  samples      = copy.samples;
  stencil      = copy.stencil;
  alpha        = copy.alpha;
  color        = copy.color;
  depth        = copy.depth;
  vsync        = copy.vsync;
  
  return *this;
}

bool GLCaps::operator==(const GLCaps &compare)
{
  return (doubleBuffer == compare.doubleBuffer) &&
         (accumulation == compare.accumulation) &&
         (samples      == compare.samples) &&
         (stencil      == compare.stencil) &&
         (alpha        == compare.alpha) &&
         (color        == compare.color) &&
         (depth        == compare.depth) &&
         (vsync        == compare.vsync) ;
}

bool GLCaps::operator!=(const GLCaps &compare)
{
  return !this->operator==(compare);
}

bool GLCaps::loadXMLSettings(XMLElement *element)
{
  if (!isSuitable(element))
    return false;
    
  XMLElement    *child     = NULL;
  
  for (size_t i = 0; i < element->getChildrenCount(); i++)
  {
    if (!(child = element->getChild(i)))
      continue;
      
    const String &childName = child->getName();
    
    if (childName == "accumulation")
    {
      setAccumulation(child->getValuei());
      continue;
    }
    
    if (childName == "samples")
    {
      setMultiSampleLevel(child->getValuei());
      continue;
    }
    
    if (childName == "vsync")
    {
      setVSyncFlag((child->getValue() == "true"));
      continue;
    }
    
    if (childName == "doubleBuffer")
    {
      setDoubleBufferFlag((child->getValue() == "true"));
      continue;
    }
    
    if (childName == "stencil")
    {
      setStencilBits(child->getValuei());
      continue;
    }
    
    if (childName == "alpha")
    {
      setAlphaBits(child->getValuei());
      continue;
    }
    
    if (childName == "color")
    {
      setColorBits(child->getValuei());
      continue;
    }
    
    if (childName == "depth")
    {
      setDepthBits(child->getValuei());
      continue;
    }
  }
  return true;
}

void GLCaps::set(int  accumulation,
                 int  samples,
                 int  stencil,
                 int  alpha,
                 int  color,
                 int  depth,
                 bool doubleBuffer,
                 bool vsyncEnabled)
{
  setAccumulation(accumulation);
  setMultiSampleLevel(samples);
  setDoubleBufferFlag(doubleBuffer);
  setVSyncFlag(vsyncEnabled);
  setStencilBits(stencil);
  setColorBits(color);
  setAlphaBits(alpha);
  setDepthBits(depth);
}

bool GLCaps::exportXMLSettings(ofstream &xmlFile)
{
  if (!xmlFile.is_open())
    return Logger::writeErrorLog("Cannot export GLCaps to XML: file not ready.");
    
  xmlFile << "<GLCaps  accumulation = " << accumulation << "\"\n"
  << "         doubleBuffer = " << (doubleBuffer ? "true" : "false")  << "\"\n"
  << "         samples      = " << samples      << "\"\n"
  << "         stencil      = " << stencil      << "\"\n"
  << "         alpha        = " << alpha        << "\"\n"
  << "         depth        = " << depth        << "\"\n"
  << "         color        = " << color        << "\"\n"
  << "         vsync        = " << (vsync ? "true" : "false") << "\" /> \n";
  
  return true;
}

const String GLCaps::toString() const
{
  return String("accumulation: ")    + accumulation +
         String(", doubleBuffer: ")  + (doubleBuffer ? "true" : "false") +
         String(", samples: ")       +  samples +
         String(", stencil: ")       +  stencil +
         String(", alpha: ")         +  alpha   +
         String(", depth: ")         +  depth   +
         String(", color: ")         +  color   +
         String(", vsync: ")         + (vsync ? "true" : "false");
}

void GLCaps::setVSyncFlag(bool on)
{
  vsync = on;
}

const bool GLCaps::getVSyncFlag() const
{
  return vsync;
}

void GLCaps::setDoubleBufferFlag(bool on)
{
  doubleBuffer = on;
}

const bool GLCaps::getDoubleBufferFlag() const
{
  return doubleBuffer;
}

void GLCaps::setAccumulation(int accumulationArg)
{
  accumulation = clamp(accumulationArg, 0, 1024);
}

const int  GLCaps::getAccumulation() const
{
  return accumulation;
}

void GLCaps::setMultiSampleLevel(int samplesArg)
{
  samplesArg = clamp(samplesArg, 0, 16);
  samples    = samplesArg ? getClosestPowerOfTwo(samplesArg) : 0;
}

const int  GLCaps::getMultiSampleLevel() const
{
  return samples;
}

void GLCaps::setStencilBits(int stencilArg)
{
  stencil = clamp(stencilArg, 0, 8);
}

const int  GLCaps::getStencilBits() const
{
  return stencil;
}

void GLCaps::setColorBits(int colorArg)
{
  color = clamp(colorArg, 0, 32);
}

const int  GLCaps::getColorBits() const
{
  return color;
}

void GLCaps::setAlphaBits(int alphaArg)
{
  alpha = clamp(alphaArg, 0, 8);
}

const int  GLCaps::getAlphaBits() const
{
  return alpha;
}

void GLCaps::setDepthBits(int depthArg)
{
  depthArg = clamp(depthArg, 0, 24);
  
  switch (depthArg)
  {
    case  0:
    case  8:
    case 16:
    case 24:
      depth = depthArg;
      return;
  }
  
  depth = (depthArg <  8) ?  8 :
          (depthArg < 16) ? 16 : 24;
}

const int  GLCaps::getDepthBits() const
{
  return depth;
}

const GLCaps &GLCaps::getMaxCaps()
{
  initialize();
  return maxCaps;
}

//Check out NeHe and paulsprojects.net for more details
bool GLCaps::initialize()
{
  static bool initialized = false,
                            valid       = false;
                            
  if (initialized)
    return valid;
    
  int    attributes[11],
  results[11];
  
  maxCaps.set(0,0,0,0,0,0,0,0);
  initialized = true;
  
  HINSTANCE dummyModuleHandle =  GetModuleHandle(NULL);
  WNDCLASS  windowsClassDummy;
  HGLRC     dummyRenderingContext;
  HWND      windowHandleDummy;
  HDC       hDC;
  GLuint    pixelFormat;
  DWORD     dwExStyleDummy = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
  DWORD     dwStyleDummy   = WS_OVERLAPPEDWINDOW;
  
  windowsClassDummy.style            = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
  windowsClassDummy.lpfnWndProc      = (WNDPROC)WindowsManager::windowEventsProcessor;
  windowsClassDummy.cbClsExtra       = 0;
  windowsClassDummy.cbWndExtra       = 0;
  windowsClassDummy.hInstance        = dummyModuleHandle;
  windowsClassDummy.hIcon            = LoadIcon(NULL, IDI_WINLOGO);
  windowsClassDummy.hCursor          = LoadCursor(NULL, IDC_ARROW);
  windowsClassDummy.hbrBackground    = NULL;
  windowsClassDummy.lpszMenuName     = NULL;
  windowsClassDummy.lpszClassName    = "Dummy";
  
  if (!RegisterClass(&windowsClassDummy))
    return Logger::writeErrorLog("Failed To Register Dummy Window Class");
    
  if (!(windowHandleDummy=CreateWindowEx(dwExStyleDummy,
                                         "Dummy", "Dummy",
                                         dwStyleDummy | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
                                         0, 0, 128, 128, 0, 0, dummyModuleHandle, 0)))
  {
    UnregisterClass("Dummy", NULL);
    return Logger::writeErrorLog("Can't Create A Dummy GL Device Context");
  }
  
  PIXELFORMATDESCRIPTOR pfd =
  {
    sizeof(PIXELFORMATDESCRIPTOR),
    1,
    PFD_DRAW_TO_WINDOW |
    PFD_SUPPORT_OPENGL,
    PFD_TYPE_RGBA,
    16,
    0, 0, 0, 0, 0, 0,
    0,
    0,
    0,
    0, 0, 0, 0,
    16,
    0,
    0,
    PFD_MAIN_PLANE,
    0,
    0, 0, 0
  };
  
  if (!(hDC =  GetDC(windowHandleDummy)))
  {
    DestroyWindow(windowHandleDummy);
    UnregisterClass("Dummy", dummyModuleHandle);
    return Logger::writeErrorLog("Can't Create A Dummy GL Device Context");
  }
  
  if (!(pixelFormat=ChoosePixelFormat(hDC,&pfd)))
  {
    ReleaseDC(windowHandleDummy, hDC);
    DestroyWindow(windowHandleDummy);
    UnregisterClass("Dummy", dummyModuleHandle);
    return Logger::writeErrorLog("Can't Find A Suitable Dummy PixelFormat");
  }
  
  if (!SetPixelFormat(hDC, pixelFormat,&pfd))
  {
    ReleaseDC(windowHandleDummy, hDC);
    DestroyWindow(windowHandleDummy);
    UnregisterClass("Dummy", dummyModuleHandle);
    return  Logger::writeErrorLog("Can't Set The PixelFormat");
  }
  
  if (!(dummyRenderingContext = wglCreateContext(hDC)))
  {
    ReleaseDC(windowHandleDummy, hDC);
    DestroyWindow(windowHandleDummy);
    UnregisterClass("Dummy", dummyModuleHandle);
    return Logger::writeErrorLog("Can't Create A Dummy GL Rendering Context");
  }
  
  if (!wglMakeCurrent(hDC, dummyRenderingContext))
  {
    ReleaseDC(windowHandleDummy, hDC);
    DestroyWindow(windowHandleDummy);
    UnregisterClass("Dummy", dummyModuleHandle);
    wglDeleteContext(dummyRenderingContext);
    return Logger::writeErrorLog("Can't Activate Dummy GL Rendering Context");
  }
  
  attributes[0]  = WGL_NUMBER_PIXEL_FORMATS_ARB;
  wglGetPixelFormatAttribivARB(hDC, 1, 0, 1, attributes, results);
  int    numPfds = results[0];
  
  //A list of attributes to check for each pixel format
  attributes[ 0]  =  WGL_COLOR_BITS_ARB;    //bits
  attributes[ 1]  =  WGL_ALPHA_BITS_ARB;
  attributes[ 2]  =  WGL_DEPTH_BITS_ARB;
  attributes[ 3]  =  WGL_STENCIL_BITS_ARB;
  
  attributes[ 4]  =  WGL_DRAW_TO_WINDOW_ARB;  //required to be true
  attributes[ 5]  =  WGL_SUPPORT_OPENGL_ARB;
  attributes[ 6]  =  WGL_DOUBLE_BUFFER_ARB;
  
  attributes[ 7]  =  WGL_ACCELERATION_ARB;  //required to be FULL_ACCELERATION_ARB
  
  attributes[ 8]  =  WGL_SAMPLE_BUFFERS_ARB;  //Multisample
  attributes[ 9]  =  WGL_SAMPLES_ARB;
  
  attributes[10]  =  WGL_ACCUM_BITS_ARB;
  
  for (int i=0; i<numPfds; ++i)
  {
    if (wglGetPixelFormatAttribivARB(hDC, i+1, 0, 11, attributes, results))
    {
      maxCaps.setDoubleBufferFlag(results[6] ? true : maxCaps.getDoubleBufferFlag());
      maxCaps.setMultiSampleLevel((maxCaps.getMultiSampleLevel()  < results[9]) ? results[9]   : maxCaps.getMultiSampleLevel());
      maxCaps.setAccumulation((maxCaps.getAccumulation() < results[10]) ? results[10]   : maxCaps.getAccumulation());
      maxCaps.setStencilBits((maxCaps.getStencilBits()   < results[ 3]) ? results[ 3]   : maxCaps.getStencilBits());
      maxCaps.setColorBits((maxCaps.getColorBits()*4     < results[ 0]) ? results[ 0]/4 : maxCaps.getColorBits());
      maxCaps.setAlphaBits((maxCaps.getAlphaBits()       < results[ 1]) ? results[ 1]   : maxCaps.getAlphaBits());
      maxCaps.setDepthBits((maxCaps.getDepthBits()       < results[ 2]) ? results[ 2]   : maxCaps.getDepthBits());
    }
  }
  
  maxCaps.setVSyncFlag((GLEE_WGL_EXT_swap_control == GL_TRUE));
  
  ReleaseDC(windowHandleDummy, hDC);
  DestroyWindow(windowHandleDummy);
  UnregisterClass("Dummy", dummyModuleHandle);
  wglDeleteContext(dummyRenderingContext);
  dummyRenderingContext = NULL;
  
  return (valid = true);
}

void  GLCaps::adjustCaps(GLCaps &caps)
{
  initialize();
  
  caps.setMultiSampleLevel((caps.getMultiSampleLevel() > maxCaps.getMultiSampleLevel()) ? maxCaps.getMultiSampleLevel() : caps.getMultiSampleLevel());
  caps.setDoubleBufferFlag((caps.getDoubleBufferFlag() && maxCaps.getDoubleBufferFlag()));
  caps.setAccumulation((caps.getAccumulation() > maxCaps.getAccumulation()) ? maxCaps.getAccumulation() : caps.getAccumulation());
  caps.setStencilBits((caps.getStencilBits()   > maxCaps.getStencilBits())  ? maxCaps.getStencilBits()  : caps.getStencilBits());
  caps.setColorBits((caps.getColorBits()       > maxCaps.getColorBits())    ? maxCaps.getColorBits()    : caps.getColorBits());
  caps.setAlphaBits((caps.getAlphaBits()       > maxCaps.getAlphaBits())    ? maxCaps.getAlphaBits()    : caps.getAlphaBits());
  caps.setDepthBits((caps.getDepthBits()       > maxCaps.getDepthBits())    ? maxCaps.getDepthBits()    : caps.getDepthBits());
  caps.setVSyncFlag((caps.getVSyncFlag() && maxCaps.getVSyncFlag()));
}