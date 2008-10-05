#include "../Nodes/TransformGroup.h"
#include "Renderer.h"

ShaderAttributes Renderer::shadowVolumeExtrusion;

Matrix4f Renderer::projectionMatrix;
Matrix4f Renderer::modelviewMatrix;
Matrix4f Renderer::spareMatrix;

Tuple4f  Renderer::corners[8];
Tuple4i  Renderer::viewport;

float    Renderer::fieldOfView           = 90.0f;
bool     Renderer::shadowVolumesVPLoaded = false;
bool     Renderer::forceCPUExtrusion     = false;
int      Renderer::availableTextureUnits =     2;
int      Renderer::maxAnisotropicFilter  =     0;

void    Renderer::setCurrentFieldOfView(float fov)
{
  fieldOfView = fov;
}
float   Renderer::getCurrentFieldOfView()
{
  return fieldOfView;
}

void Renderer::enter2DMode(GLint winWidth, GLint winHeight)
{
  Tuple4i  viewport;
  
  if (winWidth <= 0 || winHeight <= 0)
  {
    glGetIntegerv(GL_VIEWPORT, viewport);
    winWidth  = viewport.z;
    winHeight = viewport.w;
  }
  
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(0, winWidth, winHeight, 0);
  glDisable(GL_DEPTH_TEST);
}

void Renderer::enter2DModeInverse(GLint winWidth, GLint winHeight)
{
  Tuple4i  viewport;
  
  if (winWidth <= 0 || winHeight <= 0)
  {
    glGetIntegerv(GL_VIEWPORT, viewport);
    winWidth  = viewport.z;
    winHeight = viewport.w;
  }
  
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(0, winWidth, 0, winHeight);
  glDisable(GL_DEPTH_TEST);
}

void Renderer::exit2DMode()
{
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  glEnable(GL_DEPTH_TEST);
}

void Renderer::forceCPUVolumeExtrusion(bool or)
{
  forceCPUExtrusion = or;
}

bool Renderer::CPUVolumeExtrusionForced()
{
  return forceCPUExtrusion;
}

void Renderer::enableGPUVolumeExtrusion()
{
  if (GLEE_ARB_vertex_program)
  {
    if (!shadowVolumesVPLoaded)
    {
      char  *volumeExtrusionCode = {"!!ARBvp1.0\n"
                                    "PARAM mvp[4]     = { state.matrix.mvp };\n"
                                    "PARAM lightPosition =   program.env[0];\n"
                                    "PARAM zero = {0.0, 0.0, 0.0, 0.0};\n"
                                    "PARAM one  = {1.0, 1.0, 1.0, 1.0};\n"
                                    
                                    "TEMP lightVector, dotProduct, newPosition;\n"
                                    "TEMP toMult, toSubtract;\n"
                                    
                                    "SUB lightVector, lightPosition, vertex.position;\n"
                                    "DP3 dotProduct,  lightVector,   vertex.normal;\n"
                                    
                                    "MOV toSubtract, zero;\n"
                                    "SLT toSubtract, dotProduct, zero;\n"
                                    
                                    "MOV toMult, one;\n"
                                    "SGE toMult.w, dotProduct, zero;\n"
                                    
                                    "MAD newPosition, -toSubtract, lightPosition, vertex.position;\n"
                                    "MUL newPosition, newPosition, toMult;\n"
                                    
                                    "DP4 result.position.x, mvp[0], newPosition;\n"
                                    "DP4 result.position.y, mvp[1], newPosition;\n"
                                    "DP4 result.position.z, mvp[2], newPosition;\n"
                                    "DP4 result.position.w, mvp[3], newPosition;\n"
                                    
                                    "END\n"
                                   };
      Shaders::loadShaderFromMemory(&shadowVolumeExtrusion,
                                    volumeExtrusionCode,
                                    VERTEX_PROGRAM);
      shadowVolumesVPLoaded = true;
    }
    else
      shadowVolumeExtrusion.activate();
  }
}

void Renderer::disableGPUVolumeExtrusion()
{
  shadowVolumeExtrusion.deactivate();
}

Tuple3f Renderer::getScreenCoords(const Tuple3f &worldPosition)
{
  Tuple4f temp;
  
  glGetIntegerv(GL_VIEWPORT,viewport);
  glGetFloatv(GL_PROJECTION_MATRIX, projectionMatrix);
  glGetFloatv(GL_MODELVIEW_MATRIX,  modelviewMatrix);
  
  temp.set(worldPosition, 1.0f);
  temp *= modelviewMatrix;
  temp *= projectionMatrix;
  
  temp /= temp.w;
  
  temp.x = viewport[0] +  float(viewport[2])*(temp.x + 1.0f)/2.0f;
  temp.y = viewport[1] +  float(viewport[3])*(temp.y + 1.0f)/2.0f;
  temp.z = 0.5f*temp.z + 0.5f;
  return Tuple3f(temp.x, temp.y, temp.z);
}

