#ifndef TOKENIZER
#define TOKENIZER

#include <string>

class Tokenizer
{
  public:
    Tokenizer();
    Tokenizer(const std::string& str, const std::string& delim = " \t\v\n\r\f");
    ~Tokenizer();
    
    void set(const std::string& str, const std::string& delim = " \t\v\n\r\f");
    void setString(const std::string& str);
    void setDelimiter(const std::string& delim);
    
    std::string next();
    
  private:
    void skipDelimiter();
    bool isDelimiter(char c);
    
  private:
    std::string buffer;
    std::string token;
    std::string delimiter;
    std::string::const_iterator currPos;
    
};

#endif
