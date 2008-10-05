#include "FactoryPBA.h"

#include "../FileSystem/ArchivedFile.h"
#include "../FileSystem/FileSystem.h"

#include "../Geometry/Model.h"
#include "../Geometry/Geometry.h"
#include "../Appearance/Appearance.h"
#include "../Nodes/TransformGroup.h"

#include "../Managers/GeometryManager.h"

#include "../Image/ImageExt.h"

#include "../Math/Quaternion.h"

#include "../Kernel/Gateway.h"

#include "../Tools/ICString.h"

struct ModelJoint
{
  char name[32];
  int parentID;
  Quaternionf rotation;
  Tuple3f translation;
};

bool loadPBA(TransformGroup* parent, const char* filename)
{
  ArchivedFile *file;
  
  unsigned short id;
  unsigned int length;
  unsigned int fcc;
  
  char pbaname[32];
  char meshname[32];
  //char texcivname[32];
  char texturename[32];
  char stringBuffer[256];
  
  unsigned int vertscount;
  unsigned int fgvertscount;
  unsigned int indicescount;
  unsigned int texturecount;
  unsigned int texcivcount;
  unsigned int modelcount;
  unsigned int shapetype;
  unsigned int meshcount;
  unsigned int jointcount;
  unsigned int vertexoffset;
  
  int alphablendingflags;
  int textureID;
  
  ArrayPtr <Model> models;
  ArrayPtr <unsigned short> indexBuffers;
  Array <int> indexBufferSize;
  Array <String> texturenames;
  ArrayPtr <ModelJoint> joints;
  
  float meshradius;
  Tuple3f meshcenter;
  
  Tuple2f *txcoords;
  Tuple3f *normals;
  Tuple3f *points;
  Tuple4ub *colors;
  unsigned short *indices;
  
  Appearance *appearance;
  Geometry *geometry;
  Model *model;
  
  ModelJoint* joint;
  Transform3D transform;
  Quaternionf objquat;
  Tuple3f objtrans;
  
  GeometryInfo *geometryinfo = 0;
  
  bool newgeometry = false;
  bool success = false;
  bool byref = false;
  
  if (!(file = FileSystem::checkOut(String(filename) + ".pba")))
    return Logger::writeErrorLog(String("Could not check out archived item -> ") + filename + ".pba");
    
  ///header
  file->read(&fcc, 4);
  file->read(&id, 2);
  file->read(&length, 4);
  file->read(pbaname, 32);
  
  ///bones
  file->read(&id, 2);
  file->read(&length, 4);
  file->read(&jointcount, 4);
  for (unsigned int i = 0; i < jointcount; i++)
  {
    joint = new ModelJoint;
    file->read(&joint->name, 32);
    file->read(&joint->parentID, 4);
    file->read(&joint->rotation.w, 4);
    file->read(&joint->rotation.x, 4);
    file->read(&joint->rotation.y, 4);
    file->read(&joint->rotation.z, 4);
    file->read(&joint->translation, 12);
    joints.append(joint);
  }
  
  ///textures
  file->read(&texturecount, 4);
  for (unsigned int i = 0; i < texturecount; i++)
  {
    file->read(texturename, 32);
    texturenames.append(texturename);
  }
  
  ///meshes
  file->read(&id, 2);
  file->read(&length, 4);
  file->read(&meshcount, 4);
  for (unsigned int m = 0; m < 1; m++)
  {
    file->read(&id, 2);
    file->read(&length, 4);
    file->read(&shapetype, 4);
    switch (shapetype)
    {
      case MODEL_RIGIDA:
      {
        file->read(meshname, 32);
        file->read(&meshcenter, 12);
        file->read(&meshradius, 4);
        file->read(&modelcount, 4);
        
        ///facegroup arrays
        for (unsigned int sh = 0; sh < modelcount; sh++)
        {
          appearance = new Appearance();
          model = new Model();
          
          file->read(&textureID, 4);
          file->read(&alphablendingflags, 4);
          file->read(&indicescount, 4);
          
          textureID = textureID == -1 ? 0: textureID;
          
          if (!byref)
            indices = new unsigned short[indicescount];
          else
            indices = (unsigned short*) Gateway::aquireGeometryMemory(indicescount * 2);
            
          file->read(indices, indicescount * 2);
          
          if (1 < fcc)
          {
            file->read(&texcivcount, 4);
            file->seek(texcivcount * 32, SEEKD);
          }
          
          indexBuffers.append(indices);
          indexBufferSize.append(indicescount);
          
          setAlphaBlendingState(appearance, alphablendingflags);
          Texture texture;
          ImageExt image;
          image.load((String(texturenames(textureID)) + ".ptx"));
          texture.load2DImage(image);
          
          appearance->setTexture(0, texture);
          appearance->setColor(1,1,1,1);
          appearance->enableColor(true);
          
          model->setAppearance(appearance);
          
          ///hide the shadow model
          if (alphablendingflags == MATERIAL_ALPHASOMBRA)
            model->setVisible(false);
            
          models.append(model);
        }
        
        ///vertex arrays
        for (unsigned int sh = 0; sh < modelcount; sh++)
        {
          file->read(&vertscount, 4);
          
          if (!byref)
          {
            colors   = new Tuple4ub[vertscount];
            points   = new Tuple3f[vertscount];
            normals  = new Tuple3f[vertscount];
            txcoords = new Tuple2f[vertscount];
          }
          else
          {
            colors   = (Tuple4ub*) Gateway::aquireGeometryMemory(vertscount * sizeof(Tuple4ub));
            points   = (Tuple3f*) Gateway::aquireGeometryMemory(vertscount * sizeof(Tuple3f));
            normals  = (Tuple3f*) Gateway::aquireGeometryMemory(vertscount * sizeof(Tuple3f));
            txcoords = (Tuple2f*) Gateway::aquireGeometryMemory(vertscount * sizeof(Tuple2f));
          }
          
          ///transform model's vertices rather than waste CPU doing run-time matrix math
          ///according to its bone orientation.
          for (unsigned int j = 0; j < joints.length(); j++)
            if (String(joints(j)->name) == meshname)
            {
              objquat = joints(j)->rotation;
              objtrans = joints(j)->translation;
              break;
            }
            
          transform.setTranslations(objtrans);
          transform.rotateXYZ(objquat);
          
          for (unsigned int v = 0; v < vertscount; v++)
          {
            file->read(&points[v],12);
            file->read(&normals[v],12);///these aren't used anyway, bitch
            file->read(&colors[v].z,1);
            file->read(&colors[v].y,1);
            file->read(&colors[v].x,1);
            file->read(&colors[v].w,1);
            file->read(&txcoords[v],8);
            points[v] *= transform.getMatrix4f();
          }
          
          memset(stringBuffer, 0, 256);
          sprintf_s(stringBuffer, 256, "%s_%d", pbaname, sh);
          
          geometryinfo = GeometryManager::getGeometryInfo(stringBuffer);
          if (!geometryinfo)
          {
            geometry = new Geometry(stringBuffer, indexBufferSize(sh), vertscount);
            geometry->setColors(colors, byref);
            geometry->setVertices(points, byref);
            geometry->setNormals(normals, byref);
            geometry->setTextureElements(txcoords, 2, byref);
            geometry->setIndices(indexBuffers(sh), byref);
            
            ///lets skip the MATERIAL_ALPHASOMBRA.
            ///we also want to center the model by transforming its vertices one more time
            ///according to its bounding box properties.
            if (models(sh)->getAppearance()->getType() != MATERIAL_ALPHASOMBRA)
            {
              geometry->computeBounds();
              Tuple3f cen = geometry->getBoundsDescriptor().getCenterAABB();
              float ymin = geometry->getBoundsDescriptor().getMinEndAABB().y;
              transform.setIdentity();
              transform.setTranslations(-cen.x, -ymin, -cen.y);
              for (unsigned int v = 0; v < vertscount; v++)
                points[v] *= transform.getMatrix4f();
              geometry->computeBounds();
            }
            
            geometryinfo = new GeometryInfo(stringBuffer, geometry);
            GeometryManager::addGeometryInfo(geometryinfo);
            geometryinfo->decreaseUserCount();
            
            newgeometry = true;
          }
          
          if (!byref)
          {
            deleteArray(colors);
            deleteArray(points);
            deleteArray(normals);
            deleteArray(txcoords);
            deleteArray(indexBuffers(sh));
          }
          
          models(sh)->setGeometry(geometryinfo->getMedia());
        }
        
        for (unsigned int sh = 0; sh < modelcount; sh++)
          parent->addChild(models(sh));
          
        success = true;
      }
      break;
      
      case MODEL_ANIMADA:
      {
        file->read(meshname, 32);
        file->read(&meshcenter, 12);
        file->read(&meshradius, 4);
        
        ///vertex arrays
        file->read(&vertscount, 4);
        
        if (!byref)
        {
          colors   = new Tuple4ub[vertscount];
          points   = new Tuple3f[vertscount];
          normals  = new Tuple3f[vertscount];
          txcoords = new Tuple2f[vertscount];
        }
        else
        {
          colors   = (Tuple4ub*) Gateway::aquireGeometryMemory(vertscount * sizeof(Tuple4ub));
          points   = (Tuple3f*) Gateway::aquireGeometryMemory(vertscount * sizeof(Tuple3f));
          normals  = (Tuple3f*) Gateway::aquireGeometryMemory(vertscount * sizeof(Tuple3f));
          txcoords = (Tuple2f*) Gateway::aquireGeometryMemory(vertscount * sizeof(Tuple2f));
        }
        
        for (unsigned int v = 0; v < vertscount; v++)
        {
          file->read(&points[v],12);
          file->read(&normals[v],12);
          file->read(&colors[v].z,1);
          file->read(&colors[v].y,1);
          file->read(&colors[v].x,1);
          file->read(&colors[v].w,1);
          file->read(&txcoords[v],8);
        }
        
        ///facegroup arrays
        vertexoffset = 0;
        
        file->read(&modelcount, 4);
        
        for (unsigned int sh = 0; sh < modelcount; sh++)
        {
          appearance = new Appearance();
          model = new Model();
          
          file->read(&textureID, 4);
          file->read(&alphablendingflags, 4);
          file->read(&fgvertscount, 4);
          file->read(&indicescount, 4);
          
          textureID = textureID == -1 ? 0: textureID;
          
          if (!byref)
            indices = new unsigned short[indicescount];
          else
            indices = (unsigned short*) Gateway::aquireGeometryMemory(indicescount * 2);
            
          file->read(indices, indicescount * 2);
          
          if (1 < fcc)
          {
            file->read(&texcivcount, 4);
            file->seek(texcivcount * 32, SEEKD);
          }
          
          setAlphaBlendingState(appearance, alphablendingflags);
          
          Texture texture;
          ImageExt image;
          image.load((String(texturenames(textureID)) + ".ptx"));
          texture.load2DImage(image);
          
          appearance->setTexture(0, texture);
          appearance->enableColor(false);
          
          model->setAppearance(appearance);
          
          ///hide the shadow model
          if (alphablendingflags == MATERIAL_ALPHASOMBRA)
            model->setVisible(false);
            
          memset(stringBuffer, 0, 256);
          sprintf_s(stringBuffer, 256, "%s_%d", pbaname, sh);
          
          geometryinfo = GeometryManager::getGeometryInfo(stringBuffer);
          if (!geometryinfo)
          {
            geometry = new Geometry(stringBuffer, indicescount, fgvertscount);
            geometry->setColors(colors + vertexoffset, byref);
            geometry->setVertices(points + vertexoffset, byref);
            geometry->setNormals(normals + vertexoffset, byref);
            geometry->setTextureElements(txcoords + vertexoffset, 2, byref);
            geometry->setIndices(indices, byref);
            
            ///lets skip the MATERIAL_ALPHASOMBRA
            if (appearance->getType() != MATERIAL_ALPHASOMBRA)
              geometry->computeBounds();
              
            geometryinfo = new GeometryInfo(stringBuffer, geometry);
            GeometryManager::addGeometryInfo(geometryinfo);
            geometryinfo->decreaseUserCount();
            
            newgeometry = true;
          }
          
          model->setGeometry(geometryinfo->getMedia());
          
          parent->addChild(model);
          
          ///center to ground
          //parent->updateBoundsDescriptor();
          //Tuple3f cenparent = parent->getBoundsDescriptor().getCenterAABB();
          //float yminparent = parent->getBoundsDescriptor().getMinEndAABB().y;
          //parent->getTransform().setTranslations(-cenparent.x, -yminparent, -cenparent.y);
          //parent->updateBoundsDescriptor();
          
          vertexoffset += fgvertscount;
        }
        
        if (!byref)
        {
          deleteArray(colors);
          deleteArray(points);
          deleteArray(normals);
          deleteArray(txcoords);
          deleteArray(indices);
        }
        
        success = true;
      }
      break;
      
      default:
        Logger::writeWarningLog(String("Unsupported mesh type -> ") + filename + ".pba");
        break;
    }
  }
  
  joints.clearAndDestroy();
  
  FileSystem::checkIn(file);
  
  if (newgeometry)
    Logger::writeInfoLog(String("Loaded PBA model -> ") + filename + ".pba");
    
  return success;
}

void setAlphaBlendingState(Appearance* appearance, int state)
{
  switch (state)
  {
    case MATERIAL_ALPHA:
    {
      appearance->setAlphaAttributes(AlphaAttributes(GL_GREATER, 0.2f));
      appearance->setBlendAttributes(BlendAttributes(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
      appearance->setType(MATERIAL_ALPHA);
    }
    break;
    
    case MATERIAL_ALPHATEST:
    {
      appearance->setAlphaAttributes(AlphaAttributes(GL_GREATER, 0.2f));
      appearance->setType(MATERIAL_ALPHATEST);
    }
    break;
    
    case MATERIAL_ALPHASOMBRA:
    {
      appearance->setBlendAttributes(BlendAttributes(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
      appearance->setType(MATERIAL_ALPHASOMBRA);
      //appearance->enableDepthMask(false);
    }
    break;
    
    default:
      appearance->setType(MATERIAL_NONE);
      break;
  }
}