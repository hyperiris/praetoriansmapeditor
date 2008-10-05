#ifndef WINDOW_H
#define WINDOW_H

#include "../Tools/Logger.h"
#include "../GUI/GUIFrame.h"
#include "../Events/InputEventListener.h"
#include "../Renderer/GLCaps.h"
#include "DisplayMode.h"

#include <cstdlib>
#include <ctime>

class Window : public IOXMLObject
{
  public:
    Window(HINSTANCE  currAppInstance = NULL);
    ~Window();
    
    bool loadXMLSettings(XMLElement *element);
    bool exportXMLSettings(ofstream &xmlFile);
    
    bool create(const char* title,
                bool  fullscreen   = false,
                bool  centered     =  true,
                int   x            =     0,
                int   y            =     0,
                int   width        =   640,
                int   height       =   480,
                int   colorDepth   =    32,
                int   colorBits    =    32,
                int   depth        =    24,
                int   stencil      =     0,
                int   alpha        =     0,
                int   accumulation =     0,
                int   samples      =     0);
                
    bool  create(const char* title, DisplayMode &mode, GLCaps &caps);
    
    void setInputEventListener(InputEventListener *inputEventListener);
    InputEventListener *getInputEventListener();
    
    
    const HGLRC getRenderingContext() const;
    const HWND  getHandle()           const;
    const HDC   getDeviceContext()    const;
    
    void     updateDimensions(LPARAM param);
    void     updatePosition();
    void     destroy();
    void     update(float tick = 0.0f);
    
    void     setAppInstance(HINSTANCE instance);
    void     setVisible(bool);
    
    String   getTitle() const;
    void     setTitle(const char*);
    
    void     scheduleSnapshot();
    void     makeCurrent();
    bool     isFullscreen() const;
    
    const    GLCaps  &getRenderingCaps() const;
    const    Tuple4i &getRectangle()     const;
    Tuple2i  getDimensions() const;
    Tuple2i  getPosition() const;
    
    int      getHeight() const;
    int      getWidth() const;
    int      getX() const;
    int      getY() const;
    
  private:
    void  takeSnapshot(int formatBMPorTGA);
    InputEventListener  *inputEventListener;
    
    WINDOWPLACEMENT position;
    HINSTANCE       appHandle,
    hInstance;
    
    HGLRC           renderingContext; // Permanent Rendering Context
    HWND            windowHandle ;    // Holds Our Window Handle
    HDC             deviceContext;    // Private GDI Device Context
    
    String          title;
    
    bool            snapshotScheduled,
    fullscreen,
    visible;
    GLCaps          renderingCaps;
    Tuple4i         rectangle;
};

#endif
