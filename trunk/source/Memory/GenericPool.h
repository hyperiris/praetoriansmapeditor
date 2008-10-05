#ifndef GENERICPOOL
#define GENERICPOOL

#include <stack>

template <class T, int count, int min>
class GenericPool
{
  public:
    GenericPool()
    {
      for (unsigned int j = 0; j < count; j++)
        free.push(new T());
    }
    
    ~GenericPool()
    {
      destroy();
    }
    
    void destroy()
    {
      flush();
      
      while (!free.empty())
      {
        delete free.top();
        free.pop();
      }
    }
    
    T* allocate()
    {
      if (!recycled.empty())
      {
        T* r = recycled.top();
        recycled.pop();
        return r;
      }
      
      if (free.empty())
      {
        grow();
        return allocate();
      }
      
      inuse.push(free.top());
      free.pop();
      return inuse.top();
    }
    
    void recycle(T* r)
    {
      //fuck searching the inuse container
      recycled.push(r);
    }
    
    void flush()
    {
      while (!inuse.empty())
      {
        release(inuse.top());
        inuse.pop();
      }
      
      while (!recycled.empty())
        recycled.pop();
    }
    
    unsigned int getAllocatedCount()
    {
      return inuse.size();
    }
    
    unsigned int getFreeCount()
    {
      return free.size();
    }
    
  private:
    void release(T* resource)
    {
      free.push(resource);
    }
    
    void grow()
    {
      for (unsigned int j = 0; j < min; j++)
        free.push(new T());
    }
    
  private:
    std::stack <T*> inuse;
    std::stack <T*> free;
    std::stack <T*> recycled;
};

#endif
