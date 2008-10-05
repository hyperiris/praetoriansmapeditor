#include "BlockMemory.h"
#include "PageFile.h"
#include "MemChunkDescriptor.h"
#include "../Kernel/Gateway.h"
#include <fstream>
#include <math.h>
#include <stack>

using namespace std;

#define FREED_MEM_VALUE 0x00
#define ALLOC_MEM_VALUE 0xff

BlockMemory::BlockMemory():
    m_pLastChunk(0),
    m_pFirstChunk(0),
    m_pChunkCursor(0),
    m_uiUsedBytes(0),
    m_uiTotalBytes(0),
    m_uiTotalChunks(0),
    m_uiChunkSize(128),
    m_uiMinAllocSize(512),
    m_peakMemory(0)
{}

BlockMemory::BlockMemory(unsigned int chunkSize,
                         unsigned int allocChunks):
    m_pLastChunk(0),
    m_pFirstChunk(0),
    m_pChunkCursor(0),
    m_uiUsedBytes(0),
    m_uiTotalBytes(0),
    m_uiTotalChunks(0),
    m_uiChunkSize(chunkSize),
    m_uiMinAllocSize(chunkSize*allocChunks),
    m_peakMemory(0)
{
}

bool BlockMemory::initialize(unsigned int bytes)
{
  MemChunkDescriptor *pMemDescriptors = 0;
  MemChunkDescriptor *pChunk = 0;
  unsigned int iChunkCount = 0;
  unsigned int iByteCount = 0;
  unsigned char *pMemRegion = 0;
  
  if (m_uiMinAllocSize > bytes)
    bytes = m_uiMinAllocSize;
    
  iChunkCount = calculateMaxChunks(bytes);
  
  iByteCount = iChunkCount * m_uiChunkSize;
  
  m_uiTotalBytes += iByteCount;
  
  m_uiTotalChunks += iChunkCount;
  
  pMemDescriptors = new MemChunkDescriptor[iChunkCount];
  
  if (!(pMemRegion = (unsigned char*) Gateway::getPageFile()->commit(iByteCount)))
    return false;
    
  Gateway::getPageFile()->lock(pMemRegion, true);
  
  if (!m_pFirstChunk)
  {
    m_pFirstChunk = pMemDescriptors;
    m_pLastChunk = m_pFirstChunk;
    m_pChunkCursor = m_pFirstChunk;
  }
  
  mapChunks(pMemDescriptors, iChunkCount, pMemRegion);
  
  resetChunkOffsets(pMemDescriptors, iChunkCount, iByteCount);
  
  m_pChunkCursor = pMemDescriptors;
  
  return true;
}

void* BlockMemory::aquire(unsigned int bytes)
{
  unsigned int allocSize = 0;
  MemChunkDescriptor *pAvailableChunk = 0;
  
  while (!pAvailableChunk)
  {
    pAvailableChunk = findSuitableAllocator(bytes);
    
    if (!pAvailableChunk)
      return 0;
      
    //{
    //  allocSize = bytes < m_uiMinAllocSize ? m_uiMinAllocSize : bytes;
    //  initialize(bytes);
    //}
  }
  
  m_peakMemory = m_peakMemory < bytes ? bytes : m_peakMemory;
  
  return (void*) pAvailableChunk->data;
}

bool BlockMemory::release(void* memory, bool setval)
{
  MemChunkDescriptor *pChunk = findMappedAllocator(memory);
  
  if (pChunk)
  {
    freeRegion(pChunk, setval);
    m_pChunkCursor = pChunk;
  }
  
  //m_pChunkCursor = pChunk;
  return (pChunk != 0);
}

void BlockMemory::mapChunks(MemChunkDescriptor* chunks, unsigned int count, unsigned char* memory)
{
  MemChunkDescriptor *pChunk = 0;
  
  for (unsigned int i = 0; i < count; i++)
  {
    pChunk = &chunks[i];
    pChunk->data = &memory[i * m_uiChunkSize];
    pChunk->baseChunk = (i == 0) ? true : false;
    
    m_pLastChunk->nextChunk = pChunk;
    m_pLastChunk = pChunk;
  }
}

