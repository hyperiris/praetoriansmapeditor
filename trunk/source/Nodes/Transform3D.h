#ifndef TRANSFORM3D_H
#define TRANSFORM3D_H

#include "../Math/Matrix4.h"
#include "../Math/Quaternion.h"
#include "../Tools/Logger.h"

class Transform3D : public IOXMLObject
{
  private:
    Matrix4f translations,
    rotations,
    inverse,
    matrix,
    scales;
    
    bool     applyInverse,
    updated,
    apply;
    
    void     forceUpdate();
    
  public:
    Transform3D();
    Transform3D(const Transform3D &t);
    Transform3D(const Matrix4f & m);
    Transform3D &operator =(const Transform3D &t);
    
    virtual bool loadXMLSettings(XMLElement *element);
    virtual bool exportXMLSettings(ofstream &xmlFile);
    
    operator const float*();
    operator float*();
    
    const Matrix4f &getInverseMatrix4f() const;
    const Matrix4f &getMatrix4f()        const;
    
    void overrideUpdatedFlag(bool);
    bool updatedFlagOn();
    
    void undoTransform();
    void doTransform();
    
    void rotateAxis(const Tuple3f &axis, float angle);
    void rotateXYZ(const Tuple3f &angles);
    void rotateXYZ(Quaternionf &quat);
    void rotateXYZ(float ax,float ay, float az);
    void rotateX(float angle);
    void rotateY(float angle);
    void rotateZ(float angle);
    
    void setTranslations(const Tuple3f &v);
    void setTranslations(float x,float y, float z);
    
    void setInverseTranspose();
    void setInverse();
    
    void setTranspose();
    void setIdentity();
    
    void setScales(float scalesXYZ);
    void setScales(const Tuple3f &);
    void setScales(float scaleX, float scaleY, float scaleZ);
    
    void set(const Transform3D &t);
    void set(const Matrix4f &mat);
    
    bool isTransformIdentity();
    bool isInverseIdentity();
};
#endif