#include "Camera.h"
#include "../Tools/Logger.h"

Camera::Camera(float x , float y , float z,
               float xf, float yf, float zf,
               float xu, float yu, float zu)

{

  mouseSensitivity = 0.25f;
  currentXRotation = 0.0f;
  speed = 0.0f;
  
  mouseLocked = false;
  active = true;
  
  memset(activeKeys, false, 6);
  setup(x, y, z, xf, yf, zf, xu, yu, zu);
  setTranslationSpeed(0.5f);
}

void Camera::setup(const Tuple3f &positionArg,
                   const Tuple3f &focusArg,
                   const Tuple3f &upArg)
{
  position = positionArg;
  focus = focusArg;
  up = upArg;
}

void Camera::setup(float x , float y , float z,
                   float xf, float yf, float zf,
                   float xu, float yu, float zu)
{
  position.set(x , y , z);
  focus.set(xf, yf, zf);
  up.set(xu, yu, zu);
}

void Camera::update(float frameInt)
{
  Tuple3f temp = focus;
  temp -= position;
  
  strafe.crossProduct(temp, up);
  strafe.normalize();
  
  speed = frameInt*translationSpeed;
  
  setViewByMouse();
  processKeyboardEvents();
  
  viewMatrix.lookAt(position, focus, up);
  look();
}

void Camera::setMouseSensitivity(float sensitivity)
{
  mouseSensitivity = clamp(sensitivity, 0.0f, 1.0f);
}

void Camera::setTranslationSpeed(float speed)
{
  translationSpeed = clamp(speed, 0.0f, 1.0f);
  translationSpeed *= 300.0f;
}

void Camera::lockMouse(bool value)
{
  if (value)
    mouseLockedPosition = mousePosition;
    
  mouseLocked = value;
}

void Camera::move(float speed)
{
  Tuple3f temp = focus;
  
  temp -= position;
  temp.normalize();
  
  temp *= speed;
  
  position += temp;
  focus += temp;
}

void Camera::look()
{
  glMatrixMode(GL_MODELVIEW);
  glLoadMatrixf(viewMatrix);
}

void Camera::strafeMove(float speed)
{
  position.x += strafe.x * speed;
  position.z += strafe.z * speed;
  
  focus.x += strafe.x * speed;
  focus.z += strafe.z * speed;
}

void Camera::rotateView(float angle, float x, float y, float z)
{
  Tuple3f newView,
  vView = focus;
  
  float cosTheta = fastCos(angle), sinTheta = fastSin(angle);
  
  vView -= position;
  
  // Find the new x position for the new rotated point
  newView.x = (cosTheta + (1 - cosTheta) * x * x)  * vView.x;
  newView.x += ((1 - cosTheta) * x * y - z * sinTheta) * vView.y;
  newView.x += ((1 - cosTheta) * x * z + y * sinTheta) * vView.z;
  
  // Find the new y position for the new rotated point
  newView.y = ((1 - cosTheta) * x * y + z * sinTheta) * vView.x;
  newView.y += (cosTheta + (1 - cosTheta) * y * y)  * vView.y;
  newView.y += ((1 - cosTheta) * y * z - x * sinTheta) * vView.z;
  
  // Find the new z position for the new rotated point
  newView.z = ((1 - cosTheta) * x * z - y * sinTheta) * vView.x;
  newView.z += ((1 - cosTheta) * y * z + x * sinTheta) * vView.y;
  newView.z += (cosTheta + (1 - cosTheta) * z * z)  * vView.z;
  
  // Now we just add the newly rotated vector to our position to set
  // our new rotated view of our camera.
  focus = position;
  focus += newView;
}

void Camera::elevate(float speed)
{
  position.y += speed;
  focus.y += speed;
}

void Camera::setViewByMouse()
{
  float angleY = 0.0f, angleZ = 0.0f;
  
  if ((mousePosition == mouseLockedPosition) || !mouseLocked)
    return;
    
  angleY = float(mouseLockedPosition.x - mousePosition.x)*DEG2RAD*mouseSensitivity;
  angleZ = float(mouseLockedPosition.y - mousePosition.y)*DEG2RAD*mouseSensitivity;
  
  currentXRotation -= angleZ;
  
  Tuple3f vAxis = focus;
  vAxis -= position;
  vAxis ^= up;
  vAxis.normalize();
  
  rotateView(angleZ, vAxis.x, vAxis.y, vAxis.z);
  rotateView(angleY, 0, 1, 0);
  
  if (mouseLocked)
    mouseLockedPosition = mousePosition;
}

void Camera::setMouseInfo(Tuple2i &mousePosition)
{
  setMouseInfo(mousePosition.x, mousePosition.y);
}

void Camera::setMouseInfo(int x, int y)
{
  mousePosition.set(x, y);
}

void Camera::processKeyboardEvents()
{
  if (activeKeys[0]) move(speed);
  if (activeKeys[1]) move(-speed);
  if (activeKeys[2]) strafeMove(-speed);
  if (activeKeys[3]) strafeMove(speed);
  if (activeKeys[4]) elevate(speed);
  if (activeKeys[5]) elevate(-speed);
}

void Camera::setKeyboardInput(KeyEvent &event, bool active)
{
  switch (event.getKeyID())
  {
    case KEY_W:
    case KEY_UP:
      activeKeys[0] = active;
      break;
    case KEY_S:
    case KEY_DOWN:
      activeKeys[1] = active;
      break;
    case KEY_A:
    case KEY_LEFT:
      activeKeys[2] = active;
      break;
    case KEY_D:
    case KEY_RIGHT:
      activeKeys[3] = active;
      break;
    case KEY_E:
      activeKeys[4] = active;
      break;
    case KEY_Q:
      activeKeys[5] = active;
      break;
    default:
      break;
  }
}

void Camera::setViewerPosition(const Tuple3f &position_)
{
  position = position_;
}

void Camera::setFocusPosition(const Tuple3f &focus_)
{
  focus = focus_;
}

void Camera::setUpDirection(const Tuple3f &up_)
{
  up = up_;
}

const Matrix4f &Camera::getModelViewMatrix() const
{
  return viewMatrix;
}

const Tuple3f  &Camera::getViewerPosition() const
{
  return position;
}

const Tuple3f  &Camera::getFocusPosition() const
{
  return focus;
}

const Tuple3f  &Camera::getUpDirection() const
{
  return up;
}
