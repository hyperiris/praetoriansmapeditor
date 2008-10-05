#ifndef VALREADER_H_

// NOTE: Due to weird include dependencies, this file includes nothing
// itself.  If you wish to use it, make sure you include ocval.h
// first.

// A class to help read "human-readable" strings of Vals (Tabs, Arrs,
// numbers, Numeric arrays).  Format is straight-forward:
//
//   ValReader v(" { 'a':'nice', 'table':1 }");
//   Tab t;
//   v.expectTab(t);
//
// From Files (streams):
//   StreamValReader sv(istream& is);
//   Tab t;
//   sv.expectTab(t);
//
// The constructors of Tab and Arr have been augmented so as to allow
// easier construction:
//
//   Tab t = "{ 'a':1, 'b':2, 'c':3, 'list':[1,2,3] }";
//   Arr a = "[ 'a', 2, 3.3, (1+2j), { 'empty':'ish' } ]";

#include "ocstringtools.h"

// Abstract base class: All the code for parsing the letters one by
// one is here.  The code for actually getting the letters (from a
// string, stream, etc.) defers to the derived class.

class ValReaderA
{

  public:
  
    // Expect any number (including complex)
    void expectNumber(Val& n)
    {
      consumeWS_();
      int c=peekChar_();
      if (c=='(')
      {
        expectComplex_(n);
        return;
      }
      
      // Get the integer part, if any, of the number
      Str integer_part = getSignedDigits_('.');
      
      // Get the fractional part, if any
      Str fractional_part;
      c = peekChar_();
      if (c=='.')
      {
        c = getChar_(); // consume the '.'
        fractional_part = "."+getDigits_();
        if (fractional_part.length()==1)
        {
          const int i_len = integer_part.length();
          if (i_len==0 || (i_len>0 && !isdigit(integer_part[i_len-1])))
          {
            syntaxError_("Expecting some digits after a decimal point");
          }
        }
        c = peekChar_();
      }
      
      // Get the exponent part, if any
      Str exponent_part;
      if (c=='e' || c=='E')
      {
        c = getChar_();  // consume the 'e'
        exponent_part = "e"+getSignedDigits_(' ');
        if (exponent_part.length()==1) // only an e
          syntaxError_("Expected '+', '-' or digits after an exponent");
        c = peekChar_();
      }
      
      // At this point, we are (mostly) finished with the number, and we
      // have to build the proper type of number.
      if (fractional_part.length()>0 || exponent_part.length()>0)
      {
        // If we have either a fractional part or an exponential part,
        // then we have a floating point number
        Str stringized_number = integer_part+fractional_part+exponent_part;
        Val inside = stringized_number;
        real_8 num = inside; // Convert out of Val to change from string
        n = num;
        return;
      }
      
      // Well, no fractional part or exponential.  There had better be
      // some digits!
      if (integer_part.length()==0 ||
          (integer_part.length()>0 &&
           !isdigit(integer_part[integer_part.length()-1])))
        syntaxError_("Expected some digits for a number");
        
      c=peekChar_();
      Val v = integer_part;
      if (c=='l' || c=='L')   // Okay, it's a long
      {
        getChar_();  // consume long
        if (integer_part[0]=='-')
        {
          int_8 long_int = v;
          n = long_int;
          return;
        }
        else
        {
          int_u8 long_int = v;
          n = long_int;
          return;
        }
      }
      else   // plain integer
      {
        int_4 plain_int = v;
        n = plain_int;
        return;
      }
    }
    
    // Read a string from the current place in the input
    void expectStr(Str& s)
    {
      consumeWS_();
      
      char quote_mark = peekNWSChar_();
      if (quote_mark!='\'' && quote_mark!='"')
      {
        syntaxError_("A string needs to start with ' or \"");
      }
      
      expect_(quote_mark); // Start quote
      
      // Read string, keeping all escapes, and let DeImage handle escapes
      Array<char> a(80);
      for (int c=getChar_(); c!=quote_mark; c=getChar_())
      {
        if (c==EOF) syntaxError_("Unexpected EOF inside of string");
        a.append(c);
        if (c=='\\')   // escape sequence
        {
          int next = getChar_(); // Avoid '
          if (next==EOF) syntaxError_("Unexpected EOF inside of string");
          a.append(next);
        }
      }
      string temp = string(a.data(), a.length());
      string ss = DeImage(temp, false); // Do escapes
      s = Str(ss.data(), ss.length());
    }
    
    // Expect Table on the input
    void expectTab(Tab& table)
    {
      expect_('{');
      
      // Special case, empty Table
      char peek = peekNWSChar_();
      if (peek!='}')
      {
      
        for (;;)   // Continue getting key value pairs
        {
          Val key;
          expectAnything(key);
          expect_(':');
          Val value;
          expectAnything(value);
          
          table[key] = value;
          
          char peek = peekNWSChar_();
          if (peek==',')
          {
            expect_(',');         // Another k-v pair, grab comma and move on
            continue;
          }
          else if (peek=='}')   // End of table
          {
            break;
          }
          else
          {
            syntaxError_("Expecting a '}' or ',' for table");
          }
        }
      }
      expect_('}');
    }
    
