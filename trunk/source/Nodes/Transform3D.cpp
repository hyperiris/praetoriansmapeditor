#include "Transform3D.h"
#include "../Managers/MediaPathManager.h"

Transform3D::Transform3D() : IOXMLObject("Transform")
{
  setIdentity();
}

Transform3D::Transform3D(const Transform3D &transform)  : IOXMLObject("Transform")
{
  this->operator=(transform);
}

Transform3D::Transform3D(const Matrix4f & matrix)  : IOXMLObject("Transform")
{
  set(matrix);
}

Transform3D &Transform3D::operator= (const Transform3D &copy)
{
  if (this != &copy)
    set(copy);
  return *this;
}

Transform3D::operator const float*()
{
  return matrix.m;
}

Transform3D::operator float*()
{
  return matrix.m;
}

const Matrix4f &Transform3D::getMatrix4f() const
{
  return matrix;
}

const Matrix4f &Transform3D::getInverseMatrix4f() const
{
  return inverse;
}

void Transform3D::forceUpdate()
{
  matrix.setIdentity();
  matrix *= translations;
  matrix *= rotations;
  matrix *= scales;
  
  inverse = matrix;
  inverse.setInverse();
  updated = true;
  
  applyInverse = (inverse != IDENTITY);
  apply        = (matrix  != IDENTITY);
}

void Transform3D::doTransform()
{
  if (!apply)
    return;
    
  glPushMatrix();
  glMultMatrixf(matrix);
}

void Transform3D::undoTransform()
{
  if (!apply)
    return;
  glPopMatrix();
}

void Transform3D::setScales(const Tuple3f &v)
{
  scales.setScales(v.x, v.y, v.z);
  forceUpdate();
}

void Transform3D::setScales(float s)
{
  setScales(s, s, s);
}

void Transform3D::setScales(float scalex, float scaley, float scalez)
{
  scales.setScales(scalex, scaley, scalez);
  forceUpdate();
}

void Transform3D::setIdentity()
{
  applyInverse = false;
  updated      = true;
  apply        = false;
  
  translations.setIdentity();
  rotations.setIdentity();
  inverse.setIdentity();
  scales.setIdentity();
  matrix.setIdentity();
}

void Transform3D::setTranspose()
{
  matrix.setTranspose();
  inverse = matrix;
  inverse.setInverse();
  
  applyInverse = (inverse != IDENTITY);
  updated      = true;
  apply        = (matrix  != IDENTITY);
}

void Transform3D::rotateAxis(const Tuple3f &axis, float angle)
{
  rotations.rotateAxis(angle, axis);
  forceUpdate();
}

void Transform3D::rotateXYZ(Quaternionf &quat)
{
  rotations = quat.convertToMatrix();
  forceUpdate();
}

void Transform3D::rotateXYZ(const Tuple3f &angles)
{
  rotations.rotateXYZ(angles.x, angles.y, angles.z);
  forceUpdate();
}

void Transform3D::rotateXYZ(float ax,float ay, float az)
{
  rotations.rotateXYZ(ax, ay, az);
  forceUpdate();
}

void Transform3D::rotateX(float angle)
{
  rotations.rotateX(angle);
  forceUpdate();
}

void Transform3D::rotateY(float angle)
{
  rotations.rotateY(angle);
  forceUpdate();
}

void Transform3D::rotateZ(float angle)
{
  rotations.rotateZ(angle);
  forceUpdate();
}

void Transform3D::setTranslations(const Tuple3f &v)
{
  translations.setTranslations(v);
  forceUpdate();
}

void Transform3D::setTranslations(float x,float y, float z)
{
  translations.setTranslations(x, y, z);
  forceUpdate();
}

void Transform3D::set(const Transform3D &copy)
{
  if (this != &copy)
  {
    updated        = copy.updated;
    
    translations   = copy.translations;
    rotations      = copy.rotations;
    inverse        = copy.inverse;
    scales         = copy.scales;
    matrix         = copy.matrix;
    
    applyInverse   = (inverse != IDENTITY);
    apply          = (matrix  != IDENTITY);
  }
}

