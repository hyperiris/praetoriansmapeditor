#ifndef GUI_PANEL_H
#define GUI_PANEL_H

#include "../Math/MathUtils.h"
#include "../Appearance/Texture.h"
#include "../Tools/Logger.h"

#include "../Geometry/DistanceObject.h"
#include "GUITexCoordDescriptor.h"
#include "GUIClippedRectangle.h"
#include "GUIAlphaElement.h"
#include "GUIText.h"

typedef DistanceObject<GUIRectangle*> ZWidget;
typedef vector<GUIRectangle *>        Widgets;
typedef vector<ZWidget>               ZWidgets;

enum
{
  FREE_LAYOUT,
  YAXIS_LAYOUT,
  YAXIS_CEN_LAYOUT,
  XAXIS_LAYOUT,
  GRID_LAYOUT
};

class GUIPanel : public GUIRectangle, public GUIClippedRectangle
{
  protected:
    Widgets  elements;
    Tuple2i  interval;
    int      layout;
    
  private:
    void    packFreeLayout();
    void    packYAxisLayout();
    void    packXAxisLayout();
    void    correctPosition();
    
  public:
    GUIPanel(const char *callback = NULL);
    ~GUIPanel();
    GUIRectangle *getWidgetByCallbackString(const char *name);
    
    void  setInterval(int width, int height);
    void  setInterval(const Tuple2i &dimensions);
    const Tuple2i &getInterval();
    
    void  setLayout(int layout);
    int   getLayout();
    
    int   getTreeHeight();
    
    virtual bool  loadXMLFile(const char *stackPath);
    virtual bool  loadXMLSettings(XMLElement *node);
    
    virtual void  notify(GUIRectangle *element);
    virtual void  checkMouseEvents(MouseEvent  &evt, int extraInfo, bool bitInfo = false);
    virtual void  checkKeyboardEvents(KeyEvent evt, int extraInfo);
    virtual void  render(float tick);
    
    virtual void  collectZWidgets(ZWidgets &container);
    virtual void  forceUpdate(bool);
    virtual bool  addWidget(GUIRectangle *widget);
    virtual void  pack();
    virtual void  clear();
    
    int           getWidgetCountByType(int type);
    Widgets      &getWidgets();
};

#endif