    // Expect an Arr
    void expectArr(Arr& a)
    {
      expect_('[');
      
      // Special case, empty list
      char peek = peekNWSChar_();
      if (peek!=']')
      {
      
        // Continue getting each item one at a time
        while (1)
        {
          a.append(Val());
          expectAnything(a[a.length()-1]);
          
          char peek = peekNWSChar_();
          if (peek==',')
          {
            expect_(',');
            continue;
          }
          else if (peek==']')
          {
            break;
          }
        }
      }
      expect_(']');
    }
    
    // Read in a Numeric Array
    void expectNumericArray(Val& a)
    {
      consumeWS_();
      expect_("array([");
      a = Array<int_4>(); // By default, int array
      
      // Special case, empty list
      char peek = peekNWSChar_();
      if (peek!=']')
      {
      
        Val n;
        expectNumber(n);
        
        if (n.tag=='l') readNumericArray_<int_4>(a, n);
        else if (n.tag=='D') readNumericArray_<complex_16>(a, n);
        else if (n.tag=='d') readNumericArray_<real_8>(a, n);
        else throw logic_error("Only support Numeric arrays of cx16, real_8, and int_4");
      }
      expect_("])");
    }
    
    
    // We could be starting ANYTHING ... we have to dispatch to the
    // proper thing
    void expectAnything(Val& v)
    {
      char c = peekNWSChar_();
      switch (c)
      {
        case '{' :
        {
          v = Tab();
          Tab& table = v;
          expectTab(table);
          break;
        }
        case '[' :
        {
          v = Arr();
          Arr& arr = v;
          expectArr(arr);
          break;
        }
        case '\'':
        case '"' :
        {
          Str s = "";
          expectStr(s);
          v=s;
          break;
        }
        case 'a' :
        {
          expectNumericArray(v);
          break;
        }
        case 'N' :
        {
          v = Val();
          expect_("None");
          break;
        }
        case 'T' :
        {
          v = true;
          expect_("True");
          break;
        }
        case 'F' :
        {
          v = false;
          expect_("False");
          break;
        }
        default:
        {
          v = Val();
          expectNumber(v);
          break;
        }
      }
      
    }
    
  protected:
  
    // Helper Methods
    
    void syntaxError_(const string& s)
    {
      throw logic_error(s);
    }
    
    void expect_(char expected)
    {
      // Single character token
      int get      = getNWSChar_();
      if (get!=expected)
      {
        string get_string, expected_string;
        if (get==EOF) get_string="EOF";
        else get_string=get;
        expected_string = expected;
        syntaxError_("Expected:'"+expected_string+"', but saw '"+get_string+"' "
                     "on input");
      }
    }
    
    void expect_(const string& s)
    {
      for (int ii=0; ii<s.length(); ii++)
        expect_(s[ii]);
    }
    
    // Expect a complex number:  assumes it will have form (#+#j)
    void expectComplex_(Val& n)
    {
      complex_16 result;
      expect_('(');
      Val real_part, imag_part;
      expectNumber(real_part);
      char c=peekChar_();
      if (c=='+' || c=='-')
      {
        expectNumber(imag_part);
      }
      expect_("j)");
      
      result.re = real_part;
      result.im = imag_part;
      n = result;
    }
    
    // From current point of input, consume all digits until
    // next non-digit.  Leaves non-digit on input
    Str getDigits_()
    {
      Str digits;
      while (1)
      {
        int c = peekChar_();
        if (c==EOF)
          break;
        if (isdigit(c))
        {
          digits+=char(c);
          getChar_();
        }
        else
          break;
      }
      return digits;
    }
    
    // From current point in input, get the signed sequence of
    // digits
    Str getSignedDigits_(char next_marker)
    {
      // Get the sign of the number, if any
      char c=peekChar_();
      Str sign;
      if (c=='+'||c=='-')
      {
        sign = c;
        getChar_();    // consume the sign
        c=peekChar_(); // .. and see what's next
      }
      
      // Assertion: passed sign, now should see number or .
      if (!isdigit(c)&&c!=next_marker)
      {
        char s[2];
        s[0] = next_marker;
        s[1] = '\0';
        syntaxError_("Expected numeric digit or '"+string(s)+"' for number");
      }
      
      Str digits = sign+getDigits_();
      return digits;
    }
    
    template <class T>
    void readNumericArray_(Val& arr, const Val& first_value)
    {
      arr = Array<T>();  // initialize the array
      Array<T>& a = arr; // .. then a reference to it
      T v = first_value;
      a.append(v);
      
      // Continue getting each item one at a time
      while (1)
      {
        char peek = peekNWSChar_();
        if (peek==',')
        {
          expect_(',');
          continue;
        }
        else if (peek==']')
        {
          break;
        }
        
        Val n;
        expectNumber(n);
        T v = n;
        a.append(v);
      }
    }
    
    // A derived class implements these methods to read characters from
    // some input source.
    virtual char getNWSChar_() = 0;
    virtual char peekNWSChar_() = 0;
    virtual int getChar_() = 0;
    virtual int peekChar_() = 0;
    virtual int consumeWS_() = 0;
    
}; // ValReaderA



