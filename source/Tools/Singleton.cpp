#include "Singleton.h"

Singleton::Singleton()
{}

Singleton* Singleton::getInstance()
{
  static Singleton inst;
  return &inst;
}
