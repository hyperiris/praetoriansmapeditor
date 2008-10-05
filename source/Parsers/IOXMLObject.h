#ifndef XML_IO_H
#define XML_IO_H

#include "XMLParser.h"

class IOXMLObject : public NamedObject
{
  public:
    IOXMLObject(const char *ionameArg);
    virtual ~IOXMLObject() {}
    
    virtual bool loadXMLFile(const char *path);
    virtual bool loadXMLSettings(XMLElement *element)   = 0;
    
    virtual bool exportXMLSettings(std::ofstream &xmlFile) = 0;
    virtual bool exportXMLSettings(const char* xmlPath);
    
    bool isSuitable(XMLElement *element);
};

#endif