// The ValReader reads Vals from strings.  The most common usage is to
// use this to read literal constants for the constructors of Tab and
// Arr.
class ValReader : public ValReaderA
{

  public:
  
    ValReader(Array<char>& a) :
        buffer_(a),
        current_(0)
    { }
    
    ValReader(const char* s) :
        current_(0)
    {
      int len = strlen(s);
      buffer_.expandTo(len);
      memcpy(buffer_.data(), s, len);
    }
    
    
  protected:
  
    // ///// Data Members
    Array<char> buffer_;
    int current_;
    
    // Return the index of the next Non-White Space character.
    // A comment starts with a # and ends with a \n, and counts
    // as white space
    int indexOfNextNWSChar_()
    {
      const int len=buffer_.length();
      int cur = current_;
      if (cur==len) return EOF;
      // Look for WS or comments that start with #
      bool comment_mode = false;
      for (; cur<len; cur++)
      {
        if (comment_mode)
        {
          if (buffer_[cur]=='\n')
          {
            comment_mode = false;
          }
          continue;
        }
        else
        {
          if (isspace(buffer_[cur])) continue;
          else if (buffer_[cur]=='#')
          {
            comment_mode = true;
            continue;
          }
          else
          {
            break;
          }
        }
      }
      return cur;
    }
    
    // Get a the next non-white character from input
    virtual char getNWSChar_()
    {
      int index = indexOfNextNWSChar_();
      if (index==EOF) return EOF;
      current_ = index;
      // First non comment
      return buffer_[current_++];
    }
    
    virtual char peekNWSChar_()
    {
      int index = indexOfNextNWSChar_();
      if (index==EOF) return EOF;
      return buffer_[index];
    }
    
    virtual int getChar_()
    {
      const int len=buffer_.length();
      if (current_==len) return EOF;
      return buffer_[current_++];
    }
    
    virtual int peekChar_()
    {
      const int len=buffer_.length();
      if (current_==len) return EOF;
      return buffer_[current_];
    }
    
    virtual int consumeWS_()
    {
      int index = indexOfNextNWSChar_();
      if (index==EOF) return EOF;
      current_ = index;
      return buffer_[index];
    }
    
}; // ValReader

#include "occircularbuffer.h"

// Read a val from a stream
class StreamValReader : public ValReaderA
{

  public:
  
    // Open the given file, and attempt to read Vals out of it
    StreamValReader(istream& is) :
        is_(is),
        cached_(132, true)
    { }
    
  protected:
  
    istream& is_;
    CircularBuffer<int> cached_;
    
    // This routines buffers data up until the next Non-White space
    // character, ands returns what the next ws char is _WITHOUT GETTING
    // IT_.  It returns "peek_ahead" to indicate how many characters
    // into the stream you need to be to get it.
    char peekIntoNextNWSChar_(int& peek_ahead)
    {
      peek_ahead = 0;  // This marks how many characters into the stream
      // we need to consume
      bool start_comment = false;
      while (1)
      {
        // Peek at a character either from the cache, or grab a new char
        // of the stream and cache it "peeking" at it.
        int c;
        if (peek_ahead >= cached_.length())
        {
          c = is_.get();
          cached_.put(c);
        }
        else
        {
          c = cached_.peek(peek_ahead);
        }
        
        // Look at each character individually
        if (c==EOF)
        {
          // We never consume the EOF once we've seen it
          return c;
        }
        else if (start_comment)
        {
          peek_ahead++;
          start_comment = (c!='\n');
          continue;
        }
        else if (c=='#')
        {
          peek_ahead++;
          start_comment = true;
          continue;
        }
        else if (isspace(c))   // white and comments
        {
          peek_ahead++;
          continue;
        }
        else
        {
          return c;
        }
      }
    }
    
    virtual char getNWSChar_()
    {
      int peek_ahead;
      int c = peekIntoNextNWSChar_(peek_ahead);
      cached_.consume(peek_ahead);
      return getChar_();
    }
    
    virtual char peekNWSChar_()
    {
      int peek_ahead;
      int c = peekIntoNextNWSChar_(peek_ahead);
      return c;
    }
    
    virtual int getChar_()
    {
      if (cached_.empty())
      {
        return is_.get();
      }
      else
      {
        return cached_.get();
      }
    }
    
    virtual int peekChar_()
    {
      if (cached_.empty())
      {
        int c = is_.get();
        cached_.put(c);
      }
      return cached_.peek();
    }
    
    virtual int consumeWS_()
    {
      int peek_ahead;
      int c = peekIntoNextNWSChar_(peek_ahead);
      cached_.consume(peek_ahead);
      return c;
    }
    
    
}; // StreamValReader


inline void ReadTabFromFile(const string& filename, Tab& t)
{
  ifstream ifs(filename.c_str());
  StreamValReader sv(ifs);
  sv.expectTab(t);
}


inline void WriteTabToFile(const Tab& t, const string& filename)
{
  ofstream ofs(filename.c_str());
  t.prettyPrint(ofs);
}



#define VALREADER_H_
#endif // VALREADER_H_


