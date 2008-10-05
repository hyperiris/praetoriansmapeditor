#include "TexturedGrid.h"
#include "../Image/ImageExt.h"

TexturedGrid::TexturedGrid()
{
  gridSize = 8;
  yOffset = 0.0f;
  
  setupVertexField();
  setupColorField();
  setupTexCoordField();
  setupIndexField();
}

void TexturedGrid::loadTexture(const char *filename)
{
  ImageExt image;
  image.load(filename);
  texture.load2DImage(image);
}

void TexturedGrid::draw()
{
  texture.activate();
  
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  
  glTexCoordPointer(2, GL_FLOAT, 0, txCoordsField.data());
  glVertexPointer(3, GL_FLOAT, 0, vertexField.data());
  glColorPointer(3, GL_UNSIGNED_BYTE, 0, colorField.data());
  glDrawElements(GL_TRIANGLE_STRIP, indexField.length(), GL_UNSIGNED_INT, indexField.data());
  
  glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  
  texture.deactivate();
}

void TexturedGrid::setupIndexField()
{
  unsigned int idxt;
  unsigned int idxb;
  unsigned int width = gridSize + 1;
  
  for (unsigned int y = 0; y < gridSize; y++)
  {
    for (unsigned int x = 0; x < width; x++)
    {
      idxt = y * width + x;
      idxb = (y + 1) * width + x;
      indexField.append(idxt);
      indexField.append(idxb);
    }
    
    //degenerate triangle
    idxt = idxb;
    idxb = (y + 1) * width;
    indexField.append(idxt);
    indexField.append(idxb);
  }
}

void TexturedGrid::setupVertexField()
{
  Matrix4f matrix;
  matrix.setTranslations(-(float)gridSize/2, 0, -(float)gridSize/2);
  
  for (float y = 0; y < gridSize + 1; y++)
    for (float x = 0; x < gridSize + 1; x++)
      vertexField.append(matrix * Tuple3f(x, yOffset, y));
}

void TexturedGrid::setupColorField()
{
  for (unsigned int y = 0; y < gridSize + 1; y++)
    for (unsigned int x = 0; x < gridSize + 1; x++)
      colorField.append(Tuple3ub());
}

void TexturedGrid::setupTexCoordField()
{
  for (unsigned int i = 0; i < vertexField.length(); i++)
    txCoordsField.append(Tuple2f(vertexField(i).x, vertexField(i).z));
}

void TexturedGrid::applyGaussianDistribution(float strength, float radius)
{
  float d;
  Tuple3f center(0, 0, 0);
  for (unsigned int i = 0; i < vertexField.length(); i++)
  {
    d = center.getDistance(vertexField(i));
    colorField(i).set((char) round(255*strength*exp(-(d*d) / radius)));
  }
}

void TexturedGrid::setOffsetY(float value)
{
  yOffset = value;
}

void TexturedGrid::setup(unsigned int size)
{
  gridSize = size;
  
  vertexField.clear();
  txCoordsField.clear();
  colorField.clear();
  indexField.clear();
  
  setupVertexField();
  setupColorField();
  setupTexCoordField();
  setupIndexField();
}

TexturedGrid::~TexturedGrid()
{
}
