#ifndef FLAGVIEWER
#define FLAGVIEWER

#include "Visitor.h"
#include "../Math/Tuple4.h"

class FlagViewer : public Visitor
{
  public:
    FlagViewer();
    ~FlagViewer();
    
  public:
    void visit(SpatialIndexBaseNode* node);
    void visit(SpatialIndexNode* node);
    void visit(SpatialIndexCell* node);
    
    void reset();
    
  public:
    void setLogic(unsigned int value);
    unsigned int getLogic();
    
    void setFlag(unsigned int value) {}
    unsigned int getFlag()
    {
      return 0;
    }
    
    void setMode(unsigned int value);
    unsigned int getMode();
    
    void setRenderColor(const Tuple4f& color);
    
  private:
    unsigned int logic;
    unsigned int mode;
    Tuple4f renderColor[9];
};

#endif