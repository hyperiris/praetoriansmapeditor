/**
 * A 3-passes radix-sort using a 11-bit wide radix.
 * \file  IceRadix3Passes.cpp
 * \author  Pierre Terdiman
 * \date  December, 20, 2006
 */

#include "Radix.h"
#include "../Tools/Logger.h"

#define INVALIDATE_RANKS  mCurrentSize|=0x80000000
#define VALIDATE_RANKS    mCurrentSize&=0x7fffffff
#define CURRENT_SIZE      (mCurrentSize&0x7fffffff)
#define INVALID_RANKS     (mCurrentSize&0x80000000)

#define RADIX_NB_BITS 11
#define RADIX_SIZE 2048
#define MAX_NB_PASSES 3
#define H0_OFFSET 0
#define H1_OFFSET RADIX_SIZE
#define H2_OFFSET RADIX_SIZE*2

#define CREATE_HISTOGRAMS(type, buffer) \
 memset(Histogram, 0, RADIX_SIZE*MAX_NB_PASSES*sizeof(unsigned int)); \
 const unsigned int* p = (const unsigned int*)input; \
 const unsigned int* pe = (const unsigned int*)(input+nb); \
 unsigned int* h0 = &Histogram[H0_OFFSET]; \
 unsigned int* h1 = &Histogram[H1_OFFSET]; \
 unsigned int* h2 = &Histogram[H2_OFFSET]; \
 bool AlreadySorted = true; \
 if(INVALID_RANKS) \
 { \
  const type* Running = (type*)buffer; \
  type PrevVal = *Running; \
  while(p!=pe) \
  { \
   const type Val = *Running++; \
   if(Val<PrevVal) { AlreadySorted = false; break; } \
   PrevVal = Val; \
   unsigned int Data = *p++; \
   unsigned int b0 = Data & 2047; \
   unsigned int b1 = (Data>>RADIX_NB_BITS) & 2047; \
   unsigned int b2 = (Data>>(RADIX_NB_BITS*2)) & 2047; \
   h0[b0]++; \
   h1[b1]++; \
   h2[b2]++; \
  } \
  if(AlreadySorted) \
  { \
   mNbHits++; \
   for(unsigned int i=0;i<nb;i++) mRanks[i] = i; \
   return *this; \
  } \
 } \
 else \
 { \
  const unsigned int* Indices = mRanks; \
  type PrevVal = (type)buffer[*Indices]; \
  while(p!=pe) \
  { \
   const type Val = (type)buffer[*Indices++]; \
   if(Val<PrevVal) { AlreadySorted = false; break; }  \
   PrevVal = Val; \
   unsigned int Data = *p++; \
   unsigned int b0 = Data & 2047; \
   unsigned int b1 = (Data>>RADIX_NB_BITS) & 2047; \
   unsigned int b2 = (Data>>(RADIX_NB_BITS*2)) & 2047; \
   h0[b0]++; \
   h1[b1]++; \
   h2[b2]++; \
  } \
  if(AlreadySorted) { mNbHits++; return *this; } \
 } \
 while(p!=pe) \
 { \
  const unsigned int Data = *p++; \
  const unsigned int b0 = Data & 2047; \
  const unsigned int b1 = (Data>>RADIX_NB_BITS) & 2047; \
  const unsigned int b2 = (Data>>(RADIX_NB_BITS*2)) & 2047; \
  h0[b0]++; \
  h1[b1]++; \
  h2[b2]++; \
 }

RadixSort3::RadixSort3() : mRanks(0), mRanks2(0), mCurrentSize(0), mTotalCalls(0), mNbHits(0), mDeleteRanks(true)
{
  INVALIDATE_RANKS;
}

