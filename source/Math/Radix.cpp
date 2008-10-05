/**
 * Contains source code from the article "Radix Sort Revisited".
 * \file  IceRevisitedRadix.cpp
 * \author  Pierre Terdiman
 * \date  April, 4, 2000
 */

#include "Radix.h"
#include "../Tools/Logger.h"

RadixSort::RadixSort() : mIndices(0), mIndices2(0), mCurrentSize(0), mPreviousSize(0), mTotalCalls(0), mNbHits(0)
{
#ifndef RADIX_LOCAL_RAM
  mHistogram  = new unsigned int[256*4];
  mOffset = new unsigned int[256];
#endif
  ResetIndices();
}

#define CREATE_HISTOGRAMS(type, buffer) \
 memset(mHistogram, 0, 256*4*sizeof(unsigned int)); \
 type PrevVal = (type)buffer[mIndices[0]]; \
 bool AlreadySorted = true; \
 unsigned int* Indices = mIndices; \
 unsigned char* p = (unsigned char*)input; \
 unsigned char* pe = &p[nb*4]; \
 unsigned int* h0= &mHistogram[0]; \
 unsigned int* h1= &mHistogram[256]; \
 unsigned int* h2= &mHistogram[512]; \
 unsigned int* h3= &mHistogram[768]; \
 while(p!=pe) \
 { \
  type Val = (type)buffer[*Indices++]; \
  if(Val<PrevVal) { AlreadySorted = false; break; } \
  PrevVal = Val; \
  h0[*p++]++; h1[*p++]++; h2[*p++]++; h3[*p++]++; \
 } \
 if(AlreadySorted) { mNbHits++; return *this; } \
 while(p!=pe) \
 { \
  h0[*p++]++; h1[*p++]++; h2[*p++]++; h3[*p++]++; \
 }

#define CHECK_PASS_VALIDITY(pass) \
 unsigned int* CurCount = &mHistogram[pass<<8]; \
 bool PerformPass = true; \
 unsigned char UniqueVal = *(((unsigned char*)input)+pass); \
 if(CurCount[UniqueVal]==nb) PerformPass=false;


RadixSort& RadixSort::Sort(const unsigned int* input, unsigned int nb, bool signedvalues)
{
  if (!input || !nb) return *this;
  
  mTotalCalls++;
  
  CheckResize(nb);
  
#ifdef RADIX_LOCAL_RAM
  unsigned int mHistogram[256*4];
  unsigned int mOffset[256];
#endif
  
  if (!signedvalues)
  {
    CREATE_HISTOGRAMS(unsigned int, input);
  }
  else
  {
    CREATE_HISTOGRAMS(signed int, input);
  }
  
  unsigned int NbNegativeValues = 0;
  if (signedvalues)
  {
    unsigned int* h3= &mHistogram[768];
    for (unsigned int i=128;i<256;i++) NbNegativeValues += h3[i];
  }
  
  for (unsigned int j=0;j<4;j++)
  {
    CHECK_PASS_VALIDITY(j);
    
    if (PerformPass)
    {
      if (j!=3 || !signedvalues)
      {
        mOffset[0] = 0;
        for (unsigned int i=1;i<256;i++)  mOffset[i] = mOffset[i-1] + CurCount[i-1];
      }
      else
      {
        mOffset[0] = NbNegativeValues;
        for (unsigned int i=1;i<128;i++)  mOffset[i] = mOffset[i-1] + CurCount[i-1];
        
        mOffset[128] = 0;
        for (unsigned int i=129;i<256;i++)   mOffset[i] = mOffset[i-1] + CurCount[i-1];
      }
      
      unsigned char* InputBytes = (unsigned char*)input;
      unsigned int* Indices  = mIndices;
      unsigned int* IndicesEnd = &mIndices[nb];
      InputBytes += j;
      while (Indices!=IndicesEnd)
      {
        unsigned int id = *Indices++;
        mIndices2[mOffset[InputBytes[id<<2]]++] = id;
      }
      
      unsigned int* Tmp = mIndices;
      mIndices = mIndices2;
      mIndices2 = Tmp;
    }
  }
  return *this;
}

