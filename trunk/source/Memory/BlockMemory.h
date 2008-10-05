#ifndef BLOCKMEMORY
#define BLOCKMEMORY

class MemChunkDescriptor;

class BlockMemory
{
  public:
    BlockMemory();
    BlockMemory(unsigned int chunkSize, unsigned int allocChunks);
    ~BlockMemory();
    
  public:
    bool                initialize(unsigned int  bytes = 1024 * 1024);
    
    bool                release(void *memory, bool setval = false);
    void*               aquire(unsigned int bytes);
    
    void                setChunkSize(unsigned int bytes);
    void                setMinAllocSize(unsigned int bytes);
    
    void                printMemoryBlock(const char* fileName);
    void                printMemoryProfile(const char* fileName);
    
    void                destroy();
    void                reset();
    
    bool                withinBounds(void* addr);
    unsigned long       getStartAddress();
    
    unsigned int        getTotalSize();
    unsigned int        getUsedSize();
    
  private:
    void                deallocateMemory();
    void                deallocateChunks();
    void                freeRegion(MemChunkDescriptor* allocator, bool setval);
    
    void                moveToNextRegion();
    
    unsigned int        calculateRealUsedBytes(void);
    unsigned int        calculateMaxBytes(unsigned int bytes);
    unsigned int        calculateMaxChunks(unsigned int bytes);
    
    MemChunkDescriptor* findMappedAllocator(void* memory);
    MemChunkDescriptor* findSuitableAllocator(unsigned int bytes);
    
    void                mapChunks(MemChunkDescriptor* chunks, unsigned int count, unsigned char* memory);
    void                resetChunkOffsets(MemChunkDescriptor* allocator, unsigned int chunkCount, unsigned int byteCount);
    
  private:
    MemChunkDescriptor *m_pChunkCursor;
    MemChunkDescriptor *m_pFirstChunk;
    MemChunkDescriptor *m_pLastChunk;
    
    unsigned int        m_uiMinAllocSize;
    unsigned int        m_uiTotalChunks;
    unsigned int        m_uiChunkSize;
    
    unsigned int        m_uiTotalBytes;
    unsigned int        m_uiUsedBytes;
    
    unsigned int        m_peakMemory;
};

#endif
