#ifndef COMMAND
#define COMMAND

#include "../Tools/String.h"

class XMLElement;

class Command
{
  public:
    Command() {}
    virtual ~Command() {}
    
  public:
    bool          loadXMLSettings(XMLElement *element);
    
    void          setSource(const char* s);
    void          setTarget(const char* t);
    const char*   getSource();
    const char*   getTarget();
    
  public:
    virtual void  execute();
    
  protected:
    String target;
    String source;
};

#endif
