#ifndef DISPLAY_MODE_H
#define DISPLAY_MODE_H

#include "../Tools/Logger.h"

class DisplayMode : public IOXMLObject
{
  public:
    DisplayMode(int  x          =     0, int  y           =     0,
                int  width      =   640, int  height      =   480,
                int  colorDepth =    32, bool fullscreen  = false,
                bool centered   = true);
                
    DisplayMode(const DisplayMode &copy);
    DisplayMode &operator=(const DisplayMode &copy);
    
    void   set(int  x, int  y, int width, int height, int colorDepth,
               bool fullscreen, bool centered);
               
    virtual bool loadXMLSettings(XMLElement *element);
    virtual bool exportXMLSettings(ofstream &xmlFile);
    
    bool   operator==(const DisplayMode &compare);
    bool   operator!=(const DisplayMode &compare);
    
    void   setColorDepth(int colorDepthArg);
    int    getColorDepth();
    
    void   setRefreshRate(int refresh);
    int    getRefreshRate();
    
    void   setHeight(int heightArg);
    int    getHeight();
    
    void   setWidth(int widthArg);
    int    getWidth();
    
    void   setPosition(int x, int y);
    void   setXPosition(int x);
    void   setYPosition(int y);
    int    getXPosition();
    int    getYPosition();
    
    void   setFullscreen(bool);
    bool   isFullscreen();
    
    void   setCentered(bool);
    bool   isCentered();
    
    String toString();
    
  private:
    bool fullscreen,
    centered;
    
    int refreshRate,
    colorDepth,
    height,
    width,
    x,
    y;
};
#endif