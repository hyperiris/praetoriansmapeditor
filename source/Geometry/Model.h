#ifndef MODEL_H
#define MODEL_H

enum GeometriaTipo
{
  MODEL_RIGIDA = 1,
  MODEL_FLEXIBLE,
  MODEL_ANIMADA,
  MODEL_NODOS,
  MODEL_UNDEFINED
};

class Geometry;
class Appearance;
class TransformGroup;

class Model
{
  public:
    Model();
    Model(const Model& copy);
    Model& operator = (const Model& right);
    ~Model();
    
    void initialize();
    void destroy();
    
    void setGeometry(Geometry *geom);
    Geometry *getGeometry();
    
    void setAppearance(Appearance *app);
    Appearance *getAppearance();
    
    void setVisible(bool v);
    bool isVisible();
    
    void setParent(TransformGroup *group);
    TransformGroup *getParent();
    
    int draw(int format);
    void drawRegular(int format);
    
    void enableTextureUnit(int format);
    void disableTexureUnit(int format);
    
  private:
    Geometry* geometry;
    Appearance* appearance;
    TransformGroup *parent;
    bool visible;
};

#endif