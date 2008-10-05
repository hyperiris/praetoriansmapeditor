#ifndef INPUTEVENTLISTENER_H
#define INPUTEVENTLISTENER_H

#include "WindowEvent.h"
#include "MouseEvent.h"
#include "KeyEvent.h"

class InputEventListener
{
  public:
    virtual void windowPositionChanged(WindowEvent evt) = 0;
    virtual void windowSizeChanged(WindowEvent evt) = 0;
    virtual void windowMaximized(WindowEvent evt) = 0;
    virtual void windowMinimized(WindowEvent evt) = 0;
    virtual void windowClosing(WindowEvent evt) = 0;
    
    virtual void mouseDoubleClicked(MouseEvent& evt) = 0;
    virtual void mouseScrolled(MouseEvent& evt) = 0;
    virtual void mouseReleased(MouseEvent& evt) = 0;
    virtual void mouseClicked(MouseEvent& evt) = 0;
    virtual void mouseDragged(MouseEvent& evt) = 0;
    virtual void mouseMoved(MouseEvent& evt) = 0;
    virtual void mouseExit(MouseEvent& evt) = 0;
    
    virtual void keyReleased(KeyEvent evt) = 0;
    virtual void keyPressed(KeyEvent evt) = 0;
};

#endif
