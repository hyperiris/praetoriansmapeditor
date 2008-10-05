#include "TileGraphRenderer.h"
#include "../Nodes/TextureNode.h"
#include "../Nodes/IndicesNode.h"
#include "../Nodes/TileGraphNode.h"
#include "../Nodes/TextureCoordsNode.h"
#include "../Tools/NodeIterator.h"
#include "../Controllers/TileController.h"
#include "../Factories/FactoryUtils.h"
#include "../Tools/Logger.h"

TileGraphRenderer::TileGraphRenderer()
{
  reset();
}

void TileGraphRenderer::visit(Node* node)
{
  if (wireMode)
  {
    glPushAttrib(GL_POLYGON_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
  
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  
  NodeIterator iter(node->getFirstChild());
  
  if (blend)
  {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    while (!iter.end())
    {
      iter.current()->accept(this);
      iter++;
    }
    
    glDisable(GL_BLEND);
  }
  else
    while (!iter.end())
    {
      iter.current()->accept(this);
      iter++;
    }
    
  glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  
  if (wireMode)
    glPopAttrib();
}

void TileGraphRenderer::visit(IndicesNode* node)
{
  NodeIterator iter(node->getFirstChild());
  
  pIndices = node->getIndices();
  
  while (!iter.end())
  {
    iter.current()->accept(this);
    iter++;
  }
}

void TileGraphRenderer::visit(TextureNode* node)
{
  NodeIterator iter(node->getFirstChild());
  
  if (texture)
  {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, node->getID());
    
    while (!iter.end())
    {
      iter.current()->accept(this);
      iter++;
    }
    
    glDisable(GL_TEXTURE_2D);
    
  }
  else
    while (!iter.end())
    {
      iter.current()->accept(this);
      iter++;
    }
}

void TileGraphRenderer::visit(TextureCoordsNode* node)
{
  NodeIterator iter(node->getFirstChild());
  
  pCoords = node->getCoords();
  glTexCoordPointer(2, GL_FLOAT, 0, pCoords);
  
  while (!iter.end())
  {
    iter.current()->accept(this);
    iter++;
  }
}

void TileGraphRenderer::visit(TileGraphNode* node)
{
  TileController *controller = node->getController();
  
  if (texture)
  {
    transformTMatrix(controller->getFlags());
    
    glVertexPointer(3, GL_FLOAT, 0, controller->getVertices());
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, controller->getColors());
    glDrawElements(GL_TRIANGLE_FAN, 10, GL_UNSIGNED_INT, pIndices);
    
    unTransformTMatrix();
  }
  else
  {
    glVertexPointer(3, GL_FLOAT, 0, controller->getVertices());
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, controller->getColors());
    glDrawElements(GL_TRIANGLE_FAN, 10, GL_UNSIGNED_INT, pIndices);
  }
}

void TileGraphRenderer::enableBlend(bool enable)
{
  blend = enable;
}

void TileGraphRenderer::enableTexture(bool enable)
{
  texture = enable;
}

void TileGraphRenderer::transformTMatrix(unsigned short flags)
{
  glMatrixMode(GL_TEXTURE);
  glPushMatrix();
  glLoadIdentity();
  
  if (flags & TileFlags::TEXTURE_MIRRORX)
  {
    glTranslatef(pCoords[4].x, pCoords[4].y, 0);
    glScalef(-1, 1, 1);
    glTranslatef(-pCoords[4].x, -pCoords[4].y, 0);
  }
  
  if (flags & TileFlags::TEXTURE_MIRRORY)
  {
    glTranslatef(pCoords[4].x, pCoords[4].y, 0);
    glScalef(1, -1, 1);
    glTranslatef(-pCoords[4].x, -pCoords[4].y, 0);
  }
  
  if (flags & TileFlags::TEXTURE_ROTATE90)
  {
    glTranslatef(pCoords[4].x, pCoords[4].y, 0);
    glRotatef(90.0f, 0, 0, 1);
    glTranslatef(-pCoords[4].x, -pCoords[4].y, 0);
  }
  else if (flags & TileFlags::TEXTURE_ROTATE180)
  {
    glTranslatef(pCoords[4].x, pCoords[4].y, 0);
    glRotatef(180.0f, 0, 0, 1);
    glTranslatef(-pCoords[4].x, -pCoords[4].y, 0);
  }
  else if (flags & TileFlags::TEXTURE_ROTATE270)
  {
    glTranslatef(pCoords[4].x, pCoords[4].y, 0);
    glRotatef(270.0f, 0, 0, 1);
    glTranslatef(-pCoords[4].x, -pCoords[4].y, 0);
  }
  
  glMatrixMode(GL_MODELVIEW);
}

void TileGraphRenderer::unTransformTMatrix()
{
  glMatrixMode(GL_TEXTURE);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
}

void TileGraphRenderer::enableWireMode(bool enable)
{
  wireMode = enable;
}

void TileGraphRenderer::reset()
{
  blend = false;
  texture = true;
  wireMode = false;
}

TileGraphRenderer::~TileGraphRenderer()
{}