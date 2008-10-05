#ifndef ALPHA_ATTRIBUTES_H
#define ALPHA_ATTRIBUTES_H

#include "../Tools/Logger.h"

class AlphaAttributes : public IOXMLObject
{
  public:
    AlphaAttributes(int function = -1, float reference = -1.0f);
    AlphaAttributes(const AlphaAttributes &copy);
    AlphaAttributes &operator = (const AlphaAttributes &);
    
    bool loadXMLSettings(XMLElement *element);
    bool exportXMLSettings(ofstream &xmlFile);
    
    void  setAttributes(int function, float reference);
    
    void  setFunction(int function);
    int   getFunction();
    
    void  setReference(float reference);
    float getReference();
    
    void  apply();
    void  stop();
    
    bool  isSet();
    
  private:
    int   getXMLAlphaFunc(const String &value);
    float reference;
    int   function;
};
#endif