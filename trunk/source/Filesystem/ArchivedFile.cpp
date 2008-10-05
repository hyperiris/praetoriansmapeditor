#include "ArchivedFile.h"

ArchivedFile::ArchivedFile(const char* n, unsigned char* b, unsigned int s)
{
  size = s;
  data = b;
  index = 0;
}

unsigned int ArchivedFile::read(void* dest, unsigned int bytes)
{
  bytes = (index + bytes) > size ? size - index : bytes;
  
  //if (bytes > 0)
  memcopy(dest, data + index, bytes);
  //memcpy(dest, data + index, bytes);
  index += bytes;
  
  return bytes;
}

unsigned int ArchivedFile::tell()
{
  return index;
}

void ArchivedFile::seek(unsigned int bytes, unsigned int from)
{
  index = SEEKA == from ? bytes
          : SEEKD == from ? index + bytes
          : SEEKU == from ? index - bytes
          : index;
}

void ArchivedFile::memcopy(void *dst, void *src, unsigned int bytes)
{
  char *pdst = (char*) dst;
  const char *psrc = (const char*) src;
  
  while (bytes--)
    *pdst++ = *psrc++;
}

void ArchivedFile::memcopyIntel(void* dst, void* src, unsigned int bytes)
{
  _asm
  {
    mov edi, dst
    mov esi, src
    mov ecx, bytes
    push ecx
    shr ecx, 2
    rep movsd
    pop ecx
    and ecx, 3
    rep movsb
  }
}

void ArchivedFile::reset()
{
  name.clear();
  index = 0;
  size  = 0;
  data  = 0;
}
