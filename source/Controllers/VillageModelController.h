#ifndef VILLAGECONTROLLER
#define VILLAGECONTROLLER

#include "ModelController.h"

class VillageModelController : public ModelController
{
  public:
    VillageModelController();
    VillageModelController(TransformGroup* group);
    ~VillageModelController();
    
  public:
    void setPopulation(unsigned int n);
    void setMaxPopulation(unsigned int n);
    
    unsigned int getPopulation();
    unsigned int getMaxPopulation();
    
  private:
    unsigned int population;
    unsigned int maxpopulation;
};

#endif