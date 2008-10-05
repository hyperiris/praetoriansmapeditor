#ifndef RADIXSORT
#define RADIXSORT

#include "../Containers/ocport.h"

typedef const char *cpointer;

#define PREFETCH 1

#if PREFETCH
#include <xmmintrin.h> // for prefetch
#define pfval 64
#define pfval2 128
#define pf(x) _mm_prefetch(cpointer(x + i + pfval), 0)
#define pf2(x) _mm_prefetch(cpointer(x + i + pfval2), 0)
#else
#define pf(x)
#define pf2(x)
#endif

inline unsigned long floatFlip(unsigned long f)
{
  unsigned long mask = -long(f >> 31) | 0x80000000;
  return f ^ mask;
}

inline void floatFlipX(unsigned long &f)
{
  unsigned long mask = -long(f >> 31) | 0x80000000;
  f ^= mask;
}

inline unsigned long floatFlipInv(unsigned long f)
{
  unsigned long mask = ((f >> 31) - 1) | 0x80000000;
  return f ^ mask;
}

void radixSort(float *farray, float *sorted, unsigned long elements)
{
  unsigned long i;
  unsigned long *sort  = (unsigned long*)sorted;
  unsigned long *parray = (unsigned long*)farray;
  
  const unsigned long kHist = 2048;
  unsigned long b0[kHist * 3];
  
  unsigned long *b1 = b0 + kHist;
  unsigned long *b2 = b1 + kHist;
  
  for (i = 0; i < kHist * 3; i++)
    b0[i] = 0;
    
  for (i = 0; i < elements; i++)
  {
    pf(parray);
    unsigned long fi = floatFlip((unsigned long&)parray[i]);
    b0[(fi & 0x7FF)] ++;
    b1[(fi >> 11 & 0x7FF)] ++;
    b2[(fi >> 22)] ++;
  }
  
  unsigned long sum0 = 0, sum1 = 0, sum2 = 0;
  unsigned long tsum;
  for (i = 0; i < kHist; i++)
  {
    tsum = b0[i] + sum0;
    b0[i] = sum0 - 1;
    sum0 = tsum;
    
    tsum = b1[i] + sum1;
    b1[i] = sum1 - 1;
    sum1 = tsum;
    
    tsum = b2[i] + sum2;
    b2[i] = sum2 - 1;
    sum2 = tsum;
  }
  
  for (i = 0; i < elements; i++)
  {
    unsigned long fi = parray[i];
    floatFlipX(fi);
    unsigned long pos = (fi & 0x7FF);
    pf2(parray);
    sort[++b0[pos]] = fi;
  }
  
  for (i = 0; i < elements; i++)
  {
    unsigned long si = sort[i];
    unsigned long pos = (si >> 11 & 0x7FF);
    pf2(sort);
    parray[++b1[pos]] = si;
  }
  
  for (i = 0; i < elements; i++)
  {
    unsigned long ai = parray[i];
    unsigned long pos = (ai >> 22);
    pf2(parray);
    sort[++b2[pos]] = floatFlipInv(ai);
  }
  
  memcpy(parray, sorted, elements * 4);
}

#endif