RadixSort& RadixSort::Sort(const float* input2, unsigned int nb)
{
  if (!input2 || !nb) return *this;
  
  mTotalCalls++;
  
  unsigned int* input = (unsigned int*)input2;
  
  CheckResize(nb);
  
#ifdef RADIX_LOCAL_RAM
  unsigned int mHistogram[256*4];
  unsigned int mOffset[256];
#endif
  
  {
    CREATE_HISTOGRAMS(float, input2);
  }
  unsigned int NbNegativeValues = 0;
  unsigned int* h3= &mHistogram[768];
  for (unsigned int i=128;i<256;i++) NbNegativeValues += h3[i];
  
  for (unsigned int j=0;j<4;j++)
  {
    if (j!=3)
    {
      CHECK_PASS_VALIDITY(j);
      
      if (PerformPass)
      {
        mOffset[0] = 0;
        for (unsigned int i=1;i<256;i++)  mOffset[i] = mOffset[i-1] + CurCount[i-1];
        
        unsigned char* InputBytes = (unsigned char*)input;
        unsigned int* Indices  = mIndices;
        unsigned int* IndicesEnd = &mIndices[nb];
        InputBytes += j;
        while (Indices!=IndicesEnd)
        {
          unsigned int id = *Indices++;
          mIndices2[mOffset[InputBytes[id<<2]]++] = id;
        }
        
        unsigned int* Tmp = mIndices;
        mIndices = mIndices2;
        mIndices2 = Tmp;
      }
    }
    else
    {
      CHECK_PASS_VALIDITY(j);
      
      if (PerformPass)
      {
        mOffset[0] = NbNegativeValues;
        for (unsigned int i=1;i<128;i++)  mOffset[i] = mOffset[i-1] + CurCount[i-1];
        
        mOffset[255] = 0;
        for (unsigned int i=0;i<127;i++)  mOffset[254-i] = mOffset[255-i] + CurCount[255-i];
        for (unsigned int i=128;i<256;i++) mOffset[i] += CurCount[i];
        
        for (unsigned int i=0;i<nb;i++)
        {
          unsigned int Radix = input[mIndices[i]]>>24;
          if (Radix<128)  mIndices2[mOffset[Radix]++] = mIndices[i];
          else    mIndices2[--mOffset[Radix]] = mIndices[i];
        }
        unsigned int* Tmp = mIndices;
        mIndices = mIndices2;
        mIndices2 = Tmp;
      }
      else
      {
        if (UniqueVal>=128)
        {
          for (unsigned int i=0;i<nb;i++) mIndices2[i] = mIndices[nb-i-1];
          unsigned int* Tmp = mIndices;
          mIndices = mIndices2;
          mIndices2 = Tmp;
        }
      }
    }
  }
  return *this;
}

void RadixSort::ResetIndices()
{
  for (unsigned int i=0;i<mCurrentSize;i++) mIndices[i] = i;
}

unsigned int RadixSort::GetUsedRam() const
{
  unsigned int UsedRam = sizeof(RadixSort);
#ifndef RADIX_LOCAL_RAM
  UsedRam += 256*4*sizeof(unsigned int);
  UsedRam += 256*sizeof(unsigned int);
#endif
  UsedRam += 2*mCurrentSize*sizeof(unsigned int);
  return UsedRam;
}

bool RadixSort::Resize(unsigned int nb)
{
  deleteArray(mIndices2);
  deleteArray(mIndices);
  
  mIndices  = new unsigned int[nb];
  if (!mIndices)
    return false;
  mIndices2  = new unsigned int[nb];
  if (!mIndices2)
    return false;
  mCurrentSize = nb;
  
  ResetIndices();
  
  return true;
}

void RadixSort::CheckResize(unsigned int n)
{
  if (n != mPreviousSize)
  {
    if (n > mCurrentSize)
      Resize(n);
    else
      ResetIndices();
    mPreviousSize = n;
  }
}

RadixSort::~RadixSort()
{
#ifndef RADIX_LOCAL_RAM
  deleteArray(mOffset);
  deleteArray(mHistogram);
#endif
  deleteArray(mIndices2);
  deleteArray(mIndices);
}