void BlockMemory::resetChunkOffsets(MemChunkDescriptor* allocator, unsigned int count, unsigned int bytes)
{
  MemChunkDescriptor *pChunk = allocator;
  
  for (unsigned int i = 0; i < count; i++)
  {
    pChunk->segment = bytes - (i * m_uiChunkSize);
    pChunk->offset = 0;
    pChunk = pChunk->nextChunk;
  }
}

MemChunkDescriptor* BlockMemory::findSuitableAllocator(unsigned int bytes)
{
  if (bytes > m_uiTotalBytes - m_uiUsedBytes)
    return 0;
    
  MemChunkDescriptor *allocator = 0;
  
  MemChunkDescriptor *marker = 0;
  
  unsigned int range = 0;
  
  for (unsigned int i = 0; i < m_uiTotalChunks; i++)
  {
    if (!m_pChunkCursor)
      m_pChunkCursor = m_pFirstChunk;
      
    if (bytes <= m_pChunkCursor->segment)
    {
      marker = m_pChunkCursor;
      range = m_pChunkCursor->segment;
      
      while (marker)
      {
        if (marker->offset > 0)
          break;
          
        range -= m_uiChunkSize;
        
        marker = marker->nextChunk;
      }
      
      if (bytes <= m_pChunkCursor->segment - range)
      {
        allocator = m_pChunkCursor;
        allocator->offset = bytes;
        
        m_uiUsedBytes += calculateMaxBytes(bytes);
        
        moveToNextRegion();
        break;
      }
    }
    
    moveToNextRegion();
  }
  
  return allocator;
}

void BlockMemory::moveToNextRegion()
{
  unsigned int chunksToSkip = calculateMaxChunks(m_pChunkCursor->offset);
  
  if (chunksToSkip == 0)
    chunksToSkip = 1;
    
  for (unsigned int i = 0; i < chunksToSkip; i++)
    m_pChunkCursor = m_pChunkCursor->nextChunk;
}

MemChunkDescriptor* BlockMemory::findMappedAllocator(void* memory)
{
  MemChunkDescriptor *pChunk = m_pFirstChunk;
  
  while (pChunk)
  {
    if (pChunk->data == ((unsigned char*)memory))
      return pChunk;
      
    pChunk = pChunk->nextChunk;
  }
  
  //return pChunk;
  return 0;
}

void BlockMemory::freeRegion(MemChunkDescriptor* allocator, bool setval)
{
  MemChunkDescriptor *pChunk = allocator;
  unsigned int linkCount = calculateMaxChunks(pChunk->offset);
  
  if (setval)
    std::fill(pChunk->data, pChunk->data + m_uiChunkSize * linkCount, FREED_MEM_VALUE);
    
  m_uiUsedBytes -= m_uiChunkSize * linkCount;
  
  pChunk->offset = 0;
}

unsigned int BlockMemory::calculateMaxChunks(unsigned int bytes)
{
  //return (unsigned int) ceil((float)bytes/m_uiChunkSize);
  return (unsigned int)(bytes % m_uiChunkSize) == 0 ? bytes / m_uiChunkSize : (bytes / m_uiChunkSize) + 1;
}

unsigned int BlockMemory::calculateMaxBytes(unsigned int bytes)
{
  return calculateMaxChunks(bytes)*m_uiChunkSize;
}

unsigned int BlockMemory::calculateRealUsedBytes(void)
{
  MemChunkDescriptor *pChunk = m_pFirstChunk;
  unsigned int usedBytes = 0;
  
  while (pChunk)
  {
    usedBytes += pChunk->offset;
    pChunk = pChunk->nextChunk;
  }
  
  return usedBytes;
}

void BlockMemory::printMemoryBlock(const char* fileName)
{
  ofstream outputFile(fileName, ios_base::binary);
  MemChunkDescriptor *pChunk = m_pFirstChunk;
  
  while (pChunk)
  {
    if (outputFile.good())
      outputFile.write((char*)pChunk->data, (std::streamsize)m_uiChunkSize);
      
    pChunk = pChunk->nextChunk;
  }
  
  outputFile.close();
}

