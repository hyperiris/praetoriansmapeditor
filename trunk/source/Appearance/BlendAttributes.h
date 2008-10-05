#ifndef BLEND_ATTRIBUTES_H
#define BLEND_ATTRIBUTES_H

#include "../Tools/Logger.h"

class BlendAttributes : public IOXMLObject
{
  public:
    BlendAttributes(int source       = -1,
                    int destination  = -1);
                    
    BlendAttributes(const BlendAttributes &copy);
    BlendAttributes &operator = (const BlendAttributes &);
    
    bool loadXMLSettings(XMLElement *element);
    bool exportXMLSettings(ofstream &xmlFile);
    
    void setAttributes(int source, int destination);
    
    void setSource(int source);
    int getSource();
    
    void setDestination(int destination);
    int getDestination();
    
    void apply();
    void stop();
    
    bool isSet();
    
  private:
    int getXMLDestinationFactor(const String &value);
    int getXMLSourceFactor(const String &value);
    
  private:
    int destination, source;
};

#endif