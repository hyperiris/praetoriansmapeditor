#include "PageDescriptor.h"

PageDescriptor::PageDescriptor()
{
  base = false;
  locked = false;
  size = 0;
  next = 0;
  addr = 0;
  cap  = 0;
}

bool PageDescriptor::withinRange(unsigned long bytes)
{
  PageDescriptor* pMark = this;
  bool fit = false;
  
  while (pMark)
  {
    fit = (bytes <= cap - pMark->cap);
    
    if (pMark->size != 0 || fit)
      break;
      
    pMark = pMark->next;
  }
  
  return fit;
}

PageDescriptor::~PageDescriptor()
{
}
