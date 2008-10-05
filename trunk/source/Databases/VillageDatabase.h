#ifndef VILLAGEDATABASE
#define VILLAGEDATABASE

#include "ModelDatabase.h"
#include "../Containers/ocavltree.h"

class TransformGroup;
class XMLStack;
class XMLElement;

class VillageDatabase : public ModelDatabase
{
  public:
    VillageDatabase(const char* dbName = "VillageDatabase");
    ~VillageDatabase();
    
  public:
    bool initialize(const char* path);
    void compile() {}
    
    TransformGroup* buildPrototype(const char* name);
    
  protected:
    ModelController* createController(TransformGroup* group);
    
  private:
    XMLStack* xmlStack;
    AVLTree <XMLElement*> villageElementsAVLTree;
};

#endif