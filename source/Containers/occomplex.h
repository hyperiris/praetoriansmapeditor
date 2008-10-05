#ifndef OCCOMPLEX_H_

#include "ocport.h"

// A templatized class for most of the complex math.  The complex_8
// and complex_16 are simply typedefs for cx_t<real_4> and
// cx_t<real_8), respectively.

template <class T> class cx_union_t;

template <class T>
struct cx_t
{

  T re, im;
  explicit cx_t(const T& real=0, const T& imag=0):re(real),im(imag) { }
  
  // use default constructor, default op=, default destructor
  
  
  cx_t<T>& operator+= (const cx_t<T>& r)
  {
    re+=r.re;
    im+=r.im;
    return *this;
  }
  cx_t<T>& operator+= (const T& scalar)
  {
    re+=scalar;
    return *this;
  }
  cx_t<T>& operator-= (const cx_t<T>& r)
  {
    re-=r.re;
    im-=r.im;
    return *this;
  }
  cx_t<T>& operator-= (const T& scalar)
  {
    re-=scalar;
    return *this;
  }
  
  cx_t<T>& operator*= (const cx_t<T>& r)
  {
    T t1=re*r.re-im*r.im;
    T t2=im=re*r.im+im*r.re;
    re = t1;
    im = t2;
    return *this;
  }
  cx_t<T>& operator*= (const T& scalr)
  {
    re*=scalr;
    im*=scalr;
    return *this;
  }
  
  cx_t<T>& operator/= (const cx_t<T>& r)
  {
    T den=r.re*r.re+r.im*r.im;
    T t1 = (re*r.re + im*r.im)/den;
    T t2 = (im*r.re - re*r.im)/den;
    re=t1;
    im=t2;
    return *this;
  }
  cx_t<T>& operator/= (const T& scalr)
  {
    re/=scalr;
    im/=scalr;
    return *this;
  }
  
  // These are so other cx_ts can op= and construct easily
  template <class T2> cx_t(const cx_t<T2>& rhs) : re(rhs.re), im(rhs.im) { }
  template <class T2> cx_t(const T2& rhs) : re(rhs), im(0) { }
  template <class T2> cx_t operator= (const cx_t<T2>& rhs)
  {
    re=rhs.re;
    im=rhs.im;
    return *this;
  }
  template <class T2> cx_t operator= (const T2& rhs)
  {
    re=rhs;
    im=0;
    return *this;
  }
  
}; // cx_t

template <class T>
inline cx_t<T> operator+ (const cx_t<T>& l, const cx_t<T>& r)
{
  cx_t<T> retval(l);
  return retval+=r;
}
template <class T, class S>
inline cx_t<T> operator+ (const S& scalar, const cx_t<T>& r)
{
  cx_t<T> retval(scalar);
  return retval+=r;
}
template <class T, class S>
inline cx_t<T> operator+ (const cx_t<T>& l, const S& scalar)
{
  cx_t<T> retval(l);
  return retval+=scalar;
}

template <class T>
inline cx_t<T> operator- (const cx_t<T>& l, const cx_t<T>& r)
{
  cx_t<T> retval(l);
  return retval-=r;
}
template <class T, class S>
inline cx_t<T> operator- (const S& scalar, const cx_t<T>& r)
{
  cx_t<T> retval(scalar);
  return retval-=r;
}
template <class T, class S>
inline cx_t<T> operator- (const cx_t<T>& l, const S& scalar)
{
  cx_t<T> retval(l);
  return retval-=scalar;
}

template <class T>
inline cx_t<T> operator*(const cx_t<T>& l, const cx_t<T>& r)
{
  cx_t<T> retval(l);
  return retval*=r;
}
template <class T, class S>
inline cx_t<T> operator*(const S& scalar, const cx_t<T>& r)
{
  cx_t<T> retval(scalar);
  return retval*=r;
}
template <class T, class S>
inline cx_t<T> operator*(const cx_t<T>& l, const S& scalar)
{
  cx_t<T> retval(l);
  return retval*=scalar;
}

template <class T>
inline cx_t<T> operator/ (const cx_t<T>& l, const cx_t<T>& r)
{
  cx_t<T> retval(l);
  return retval/=r;
}
template <class T, class S>
inline cx_t<T> operator/ (const S& scalar, const cx_t<T>& r)
{
  cx_t<T> retval(scalar);
  return retval/=r;
}
template <class T, class S>
inline cx_t<T> operator/ (const cx_t<T>& l, const S& scalar)
{
  cx_t<T> retval(l);
  return retval/=scalar;
}

template <class T>
inline bool operator== (const cx_t<T>& lhs, const cx_t<T>& rhs)
{
  return lhs.re==rhs.re && lhs.im==rhs.im;
}

template <class T>
inline bool operator!= (const cx_t<T>& lhs, const cx_t<T>& rhs)
{
  return !(lhs==rhs);
}


// ///// Specialize


#if !defined(OC_SUPPORT_XM)
typedef cx_t<real_4> complex_8;
typedef cx_t<real_8> complex_16;

inline ostream& operator<< (ostream& os, complex_8 a)
{
  os.precision(7);
  return os<<"("<<a.re<<((a.im<0)?"":"+")<<a.im<<"j)";
}

inline ostream& operator<< (ostream& os, complex_16 a)
{
  os.precision(15);
  return os<<"("<<a.re<<((a.im<0)?"":"+")<<a.im<<"j)";
}

// TODO:  Ya, this probably needs to be made more robust
template <class T>
inline istream& operator>> (istream& is, cx_t<T>& a)
{
  char c;
  is >> c;
  is >> a.re;
  is >> c;
  is >> a.im;
  is >> c;
  is>>c;
  return is;
}

#else // X-Midas support ... already has complex_8 and complex_16

#include <complex_n.h>
inline istream& operator>> (istream& is, complex_8& a)
{
  char c;
  is >> c;
  is >> a.re;
  is >> c;
  is >> a.im;
  is >> c;
  is>>c;
  return is;
}

inline istream& operator>> (istream& is, complex_16& a)
{
  char c;
  is >> c;
  is >> a.re;
  is >> c;
  is >> a.im;
  is >> c;
  is>>c;
  return is;
}

#endif

// For converting out to scalars, we tend to return the magnitude (squared).
template <class T>
T mag2(const cx_t<T>& v)
{
  return T(v.re*v.re + v.im*v.im);
}



// ///// cx_union_t: This class is bitwise compatible with cx_t, but
// exists only to fit into a Union and play at least somewhat nice
// with cx_t

template <class T>
struct cx_union_t
{
  T re, im;
  operator cx_t<T> () const
  {
    cx_t<T> retval(re, im);
    return retval;
  }
}; // cx_union_t

template <class T>
cx_t<T> cx_union_t_cx(const cx_union_t<T>& x)
{
  return cx_t<T>(x.re, x.im);
}

template <class T>
inline bool operator== (const cx_union_t<T>& lhs, const cx_union_t<T>& rhs)
{
  return lhs.re==rhs.re && lhs.im==rhs.im;
}

template <class T>
inline bool operator!= (const cx_union_t<T>& lhs, const cx_union_t<T>& rhs)
{
  return !(lhs==rhs);
}

template <class T>
T mag2(const cx_union_t<T>& v)
{
  return T(v.re*v.re + v.im*v.im);
}

#define OCCOMPLEX_H_
#endif // OCCOMPLEX_H_