RadixSort3& RadixSort3::Sort(const unsigned int* input, unsigned int nb, RadixHint hint)
{
  if (!input || !nb || nb&0x80000000) return *this;
  
  mTotalCalls++;
  
  CheckResize(nb);
  
  unsigned int Histogram[RADIX_SIZE*MAX_NB_PASSES];
  unsigned int* Link[RADIX_SIZE];
  
  if (hint==RADIX_UNSIGNED)
  {
    CREATE_HISTOGRAMS(unsigned int, input);
  }
  else
  {
    CREATE_HISTOGRAMS(signed int, input);
  }
  
  for (unsigned int j=0;j<MAX_NB_PASSES;j++)
  {
    const unsigned int* CurCount = &Histogram[j<<RADIX_NB_BITS];
    bool PerformPass = true;
    unsigned int UniqueVal = *input;
    UniqueVal >>= j*RADIX_NB_BITS;
    UniqueVal &= 2047;
    if (CurCount[UniqueVal]==nb) PerformPass=false;
    
    if (PerformPass)
    {
      if (j!=MAX_NB_PASSES-1 || hint==RADIX_UNSIGNED)
      {
        Link[0] = mRanks2;
        for (unsigned int i=1;i<RADIX_SIZE;i++)
          Link[i] = Link[i-1] + CurCount[i-1];
      }
      else
      {
#ifdef KYLE_HUBERT_VERSION
        Link[1024/2] = mRanks2;
        for (unsigned int i=1+1024/2;i<1024;i++)
          Link[i] = Link[i-1] + CurCount[i-1];
          
        Link[0] = Link[1024-1] + CurCount[1024-1];
        for (unsigned int i=1;i<1024/2;i++)
          Link[i] = Link[i-1] + CurCount[i-1];
#else
        unsigned int NbNegativeValues = 0;
        if (hint==RADIX_SIGNED)
        {
          const unsigned int* h2 = &Histogram[H2_OFFSET];
          for (unsigned int i=1024/2;i<1024;i++) NbNegativeValues += h2[i];
        }
          
        Link[0] = &mRanks2[NbNegativeValues];
        for (unsigned int i=1;i<1024/2;i++)
          Link[i] = Link[i-1] + CurCount[i-1];
          
        Link[1024/2] = mRanks2;
        for (unsigned int i=1024/2+1;i<1024;i++)
          Link[i] = Link[i-1] + CurCount[i-1];
#endif
      }
      
      const unsigned int Shift = j*RADIX_NB_BITS;
      if (INVALID_RANKS)
      {
        for (unsigned int i=0;i<nb;i++)
        {
          unsigned int data = (input[i]>>Shift)&2047;
          *Link[data]++ = i;
        }
        VALIDATE_RANKS;
      }
      else
      {
        const unsigned int* Indices  = mRanks;
        const unsigned int* IndicesEnd = mRanks + nb;
        while (Indices!=IndicesEnd)
        {
          unsigned int id = *Indices++;
          unsigned int data = (input[id]>>Shift)&2047;
          *Link[data]++ = id;
        }
      }
      
      unsigned int* Tmp = mRanks;
      mRanks = mRanks2;
      mRanks2 = Tmp;
    }
  }
  return *this;
}

