#include "Geometry.h"
#include "../Managers/GeometryManager.h"
#include "../Tools/Logger.h"
#include "../Kernel/Gateway.h"

Geometry::Geometry(const char* geoname, int indicesC, int verticesC) : NamedObject(geoname)
{
  verticesCount = verticesC;
  indicesCount  = indicesC;
  initialize();
}

void Geometry::initialize()
{
  memset(byReference, false, 12);
  colors = 0;
  vertices = 0;
  normals = 0;
  indices = 0;
  textureCoords = 0;
  texElementsCount = 0;
  vertexFormat = VERTICES;
  type = GL_TRIANGLES;
}

void Geometry::setColors(Tuple4ub* cols, bool byRef)
{
  if (!cols) return;
  if (!byReference[0]) deleteArray(colors);
  
  byReference[0] = byRef;
  vertexFormat |= COLORS;
  
  if (byRef)
    colors = cols;
  else
  {
    colors = new Tuple4ub[verticesCount];
    memcpy(colors, cols, verticesCount * sizeof(Tuple4ub));
  }
}

Tuple4ub* Geometry::getColors()
{
  return colors;
}

void Geometry::setVertices(Tuple3f* verts, bool byRef)
{
  if (!verts) return;
  if (!byReference[1]) deleteArray(vertices);
  
  byReference[1] = byRef;
  vertexFormat |= VERTICES;
  
  if (byRef)
    vertices = verts;
  else
  {
    vertices = new Tuple3f[verticesCount];
    memcpy(vertices, verts, verticesCount * sizeof(Tuple3f));
  }
}

Tuple3f* Geometry::getVertices()
{
  return vertices;
}

int Geometry::getVerticesCount()
{
  return verticesCount;
}

void Geometry::setNormals(Tuple3f* norms, bool byRef)
{
  if (!norms) return;
  if (!byReference[2]) deleteArray(normals);
  
  byReference[2] = byRef;
  vertexFormat |= NORMALS;
  
  if (byRef)
    normals = norms;
  else
  {
    normals = new Tuple3f[verticesCount];
    memcpy(normals, norms, verticesCount * sizeof(Tuple3f));
  }
}

Tuple3f* Geometry::getNormals()
{
  return normals;
}

void Geometry::setIndices(unsigned short* idx, bool byRef)
{
  if (!idx) return;
  if (!byReference[3]) deleteArray(indices);
  
  byReference[3] = byRef;
  vertexFormat |= INDICES;
  
  if (byRef)
    indices = idx;
  else
  {
    indices = new unsigned short[indicesCount];
    memcpy(indices, idx, indicesCount * sizeof(unsigned short));
  }
}

unsigned short* Geometry::getIndices()
{
  return indices;
}

unsigned short Geometry::getIndicesCount()
{
  return indicesCount;
}

void Geometry::setTextureElements(Tuple2f* coords, int elementSize, bool byRef)
{
  if (!coords) return;
  if (!byReference[4]) deleteArray(textureCoords);
  
  vertexFormat |= TEXTURE0;
  byReference[4] = byRef;
  texElementsCount = elementSize;
  
  if (byRef)
    textureCoords = coords;
  else
  {
    textureCoords = new Tuple2f[verticesCount];
    memcpy(textureCoords, coords, verticesCount * sizeof(Tuple2f));
  }
}

void Geometry::setTextureElementsCount(int elementSize)
{
  texElementsCount = elementSize;
}

int Geometry::getTextureElementsCount()
{
  return texElementsCount;
}

Tuple2f* Geometry::getTextureCoords()
{
  return textureCoords;
}

void Geometry::computeBounds()
{
  Tuple3f *pvertex;
  Tuple3f minEnd(1000,1000,1000), maxEnd(-1000,-1000,-1000);
  
  for (int i = 0; i < verticesCount; i++)
  {
    pvertex = &vertices[i];
    
    minEnd.x = pvertex->x < minEnd.x ? pvertex->x : minEnd.x;
    maxEnd.x = pvertex->x > maxEnd.x ? pvertex->x : maxEnd.x;
    
    minEnd.y = pvertex->y < minEnd.y ? pvertex->y : minEnd.y;
    maxEnd.y = pvertex->y > maxEnd.y ? pvertex->y : maxEnd.y;
    
    minEnd.z = pvertex->z < minEnd.z ? pvertex->z : minEnd.z;
    maxEnd.z = pvertex->z > maxEnd.z ? pvertex->z : maxEnd.z;
  }
  
  boundsDescriptor.computeBounds(minEnd, maxEnd);
}

BoundsDescriptor &Geometry::getBoundsDescriptor()
{
  return boundsDescriptor;
}

int Geometry::getVertexFormat()
{
  return vertexFormat;
}

int Geometry::getTriangleCount()
{
  return (type == GL_TRIANGLE_STRIP) ? indicesCount - 2:
         (type == GL_TRIANGLE_FAN) ? indicesCount - 2:
         (type == GL_TRIANGLES) ? indicesCount/3  :
         (type == GL_QUADS) ? indicesCount/2 : 0;
}

void Geometry::destroy()
{
  GeometryInfo *geomtryInfo = GeometryManager::getGeometryInfo(name);
  
  if (geomtryInfo)
  {
    geomtryInfo->decreaseUserCount();
    if (geomtryInfo->getUserCount() > 0)
      return;
  }
  
  /*if (byReference[0])
    Gateway::releaseGeometryMemory(colors);
  else
    deleteArray(colors);
  
  if (byReference[1])
    Gateway::releaseGeometryMemory(colors);
  else
    deleteArray(vertices);
  
  if (byReference[2])
    Gateway::releaseGeometryMemory(normals);
  else
    deleteArray(normals);
  
  if (byReference[3])
    Gateway::releaseGeometryMemory(indices);
  else
    deleteArray(indices);
  
  if (byReference[4])
    Gateway::releaseGeometryMemory(textureCoords);
  else
    deleteArray(textureCoords);*/
  
  if (!byReference[0])
    deleteArray(colors);
    
  if (!byReference[1])
    deleteArray(vertices);
    
  if (!byReference[2])
    deleteArray(normals);
    
  if (!byReference[3])
    deleteArray(indices);
    
  if (!byReference[4])
    deleteArray(textureCoords);
    
  GeometryManager::flushUnusedGeometry();
}

Geometry::~Geometry()
{
  destroy();
}