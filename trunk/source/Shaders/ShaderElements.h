/*
 *Author: Abdul Bezrati
 *Email : abezrati@hotmail.com
 */

#ifndef SHADERSELEMENT_H
#define SHADERSELEMENT_H

#include "../Tools/NamedObject.h"

template <int size, class T>

class ShaderElements : public NamedObject
{
  private:
    bool  localElements;
    T     elements[size];
    
  public:
    ShaderElements(const String &nameArg,
                   const T* newElements = NULL) : NamedObject(nameArg)
    {
      if (!newElements)
        memset(elements, 0, sizeof(T)*size);
      else
        memcpy(elements, newElements, sizeof(T)*size);
      localElements = false;
    }
    
    ShaderElements(const char* nameArg  = NULL,
                   const T* newElements = NULL) : NamedObject(nameArg)
    {
      if (!newElements)
        memset(elements, 0, sizeof(T)*size);
      else
        memcpy(elements, newElements, sizeof(T)*size);
      localElements = false;
    }
    
    ShaderElements(const ShaderElements &copy)
    {
      if (this != &copy)
        this->operator =(copy);
    };
    
    ShaderElements &operator = (const ShaderElements &copy)
    {
      if (this != &copy)
      {
        name          = copy.name;
        localElements = copy.localElements;
        memcpy(elements, copy.elements, size*sizeof(T));
        
      }
      return *this;
    };
    
    void setLocal(bool loc)
    {
      localElements = loc;
    }
    
    void setElements(const T* elems)
    {
      if (!elems)
        return;
      memcpy(elements, elems, sizeof(T)*size);
    }
    
    operator const T*() const
    {
      return elements;
    }
    operator       T*()
    {
      return elements;
    }
    bool           locals()
    {
      return localElements;
    }
    
    const int      getElementsCount() const
    {
      return size;
    }
    const T*       getElements()      const
    {
      return elements;
    }
};

typedef ShaderElements<1, float> ShaderElements1f;
typedef ShaderElements<1, int>   ShaderElements1i;

typedef ShaderElements<2, float> ShaderElements2f;
typedef ShaderElements<2, int>   ShaderElements2i;

typedef ShaderElements<3, float> ShaderElements3f;
typedef ShaderElements<3, int>   ShaderElements3i;

typedef ShaderElements<4, float> ShaderElements4f;
typedef ShaderElements<4, int>   ShaderElements4i;

typedef ShaderElements<16, float> ShaderElements16f;
typedef ShaderElements<16, int>   ShaderElements16i;

#endif