Tuple3f Renderer::getWorldCoords(const Tuple3f &screenCoords)
{
  Tuple4f temp(screenCoords, 1.0f);
  
  glGetIntegerv(GL_VIEWPORT,viewport);
  glGetFloatv(GL_PROJECTION_MATRIX, projectionMatrix);
  glGetFloatv(GL_MODELVIEW_MATRIX,  modelviewMatrix);
  
  spareMatrix  = projectionMatrix;
  spareMatrix *= modelviewMatrix;
  spareMatrix.setInverse();
  
  temp.x = (screenCoords.x - viewport[0])*2.0f/ viewport[2] - 1.0f;
  temp.y = (screenCoords.y - viewport[1])*2.0f/ viewport[3] - 1.0f;
  temp.z =  screenCoords.z                *2.0f               - 1.0f;
  
  temp  *= spareMatrix;
  //Check for division by zero and return an all zero tuple or the correct world
  //coordinates accordingly
  return !temp.w ? Tuple3f() : Tuple3f(temp.x/temp.w, temp.y/temp.w, temp.z/temp.w);
}

Tuple4i Renderer::getScissorRectAndDepthBounds(const Tuple4f &lightInfo, Tuple2f &zMinZmax)
{
  Tuple3f light(lightInfo.x, lightInfo.y, lightInfo.z);
  Tuple4i scissor;
  float   radius = lightInfo.w;
  
  glGetIntegerv(GL_VIEWPORT,viewport);
  zMinZmax.set(0.0, 1.0f);
  
  if (light.getDistance(getCameraPosition()) - radius <= EPSILON)
  {
    return viewport;
  }
  
  radius*=1.4142f;
  
  glGetFloatv(GL_PROJECTION_MATRIX, projectionMatrix);
  glGetFloatv(GL_MODELVIEW_MATRIX, modelviewMatrix);
  
  Tuple4f zMinVec,
  zMaxVec;
  Tuple3f vec1,
  vec2;
  
  corners[0].set(light.x + (-modelviewMatrix[0] - modelviewMatrix[1])*radius,
                 light.y + (-modelviewMatrix[4] - modelviewMatrix[5])*radius,
                 light.z + (-modelviewMatrix[8] - modelviewMatrix[9])*radius, 1.0f);
                 
  corners[1].set(light.x + (modelviewMatrix[0] - modelviewMatrix[1])*radius,
                 light.y + (modelviewMatrix[4] - modelviewMatrix[5])*radius,
                 light.z + (modelviewMatrix[8] - modelviewMatrix[9])*radius, 1.0f);
                 
  corners[2].set(light.x + (-modelviewMatrix[0] + modelviewMatrix[1])*radius,
                 light.y + (-modelviewMatrix[4] + modelviewMatrix[5])*radius,
                 light.z + (-modelviewMatrix[8] + modelviewMatrix[9])*radius, 1.0f);
                 
  corners[3].set(light.x + (modelviewMatrix[0] + modelviewMatrix[1])*radius,
                 light.y + (modelviewMatrix[4] + modelviewMatrix[5])*radius,
                 light.z + (modelviewMatrix[8] + modelviewMatrix[9])*radius, 1.0f);
                 
  vec1.set(corners[1].x - corners[0].x,
           corners[1].y - corners[0].y,
           corners[1].z - corners[0].z);
  vec2.set(corners[2].x - corners[0].x,
           corners[2].y - corners[0].y,
           corners[2].z - corners[0].z);
           
  vec1.crossProduct(vec1, vec2).normalize();
  
  radius/=1.41f;
  
  vec2.set(light.x - vec1.x*radius,
           light.y - vec1.y*radius,
           light.z - vec1.z*radius);
           
  vec1.set(light.x + vec1.x*radius,
           light.y + vec1.y*radius,
           light.z + vec1.z*radius);
           
  corners[1].set(vec1, 1.0f);
  corners[2].set(vec2, 1.0f);
  
  for (int i = 0; i < 4; i++)
  {
    corners[i] *= modelviewMatrix;
    corners[i] *= projectionMatrix;
    corners[i] /= corners[i].w;
    corners[i].x = viewport[0] +  float(viewport[2])*(corners[i].x + 1.0f)/2.0f;
    corners[i].y = viewport[1] +  float(viewport[3])*(corners[i].y + 1.0f)/2.0f;
    corners[i].z = 0.5f*corners[i].z + 0.5f;
  }
  
  scissor[0] = int(corners[0].x);
  scissor[1] = int(corners[0].y);
  scissor[2] = int(corners[3].x);
  scissor[3] = int(corners[3].y);
  zMinZmax.x = corners[1].z;
  zMinZmax.y = corners[2].z;
  
  zMinZmax.clamp(0.0f, 1.0f);
  
  if (zMinZmax.x > zMinZmax.y)
    zMinZmax.set(0.0f, 1.0f);
    
  return scissor;
}