void BlockMemory::printMemoryProfile(const char* fileName)
{
  ofstream outputFile(fileName, ios_base::app);
  
  if (outputFile.good())
  {
    MemChunkDescriptor *pChunk = m_pFirstChunk;
    unsigned int usedBytes = calculateRealUsedBytes();
    unsigned int memBlocks = 0;
    
    while (pChunk)
    {
      if (pChunk->baseChunk)
        memBlocks++;
        
      pChunk = pChunk->nextChunk;
    }
    
    outputFile << "Memory Profile\n\n"
    << "------------------------------\n"
    << "total memory blocks: " << memBlocks << "\n"
    << "total allocated memory: " << m_uiTotalBytes << "\n"
    << "total used memory: " << usedBytes << "\n"
    << "total unused memory: " << m_uiTotalBytes - usedBytes << "\n"
    << "total peak memory: " << m_peakMemory << "\n"
    << "total chunks: " << m_uiTotalChunks << "\n"
    << "chunk size: " << m_uiChunkSize << "\n"
    << "chunk footprint: " << sizeof(MemChunkDescriptor) << "\n"
    << "total chunk footprint: " << sizeof(MemChunkDescriptor)*m_uiTotalChunks << "\n\n";
  }
  
  outputFile.close();
}

void BlockMemory::deallocateMemory()
{
  MemChunkDescriptor *pChunk = 0;
  
  unsigned char *pData = 0;
  
  stack <unsigned char*> memBlocks;
  
  pChunk = m_pFirstChunk;
  
  while (pChunk)
  {
    if (pChunk->baseChunk)
      memBlocks.push(pChunk->data);
      
    pChunk = pChunk->nextChunk;
  }
  
  while (!memBlocks.empty())
  {
    pData = memBlocks.top();
    Gateway::getPageFile()->decommit(pData);
    memBlocks.pop();
  }
}

void BlockMemory::deallocateChunks()
{
  MemChunkDescriptor *pChunk = 0;
  
  MemChunkDescriptor *pBaseChunk = 0;
  
  stack <MemChunkDescriptor*> baseChunks;
  
  pChunk = m_pFirstChunk;
  
  while (pChunk)
  {
    if (pChunk->baseChunk)
      baseChunks.push(pChunk);
      
    pChunk = pChunk->nextChunk;
  }
  
  while (!baseChunks.empty())
  {
    pBaseChunk = baseChunks.top();
    baseChunks.pop();
    delete [] pBaseChunk;
  }
  
  m_pFirstChunk = 0;
}

bool BlockMemory::withinBounds(void* addr)
{
  return ((unsigned long)addr >= getStartAddress() && (unsigned long)addr < getStartAddress() + m_uiTotalBytes);
}

unsigned long BlockMemory::getStartAddress()
{
  return (unsigned long) m_pFirstChunk->data;
}

void BlockMemory::setChunkSize(unsigned int bytes)
{
  m_uiChunkSize = bytes;
}

void BlockMemory::setMinAllocSize(unsigned int bytes)
{
  m_uiMinAllocSize = bytes;
}

unsigned int BlockMemory::getTotalSize()
{
  return m_uiTotalBytes;
}

unsigned int BlockMemory::getUsedSize()
{
  return m_uiUsedBytes;
}

void BlockMemory::reset()
{
  Gateway::getPageFile()->reset(m_pFirstChunk->data);
  resetChunkOffsets(m_pFirstChunk, m_uiTotalChunks, m_uiTotalChunks * m_uiChunkSize);
  m_pChunkCursor = m_pFirstChunk;
  m_uiUsedBytes = 0;
  m_peakMemory = 0;
  //m_uiTotalBytes = 0;
  //m_uiTotalChunks = 0;
}

void BlockMemory::destroy()
{
  if (!m_pFirstChunk)
    return;
    
  deallocateMemory();
  deallocateChunks();
}

BlockMemory::~BlockMemory()
{
  destroy();
}
