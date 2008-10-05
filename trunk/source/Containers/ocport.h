#ifndef OCPORT_H_

// This file exists so you can make a few portability decisions.
// Here are the current portability decisions you need to decide
// to define
// #define OC_NEEDS_BOOL
#define OC_NEW_STYLE_INCLUDES
// #define OC_NO_STD_CHAR
// #define OC_LONG_INT_IS_64BIT
// #define OC_POINTERS_8_BYTES || #define OC_POINTERS_4_BYTES


// They are all turned off by default, and your code can include it
// directly and turn on the portability features you need with
// -D on the command line.
//
// For example, on Solaris (CC: WorkShop Compilers 4.2) we invoke:
//  CC -I/home/opencontainers/include -D OC_NEEDS_BOOL string_test.cc
// On DECUnix (Compaq C++ V6.5-033) we invoke
//  cxx -I /home/opencontainers/include -D OC_NEW_STYLE_INCLUDES array_test.cc

// Common
#include <stddef.h>      // For size_t
#include <stdlib.h>      // For malloc/free
#include <limits.h>

// DECISION:
// Most compilers support bools, but not all
#if defined(OC_NEEDS_BOOL)
typedef int bool;
enum { false=0, true=1 };
#endif

// DECISION:
// Do we support X-Midas?  There are a few typedefs that typeclash
// when using X-MIdas, so we have to turn them off (mostly in complex)
// #define OC_SUPPORT_XM

// DECISION:
// What kind of includes do we use?  #include <iostream.h>
// or the newer style #include <stream>  (without the .h)

#if defined(OC_NEW_STYLE_INCLUDES)

#  include <iostream>    // For a few streams
#  include <new>         // For placement new
#  include <sstream>
#  include <fstream>

using namespace std;
typedef istringstream istrstream;  // So we can use istrstream in both worlds

#else  // defined(OC_NEW_STYLE_INCLUDES)

#  include <iostream.h>    // For a few streams
#  include <new.h>         // For placement new
#  include <strstream.h>
#  include <fstream.h>

#endif // defined(OC_NEW_STYLE_INCLUDES)



// ///////////////////////////////////////////// Type definitions

// On platforms that do not allow overloading on "char" and "signed char"
// we typedef to "char".
// DECISION:
#ifdef OC_NO_STD_CHAR
typedef char          int_1;                  // int_1, int_u1
#else
typedef signed char   int_1;
#endif
typedef unsigned char   int_u1;

typedef short           int_2;                  // int_2, int_u2
typedef unsigned short  int_u2;
typedef int             int_4;                  // int_4, int_u4
typedef unsigned int    int_u4;

// DECISION:
#if defined(OC_LONG_INT_IS_64BIT)
typedef long          int_8;
typedef unsigned long int_u8;
#elif defined(VMS_)
typedef int64         int_8;
typedef uint64        int_u8;
#elif defined(WIN32_)
typedef __int64               int_8;
typedef unsigned __int64      int_u8;
#else
typedef long long             int_8;
typedef unsigned long long    int_u8;
#endif

typedef float   real_4;                         // real_4
typedef double  real_8;                         // real_8


// DECISION:
// How big are your pointers?
#define OC_BYTES_IN_POINTER 4
// #define OC_BYTES_IN_POINTER 8


// Used for both AVLTreeT and AVLHashT classes
#if OC_BYTES_IN_POINTER==8
typedef int_u8 AVLP;
#   define AVL_EMPTY_MASK 0xfffffffffffffffe
#elif OC_BYTES_IN_POINTER==4
typedef int_u4 AVLP;
#   define AVL_EMPTY_MASK 0xfffffffe
#else
#   error 666 "You need to define OC_BYTES_IN_POINTER, the number of bytes in a pointer"
#endif

// DECISION: Do we use STL or OC strings? Note we have to do this
// before the exceptions, because they need to know what a string is.
#if defined(OC_USE_OC_STRING)
# include "ocstring.h"
typedef OCString string;
#else
#include <string>
#endif

// DECISION:
// Do we use STL exceptions?
#if defined(OC_USE_OC_EXCEPTIONS)
# include "ocexception.h"
#else
# include <stdexcept>
#endif

// DECISION:
// Is everything inlined, or do we factor out into .h and .cc files?
#if defined(OC_FACTOR_INTO_H_AND_CC)
# define OC_INLINE
#else
# define OC_INLINE inline
#endif


// A very standard way to Stringize items
template <class T>
inline string Stringize(const T& v)
{
#if defined(OC_NEW_STYLE_INCLUDES)
  ostringstream os;
  os << v;
  string ret_val = os.str();
  return ret_val;
#else
  ostrstream os;
  os << v << ends; // operator<< will do precision correctly
  string ret_val = string(os.str());
  delete [] os.str(); // Clean up ostrstream's mess
  return ret_val;
#endif
}

template <>
inline string Stringize<string> (const string& s)
{
  return s;
}

#if defined(OC_NEW_STYLE_INCLUDES)
#include <algorithm>
using std::swap;

#else
template <class T>
void swap(T& t1, T& t2)
{
  T temp = t1;
  t1 = t2;
  t2 = temp;
}
#endif


#define OCPORT_H_
#endif // OCPORT_H_
