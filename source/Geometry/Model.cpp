#include "Model.h"
#include "Geometry.h"
#include "../Appearance/Appearance.h"
#include "../Managers/GeometryManager.h"

Model::Model()
{
  initialize();
}

Model::Model(const Model &copy)
{
  if (this != &copy)
    initialize();
    
  this->operator = (copy);
}

Model& Model::operator = (const Model &right)
{
  if (this != &right)
  {
    if (right.appearance)
    {
      if (!appearance)
        appearance = new Appearance();
      else
        appearance->destroy();
        
      appearance->operator = (*right.appearance);
    }
    
    if (right.geometry)
      setGeometry(right.geometry);
      
    visible = right.visible;
  }
  
  return *this;
}

void Model::initialize()
{
  geometry = 0;
  appearance = 0;
  parent = 0;
  visible = true;
}

int Model::draw(int format)
{
  if (!appearance || !geometry || !visible)
    return 0;
    
  int vertexformat = geometry->getVertexFormat();
  
  //if (format & MATERIAL && (appearance->getMaterial()))
  //  appearance->getMaterial()->bindMaterial();
  
  if (format & BLEND_ATTR)
    appearance->getBlendAttributes().apply();
    
  if (format & ALPHA_ATTR)
    appearance->getAlphaAttributes().apply();
    
  if (appearance->colorOn())
    glColor4fv(appearance->getColor());
    
  //if (!appearance->depthMaskEnabled())
  //  glDisable(GL_DEPTH_TEST);
  //  //glDepthMask(GL_FALSE);
  
  enableTextureUnit(vertexformat);
  drawRegular(vertexformat);
  disableTexureUnit(vertexformat);
  
  if (format & BLEND_ATTR)
    appearance->getBlendAttributes().stop();
    
  if (format & ALPHA_ATTR)
    appearance->getAlphaAttributes().stop();
    
  if (appearance->colorOn())
    glColor4f(1,1,1,1);
    
  //if (!appearance->depthMaskEnabled())
  //  glEnable(GL_DEPTH_TEST);
  //  //glDepthMask(GL_TRUE);
  
  return geometry->getTriangleCount();
}

void Model::drawRegular(int format)
{
  if (format & VERTICES)
  {
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, geometry->getVertices());
  }
  
  if (format & COLORS)
  {
    glEnableClientState(GL_COLOR_ARRAY);
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, geometry->getColors());
  }
  
  //if (vertexformat & NORMALS)
  //{
  //  glNormalPointer(GL_FLOAT, geometry->getStrideSize(), geometry->getNormals());
  //  glEnableClientState(GL_NORMAL_ARRAY);
  //}
  
  if (geometry->getIndices())
    glDrawElements(GL_TRIANGLES, geometry->getIndicesCount(), GL_UNSIGNED_SHORT, geometry->getIndices());
  else
    glDrawArrays(GL_TRIANGLES, 0, geometry->getIndicesCount());
    
  if (format & VERTICES)
    glDisableClientState(GL_VERTEX_ARRAY);
    
  if (format & COLORS)
    glDisableClientState(GL_COLOR_ARRAY);
    
  //if (format & NORMALS)
  //  glDisableClientState(GL_NORMAL_ARRAY);
}

void Model::setGeometry(Geometry *geom)
{
  GeometryInfo *geomtryInfo;
  
  if (!geom)
    return;
    
  if (geometry)
    if (geomtryInfo = GeometryManager::getGeometryInfo(geometry->getName()))
      geomtryInfo->decreaseUserCount();
      
  if (geomtryInfo = GeometryManager::getGeometryInfo(geom->getName()))
    geomtryInfo->increaseUserCount();
  else
  {
    geomtryInfo = new GeometryInfo(geom->getName(), geom);
    GeometryManager::addGeometryInfo(geomtryInfo);
  }
  
  geometry = geom;
}

Geometry *Model::getGeometry()
{
  return geometry;
}

void Model::setAppearance(Appearance *app)
{
  appearance = app;
}

Appearance *Model::getAppearance()
{
  return appearance;
}

void Model::setParent(TransformGroup *group)
{
  if (!group)
    return;
  parent = group;
}

TransformGroup *Model::getParent()
{
  return parent;
}

void Model::setVisible(bool v)
{
  visible = v;
}

bool Model::isVisible()
{
  return visible;
}

void Model::enableTextureUnit(int format)
{
  Texture *texturePointer;
  bool activeTexture;
  
  if (format & TEXTURE0)
  {
    glTexCoordPointer(geometry->getTextureElementsCount(), GL_FLOAT, 0, geometry->getTextureCoords());
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    
    texturePointer = appearance->getTexture(0);
    activeTexture = texturePointer && texturePointer->getID();
    if (activeTexture)
      texturePointer->activate(0);
  }
}

void Model::disableTexureUnit(int format)
{
  Texture *texturePointer;
  bool activeTexture;
  
  if (format & TEXTURE0)
  {
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    
    texturePointer = appearance->getTexture(0);
    activeTexture = texturePointer && texturePointer->getID();
    if (activeTexture)
      texturePointer->deactivate();
  }
}

void Model::destroy()
{
  if (geometry)
    geometry->destroy();
    
  deleteObject(appearance);
}

Model::~Model()
{
  destroy();
}