RadixSort3& RadixSort3::Sort(const float* input2, unsigned int nb)
{
  if (!input2 || !nb || nb&0x80000000) return *this;
  
  mTotalCalls++;
  
  const unsigned int* input = (const unsigned int*)input2;
  
  CheckResize(nb);
  
  unsigned int Histogram[RADIX_SIZE*MAX_NB_PASSES];
  unsigned int* Link[RADIX_SIZE];
  
  CREATE_HISTOGRAMS(float, input2);
  
  for (unsigned int j=0;j<MAX_NB_PASSES;j++)
  {
    const unsigned int* CurCount = &Histogram[j<<RADIX_NB_BITS];
    bool PerformPass = true;
    unsigned int UniqueVal = *input;
    UniqueVal >>= j*RADIX_NB_BITS;
    UniqueVal &= 2047;
    if (CurCount[UniqueVal]==nb) PerformPass=false;
    
    if (j!=MAX_NB_PASSES-1)
    {
      if (PerformPass)
      {
        Link[0] = mRanks2;
        for (unsigned int i=1;i<RADIX_SIZE;i++)
          Link[i] = Link[i-1] + CurCount[i-1];
          
        const unsigned int Shift = j*RADIX_NB_BITS;
        if (INVALID_RANKS)
        {
          for (unsigned int i=0;i<nb;i++)
          {
            unsigned int data = (input[i]>>Shift)&2047;
            *Link[data]++ = i;
          }
          VALIDATE_RANKS;
        }
        else
        {
          const unsigned int* Indices  = mRanks;
          const unsigned int* IndicesEnd = mRanks + nb;
          while (Indices!=IndicesEnd)
          {
            unsigned int id = *Indices++;
            unsigned int data = (input[id]>>Shift)&2047;
            *Link[data]++ = id;
          }
        }
        
        unsigned int* Tmp = mRanks;
        mRanks = mRanks2;
        mRanks2 = Tmp;
      }
    }
    else
    {
      if (PerformPass)
      {
#ifdef KYLE_HUBERT_VERSION
        Link[1024-1] = mRanks2 + CurCount[1024-1];
        for (unsigned int i=1024-2;i>(1024/2)-1;i--) Link[i] = Link[i+1] + CurCount[i];
        
        Link[0] = Link[1024/2] + CurCount[1024/2];
        for (unsigned int i=1;i<1024/2;i++) Link[i] = Link[i-1] + CurCount[i-1];
#else
        unsigned int NbNegativeValues = 0;
        const unsigned int* h2 = &Histogram[H2_OFFSET];
        for (unsigned int i=1024/2;i<1024;i++) NbNegativeValues += h2[i];
        
        Link[0] = &mRanks2[NbNegativeValues];
        for (unsigned int i=1;i<1024/2;i++)  Link[i] = Link[i-1] + CurCount[i-1];
        
        Link[1023] = mRanks2;
        for (unsigned int i=0;i<1024/2-1;i++) Link[1022-i] = Link[1023-i] + CurCount[1023-i];
        for (unsigned int i=1024/2;i<1024;i++) Link[i] += CurCount[i];
#endif
        if (INVALID_RANKS)
        {
          for (unsigned int i=0;i<nb;i++)
          {
            const unsigned int Radix = input[i]>>22;
            if (Radix<1024/2) *Link[Radix]++ = i;
            else    *(--Link[Radix]) = i;
          }
          VALIDATE_RANKS;
        }
        else
        {
          for (unsigned int i=0;i<nb;i++)
          {
            const unsigned int Radix = input[mRanks[i]]>>22;
            if (Radix<1024/2) *Link[Radix]++ = mRanks[i];
            else    *(--Link[Radix]) = mRanks[i];
          }
        }
        unsigned int* Tmp = mRanks;
        mRanks = mRanks2;
        mRanks2 = Tmp;
      }
      else
      {
        if (UniqueVal>=1024/2)
        {
          if (INVALID_RANKS)
          {
            for (unsigned int i=0;i<nb;i++) mRanks2[i] = nb-i-1;
            VALIDATE_RANKS;
          }
          else
          {
            for (unsigned int i=0;i<nb;i++) mRanks2[i] = mRanks[nb-i-1];
          }
          
          unsigned int* Tmp = mRanks;
          mRanks = mRanks2;
          mRanks2 = Tmp;
        }
      }
    }
  }
  return *this;
}

bool RadixSort3::SetRankBuffers(unsigned int* ranks0, unsigned int* ranks1)
{
  if (!ranks0 || !ranks1) return false;
  
  mRanks = ranks0;
  mRanks2 = ranks1;
  mDeleteRanks = false;
  return true;
}

bool RadixSort3::Resize(unsigned int nb)
{
  if (mDeleteRanks)
  {
    deleteArray(mRanks2);
    deleteArray(mRanks);
    
    mRanks = new unsigned int[nb];
    mRanks2 = new unsigned int[nb];
    if (!mRanks && !mRanks2) return false;
  }
  return true;
}

inline void RadixSort3::CheckResize(unsigned int nb)
{
  unsigned int CurSize = CURRENT_SIZE;
  if (nb!=CurSize)
  {
    if (nb>CurSize) Resize(nb);
    mCurrentSize = nb;
    INVALIDATE_RANKS;
  }
}

RadixSort3::~RadixSort3()
{
  if (mDeleteRanks)
  {
    deleteArray(mRanks2);
    deleteArray(mRanks);
  }
}
