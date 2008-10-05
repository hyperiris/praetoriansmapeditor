#include "Grid.h"

#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>

Grid::Grid()
{
  gridSize  = 8.0f;
  stepSize  = 1.0f;
  majorSize = 4.0f;
  yOffset   = 0.0f;
}

void Grid::draw(void)
{
  drawMinorGridLines();
  if (majorSize > 0.0f && majorSize <= gridSize)
    drawMajorGridLines();
}

void Grid::drawMinorGridLines()
{
  float i;
  glBegin(GL_LINES);
  glColor3ub(92, 80, 56);
  for (i = 0; i <= gridSize; i += stepSize)
  {
    glVertex3f(i, yOffset-0.001f, 0);
    glVertex3f(i, yOffset-0.001f, gridSize);
  }
  for (i = 0; i <= gridSize; i += stepSize)
  {
    glVertex3f(gridSize, yOffset-0.001f, i);
    glVertex3f(0, yOffset-0.001f, i);
  }
  glEnd();
}

void Grid::drawMajorGridLines()
{
  float i;
  glBegin(GL_LINES);
  glColor3ub(255, 162, 0);
  for (i = 0; i <= gridSize; i += stepSize * majorSize)
  {
    glVertex3f(i, yOffset, 0);
    glVertex3f(i, yOffset, gridSize);
  }
  for (i = 0; i <= gridSize; i += stepSize * majorSize)
  {
    glVertex3f(gridSize, yOffset, i);
    glVertex3f(0, yOffset, i);
  }
  glEnd();
}

void Grid::setOffsetY(float value)
{
  yOffset = value;
}

void Grid::setup(float step, float size, float major)
{
  gridSize  = size;
  stepSize  = step;
  majorSize = major;
}

Grid::~Grid()
{
}