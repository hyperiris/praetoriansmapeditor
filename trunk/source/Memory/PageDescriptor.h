#ifndef PAGEDESCRIPTOR
#define PAGEDESCRIPTOR

class PageDescriptor
{

  public:
    PageDescriptor();
    ~PageDescriptor();
    
  public:
    bool withinRange(unsigned long bytes);
    
  public:
    unsigned long addr;
    unsigned long cap;
    unsigned long size;
    bool base;
    bool locked;
    PageDescriptor* next;
};

#endif