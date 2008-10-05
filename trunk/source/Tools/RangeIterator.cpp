#include "RangeIterator.h"

RangeIterator::RangeIterator() : runLength(0)
{}

RangeIterator::RangeIterator(unsigned int run, const Tuple4i &range) : runLength(run), area(range)
{
  element.set(range.x, range.y);
}

RangeIterator::RangeIterator(const RangeIterator &source)
{
  operator = (source);
}

RangeIterator& RangeIterator::operator = (const RangeIterator & right)
{
  if (this != &right)
  {
    runLength = right.runLength;
    area      = right.area;
    element   = right.element;
  }
  
  return *this;
}

RangeIterator& RangeIterator::operator ++ ()
{
  element.x++;
  
  if (element.x > area.z)
  {
    element.y++;
    element.x = area.x;
  }
  
  return *this;
}

RangeIterator RangeIterator::operator ++ (int)
{
  RangeIterator temp = *this;
  element.x++;
  
  if (element.x > area.z)
  {
    element.y++;
    element.x = area.x;
  }
  
  return temp;
}

void RangeIterator::set(unsigned int run, const Tuple4i &range)
{
  runLength = run;
  area = range;
  element.set(range.x, range.y);
}

unsigned int RangeIterator::current()
{
  return element.y * runLength + element.x;
}

bool RangeIterator::end()
{
  return (element.y > area.w);
}