void Transform3D::set(const Matrix4f &matrixArg)
{
  setIdentity();
  matrix  = matrixArg;
  inverse = matrix;
  inverse.setInverse();
  
  applyInverse = (inverse != IDENTITY);
  apply        = (matrix  != IDENTITY);
}

bool Transform3D::isInverseIdentity()
{
  return !applyInverse;
}

bool Transform3D::isTransformIdentity()
{
  return apply;
}

void Transform3D::setInverseTranspose()
{
  matrix.setInverseTranspose();
  inverse = matrix;
  inverse.setInverse();
  
  applyInverse = (inverse != IDENTITY);
  updated      = true;
  apply        = (matrix  != IDENTITY);
}

void Transform3D::setInverse()
{
  matrix.setInverse();
  inverse = matrix;
  inverse.setInverse();
  
  applyInverse = (inverse != IDENTITY);
  updated      = true;
  apply        = (matrix  != IDENTITY);
}

bool Transform3D::loadXMLSettings(XMLElement *element)
{
  if (!isSuitable(element))
    return  false;
    
  XMLElement *child;
  
  if (child = element->getChildByName("Row"))
  {
    XMLElement *subChild;
    Tuple4f     xyzw;
    int         offset = 0;
    for (size_t i = 0; i < element->getChildrenCount(); i++)
    {
      if (offset >= 16)
        return true;
        
      if (!(subChild = element->getChild(i)))
        continue;
      xyzw.set(0.0f, 0.0f, 0.0f, 0.0f);
      if (subChild->getName() == "Row")
      {
        XMLElement::loadRX_GY_BZ_AWf(*subChild, xyzw);
        updated = true;
        matrix[offset + 0] = xyzw.x;
        matrix[offset + 1] = xyzw.y;
        matrix[offset + 2] = xyzw.z;
        matrix[offset + 3] = xyzw.w;
        offset += 4;
      }
    }
    return true;
  }
  
  Tuple3f translations,
  rotations,
  scales(1.0f, 1.0f, 1.0f);
  
  if (child = element->getChildByName("Scales"))
    XMLElement::loadRX_GY_BZf(*child, scales);
    
    
  if (child = element->getChildByName("Rotations"))
    XMLElement::loadRX_GY_BZf(*child, rotations);
    
    
  if (child = element->getChildByName("Translations"))
    XMLElement::loadRX_GY_BZf(*child, translations);
    
  this->translations.setTranslations(translations);
  this->rotations.rotateXYZ(rotations);
  this->scales.setScales(scales);
  forceUpdate();
  return true;
}

void Transform3D::overrideUpdatedFlag(bool flag)
{
  updated = flag;
}

bool Transform3D::updatedFlagOn()
{
  return updated;
}

bool Transform3D::exportXMLSettings(ofstream &xmlFile)
{
  if (!xmlFile.is_open())
    return Logger::writeErrorLog("Cannot export Transform to XML: file not ready.");
  forceUpdate();
  xmlFile << "  <Transform>\n"
  << "    <Row x = \"" << matrix[ 0] << "\" y = \"" << matrix[ 1] << "\" z = \"" << matrix[ 2] << "\" w = \"" << matrix[ 3] << "\" />\n"
  << "    <Row x = \"" << matrix[ 4] << "\" y = \"" << matrix[ 5] << "\" z = \"" << matrix[ 6] << "\" w = \"" << matrix[ 7] << "\" />\n"
  << "    <Row x = \"" << matrix[ 8] << "\" y = \"" << matrix[ 9] << "\" z = \"" << matrix[10] << "\" w = \"" << matrix[11] << "\" />\n"
  << "    <Row x = \"" << matrix[12] << "\" y = \"" << matrix[13] << "\" z = \"" << matrix[14] << "\" w = \"" << matrix[15] << "\" />\n"
  << "  </Transform>\n";
  return true;
}