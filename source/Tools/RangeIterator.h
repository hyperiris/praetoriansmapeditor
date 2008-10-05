#ifndef RANGEITERATOR
#define RANGEITERATOR

#include "../Math/Tuple4.h"

class RangeIterator
{
  public:
    RangeIterator();
    RangeIterator(const RangeIterator &source);
    RangeIterator(unsigned int run, const Tuple4i &range);
    
  public:
    RangeIterator&  operator = (const RangeIterator &right);
    RangeIterator&  operator ++ ();
    RangeIterator   operator ++ (int);
    
    void set(unsigned int run, const Tuple4i &range);
    unsigned int current();
    bool end();
    
  private:
    unsigned int runLength;
    Tuple4i area;
    Tuple2i element;
};

#endif