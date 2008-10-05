#ifndef MODELREVEALER
#define MODELREVEALER

#include "Visitor.h"
#include "../Math/Tuple4.h"

class Frustum;
class Camera;

class TileRevealer : public Visitor
{
  public:
    TileRevealer();
    ~TileRevealer();
    
    void visit(SpatialIndexBaseNode* node);
    void visit(SpatialIndexNode* node);
    void visit(SpatialIndexCell* node);
    
    void setFrustum(Frustum *frus);
    void setCamera(Camera *cam);
    void setRadius(float r);
    
    void reset();
    
  private:
    void cellVisibility(const Tuple4i& range, bool visibility);
    void tileVisibility(const Tuple4i& range, bool visibility);
    
  private:
    Frustum *frustum;
    Camera *camera;
    Tuple3f eyePos;
    float radius;
};

#endif