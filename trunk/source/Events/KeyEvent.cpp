#include "KeyEvent.h"
#include <windows.h>

KeyEvent::KeyEvent(int keyID)
{
  id = keyID;
}

KeyEvent::KeyEvent(const KeyEvent& copy)
{
  this->operator =(copy);
}

KeyEvent &KeyEvent::operator= (const KeyEvent& copy)
{
  if (this != &copy)
  {
    id = copy.id;
  }
  return *this;
}

bool KeyEvent::displayable()
{
  return id >= 32 && id <= 126;
}

char KeyEvent::getRealKeyChar()
{
  if ((id >= 65 && id <= 90))
    if (!GetKeyState(VK_CAPITAL)&& !GetAsyncKeyState(VK_SHIFT))
      id += 32;
  return id;
}

int KeyEvent::getRealKeyID()
{
  if ((id >= 65 && id <= 90))
    if (!GetKeyState(VK_CAPITAL) && !GetAsyncKeyState(VK_SHIFT))
      id += 32;
  return id;
}

char KeyEvent::getKeyChar()
{
  return char(id);
}
int  KeyEvent::getKeyID()
{
  return id      ;
}