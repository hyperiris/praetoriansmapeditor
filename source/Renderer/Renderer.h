#ifndef RENDERER_H
#define RENDERER_H

#include "../Appearance/Appearance.h"
#include "../Viewer/Frustum.h"
#include "../Viewer/Camera.h"
#include "../Math/MathUtils.h"
#include "GLPBuffer.h"
#include <gl/glu.h>

class TransformGroup;
class Renderer
{
  public:
  
    static Tuple4i getScissorRectAndDepthBounds(const Tuple4f &lightInfoWRange,
        Tuple2f &zMinzMax);
    static Tuple3f getScreenCoords(const Tuple3f &worldPosition);
    static Tuple3f getWorldCoords(const Tuple3f &windowPosition);
    
    static Tuple4i getScissorRect(const Tuple3f &lightPosition,
                                  float lightRange);
                                  
    static Tuple3f getCameraPosition();
    
    static void    setCurrentFieldOfView(float fov);
    static float   getCurrentFieldOfView();
    
    static void    enter2DModeInverse(GLint winWidth = 0, GLint winHeight = 0);
    static void    enter2DMode(GLint winWidth = 0, GLint winHeight = 0);
    static void    exit2DMode();
    
    static void    forceCPUVolumeExtrusion(bool or);
    static bool    CPUVolumeExtrusionForced();
    
    static void    enableGPUVolumeExtrusion();
    static void    disableGPUVolumeExtrusion();
    
    static int     getMaxAvailableTextureUnits();
    static int     getMaxAnisotropicFilter();
    
    static void    applyCubeFaceMatrix(int face, const Tuple3f &centerToBeNegated);
    static void    applyCubeFaceMatrix(int face, float x, float y, float z);
    
  private:
  
    static ShaderAttributes shadowVolumeExtrusion;
    
    static Matrix4f         projectionMatrix,
    modelviewMatrix,
    spareMatrix;
    
    static Tuple4f          corners[8];
    static Tuple4i          viewport;
    
    static float            fieldOfView;
    
    static bool             shadowVolumesVPLoaded,
    forceCPUExtrusion;
    
    static int              availableTextureUnits,
    maxAnisotropicFilter;
    
    
    
    
};

#endif