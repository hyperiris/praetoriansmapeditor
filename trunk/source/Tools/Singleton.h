#ifndef SINGLETON
#define SINGLETON

class Singleton
{
  public:
    static Singleton* getInstance();
    
  protected:
    Singleton();
    Singleton(const Singleton& copy);
    Singleton& operator = (const Singleton& right);
};

#endif