Tuple4i Renderer::getScissorRect(const Tuple3f &lightPosition,
                                 float lightRange)
{
  Tuple4i scissor;
  
  //Retrieve the current view port to extract the screen width and height
  glGetIntegerv(GL_VIEWPORT,viewport);
  
  //Get the camera world position
  Tuple3f cameraPosition = getCameraPosition(),
                           lightPos       = lightPosition,
                                            diagonal;
                                            
  //If the camera distance to the light source is lesser than
  //the light range, return the description of the whole screen
  if (cameraPosition.getDistance(lightPos) - lightRange <= EPSILON)
    return viewport;
    
  //Retrieve the projection matrix, we will use it later to project
  //our corners points onto the screen
  glGetFloatv(GL_PROJECTION_MATRIX, projectionMatrix);
  
  /**************************************/
  /*                    + Upper corner  */
  /*                   /                */
  /*                  /                 */
  /*                 /                  */
  /*                /                   */
  /*               /                    */
  /*              /                     */
  /*            ( ) Light position      */
  /*            /                       */
  /*           /                        */
  /*          /                         */
  /*         /                          */
  /*        /                           */
  /*       /                            */
  /*      + Lower corner                */
  /**************************************/
  
  //Multiply the light range by square root of two since we will compute
  //the corners of square
  lightRange *= 1.42f;
  
  diagonal.set(modelviewMatrix[0] + modelviewMatrix[1],
               modelviewMatrix[4] + modelviewMatrix[5],
               modelviewMatrix[8] + modelviewMatrix[9]);
  diagonal   *= lightRange;
  
  //Compute the lower corner
  corners[0].set(lightPosition.x - diagonal.x,
                 lightPosition.y - diagonal.y,
                 lightPosition.z - diagonal.z,
                 1.0f);
                 
  //Compute the upper corner
  corners[1].set(lightPosition.x + diagonal.x,
                 lightPosition.y + diagonal.y,
                 lightPosition.z + diagonal.z,
                 1.0f);
                 
  //Project both onto the screen surface
  for (int i = 0; i < 2; i++)
  {
    corners[i] *= modelviewMatrix;
    corners[i] *= projectionMatrix;
    corners[i] /= corners[i].w;
    corners[i].x = viewport[0] +  float(viewport[2])*(corners[i].x + 1.0f)/2.0f;
    corners[i].y = viewport[1] +  float(viewport[3])*(corners[i].y + 1.0f)/2.0f;
    corners[i].z = 0.5f*corners[i].z + 0.5f;
  }
  
  //Set up the scissor info
  scissor[0] = int(corners[0].x);
  scissor[1] = int(corners[0].y);
  scissor[2] = int(corners[1].x);
  scissor[3] = int(corners[1].y);
  
  scissor[0] = scissor[0] < 0 ? 0 : scissor[0];
  scissor[1] = scissor[1] < 0 ? 0 : scissor[1];
  
  return scissor;
}

int  Renderer::getMaxAvailableTextureUnits()
{
  static bool initialized = false;
  if (!initialized)
  {
    glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &availableTextureUnits);
    initialized = true;
  }
  return availableTextureUnits;
}

int Renderer::getMaxAnisotropicFilter()
{
  static bool initialized = false;
  if (!initialized)
  {
    glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropicFilter);
    initialized = true;
  }
  return maxAnisotropicFilter;
}

Tuple3f Renderer::getCameraPosition()
{
  Tuple3f  cameraPosition;
  Matrix4f matrixBuffer;
  
  glGetFloatv(GL_MODELVIEW_MATRIX , modelviewMatrix);
  matrixBuffer = modelviewMatrix;
  matrixBuffer.setInverse();
  
  cameraPosition.set(matrixBuffer[12], matrixBuffer[13], matrixBuffer[14]);
  return cameraPosition;
}

void Renderer::applyCubeFaceMatrix(int face, const Tuple3f &centerToBeNegated)
{
  applyCubeFaceMatrix(face, centerToBeNegated.x, centerToBeNegated.y, centerToBeNegated.z);
}

void Renderer::applyCubeFaceMatrix(int face, float x, float y, float z)
{

  switch (face)
  {
    case 0:
      glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
      glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
      break;
    case 1:
      glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
      glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
      break;
    case 2:
      glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
      break;
    case 3:
      glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
      break;
    case 4:
      glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
      break;
    case 5:
      glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
      break;
  }
  
  glTranslatef(-x, -y, -z);
}