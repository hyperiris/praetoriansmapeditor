#include "Timer.h"
#include <windows.h>

LARGE_INTEGER currentTime,
frequency;

void Timer::intialize()
{
  QueryPerformanceFrequency(&frequency);
}
__int64 Timer::getCurrentTime()
{
  QueryPerformanceCounter(&currentTime);
  return currentTime.QuadPart;
}

__int64 Timer::getPrecision()
{
  return frequency.QuadPart;
}

float Timer::getElapsedTimeSeconds(__int64 lastTime)
{
  return float(getCurrentTime() - lastTime)/frequency.QuadPart;
}
