#ifndef WINDOWEVENT_H
#define WINDOWEVENT_H

class WindowEvent
{
  public:
    WindowEvent(int x, int y, int width, int height);
    WindowEvent(const WindowEvent& copy);
    WindowEvent &operator= (const WindowEvent& copy);
    
    int  getHeight();
    int  getWidth();
    int  getX();
    int  getY();
    
  private:
    int height,
    width,
    x,
    y;
};

#endif