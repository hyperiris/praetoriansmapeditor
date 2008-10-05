#ifndef MEMCHUNKDESCRIPTOR
#define MEMCHUNKDESCRIPTOR

class MemChunkDescriptor
{
  public:
    MemChunkDescriptor();
    ~MemChunkDescriptor();
    
  public:
    //unsigned char*      Data();
    //void                Data(unsigned char* address);
    //
    //MemChunkDescriptor* NextChunk();
    //void                NextChunk(MemChunkDescriptor* chunk);
    //
    //unsigned int        Segment();
    //void                Segment(unsigned int segment);
    //
    //unsigned int        Offset();
    //void                Offset(unsigned int offset);
    //
    //bool                BaseChunk();
    //void                BaseChunk(bool value);
    //
    void                reset();
    
    //private:
    unsigned int        segment;
    unsigned int        offset;
    bool                baseChunk;
    unsigned char*      data;
    MemChunkDescriptor* nextChunk;
};

#endif
