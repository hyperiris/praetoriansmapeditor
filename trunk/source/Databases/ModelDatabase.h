#ifndef MODELDATABASE
#define MODELDATABASE

#include "../Tools/NamedObject.h"
#include "../Containers/ocarray.h"
#include "../Factories/FactoryUtils.h"
#include "../Tools/ICString.h"
#include "../Appearance/Texture.h"

class TransformGroup;
class ModelController;

class ModelDatabase : public NamedObject
{
  public:
    ModelDatabase(const char* dbName = "Modelos");
    virtual ~ModelDatabase();
    
  public:
    virtual bool initialize(const char* path);
    virtual void compile();
    virtual void reset();
    
    virtual TransformGroup* buildPrototype(const char* name);
    virtual TransformGroup* getPrototype(const char* name);
    
    virtual void renderPrototypeImage(const char* name, const Transform3D& transform, const Tuple2i& size, int format);
    
    virtual ModelController* instantiateModel(const char* name);
    ModelController* getModelController(unsigned int index);
    unsigned int getModelControllerCount();
    
    void setType(int typeVal);
    int getType(const char* objName);
    
    TransformGroup* getRootGroup();
    
    const Array <ICString>* getNamesList();
    
  protected:
    virtual ModelController* createController(TransformGroup* group);
    
  protected:
    Array <ICString> registeredModelNames;
    ArrayPtr <ModelController> modelControllers;
    TransformGroup* rootGroup;
    int dbtype;
};

#endif