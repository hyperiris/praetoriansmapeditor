#ifndef RADIX
#define RADIX

#define RADIX_LOCAL_RAM

enum RadixHint
{
  RADIX_SIGNED,
  RADIX_UNSIGNED,
  RADIX_FORCE_DWORD = 0x7fffffff
};

class RadixSort
{
  public:
    RadixSort();
    ~RadixSort();
    
  public:
    RadixSort& Sort(const unsigned int* input, unsigned int nb, bool signedvalues = true);
    RadixSort& Sort(const float* input, unsigned int nb);
    
    inline unsigned int* GetIndices() const
    {
      return mIndices;
    }
    
    inline unsigned int* GetRecyclable() const
    {
      return mIndices2;
    }
    
    unsigned int GetUsedRam() const;
    
    inline unsigned int GetNbTotalCalls() const
    {
      return mTotalCalls;
    }
    
    inline unsigned int GetNbHits() const
    {
      return mNbHits;
    }
    
  private:
    RadixSort(const RadixSort& object);
    RadixSort& operator = (const RadixSort& object);
    
    bool Resize(unsigned int nb);
    void ResetIndices();
    void CheckResize(unsigned int n);
    
  private:
#ifndef RADIX_LOCAL_RAM
    unsigned int* mHistogram;
    unsigned int* mOffset;
#endif
    unsigned int mCurrentSize;
    unsigned int mPreviousSize;
    unsigned int* mIndices;
    unsigned int* mIndices2;
    
    unsigned int mTotalCalls;
    unsigned int mNbHits;
};

class RadixSort3
{
  public:
    RadixSort3();
    ~RadixSort3();
    
  public:
    RadixSort3& Sort(const unsigned int* input, unsigned int nb, RadixHint = RADIX_SIGNED);
    RadixSort3& Sort(const float* input, unsigned int nb);
    
    inline const unsigned int* GetRanks() const
    {
      return mRanks;
    }
    
    inline unsigned int* GetRecyclable() const
    {
      return mRanks2;
    }
    
    inline unsigned int GetNbTotalCalls() const
    {
      return mTotalCalls;
    }
    
    inline unsigned int GetNbHits() const
    {
      return mNbHits;
    }
    
    bool SetRankBuffers(unsigned int* ranks0, unsigned int* ranks1);
    
  private:
    void CheckResize(unsigned int nb);
    bool Resize(unsigned int nb);
    
  private:
    unsigned int mCurrentSize;
    unsigned int* mRanks;
    unsigned int* mRanks2;
    
    unsigned int mTotalCalls;
    unsigned int mNbHits;
    
    bool mDeleteRanks;
};

#endif
