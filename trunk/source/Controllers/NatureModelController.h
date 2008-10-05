#ifndef NATURECONTROLLER
#define NATURECONTROLLER

#include "ModelController.h"

class NatureModelController : public ModelController
{
  public:
    NatureModelController();
    NatureModelController(TransformGroup* group);
    ~NatureModelController();
    
  public:
    void setWindFactor(float factor);
    float getWindFactor();
    
  private:
    float windFactor;
};

#endif