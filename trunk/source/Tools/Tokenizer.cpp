#include "Tokenizer.h"
using namespace std;

Tokenizer::Tokenizer() : buffer(""), token(""), delimiter(" \t\v\n\r\f")
{
  currPos = buffer.begin();
}

Tokenizer::Tokenizer(const string& str, const string& delim) : buffer(str), token(""), delimiter(delim)
{
  currPos = buffer.begin();
}

void Tokenizer::set(const string& str, const string& delim)
{
  buffer = str;
  delimiter = delim;
  currPos = buffer.begin();
}

void Tokenizer::setString(const string& str)
{
  buffer = str;
  currPos = buffer.begin();
}

void Tokenizer::setDelimiter(const string& delim)
{
  delimiter = delim;
  currPos = buffer.begin();
}

string Tokenizer::next()
{
  if (buffer.size() <= 0) return "";
  
  token.clear();
  
  skipDelimiter();
  
  while (currPos != buffer.end() && !isDelimiter(*currPos))
  {
    token += *currPos;
    ++currPos;
  }
  return token;
}

void Tokenizer::skipDelimiter()
{
  while (currPos != buffer.end() && isDelimiter(*currPos))
    ++currPos;
}

bool Tokenizer::isDelimiter(char c)
{
  return (delimiter.find(c) != string::npos);
}

Tokenizer::~Tokenizer()
{
}