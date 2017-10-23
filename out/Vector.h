#ifndef VECTORN_H
#define VECTORN_H
/**
 * @addtogroup VectorMath Template based Vector/Matrix library
 * @{
 *
 * @file VectorN.h 
 * Math Vector implementation. Based on original by Peter Ørbæk.  
 * @todo: Add #ifdef's for disabling use of exceptions
 */
#pragma once

#include "MathCommon.h"

//typedef unsigned int uint;
namespace cgMath {

/** Standard 3D indices */
enum Axes
{
	X, Y, Z, W
};

/** N-element vector */
template <unsigned int N, typename T = double>
class Vector 
{
public:
	enum {elements = N};

    // ctors
    Vector();
	explicit Vector(const T x);
	explicit Vector(const T x, const T y);
	explicit Vector(const T x, const T y, const T z);
	explicit Vector(const T x, const T y, const T z, const T w);            
	// NOTE: using default copy ctor

    // access
	// NOTE: using default operator=

	/** Access to element at specified index */
	inline T& idx(uint i);
	/** Access to element at specified index */
	inline const T& idx(uint i) const;
	/** Access to element at specified index */
	inline T& operator[](uint i); 
	/** Access to element at specified index */
	inline const T& operator[](uint i) const;

	/** inserts M vector starting at j */
	template<unsigned int M>
		Vector<N,T>& insert(const Vector<M,T>& a, uint j=0)
	{
		static_assert(M<=N);
		if( M+j > N)
		{
			char buf[128];    
			sprintf(buf, "Vector<%i>::insert(Vector<%i>,%i)const, line: %i"
				" Illegal index!", N, M, j, __LINE__);
			throw std::out_of_range(buf);
		}
		for(uint i=0; i<M;i++)
			v[j+i] = a[i];
		
		return (*this);
	}

	/** extracts M vector starting at j 
	*  usage: v2 = v.extract<2>(1), gets 2 element vector: v2= [ v[1], v[2] ] 
	*/
	template<unsigned int M>            
		Vector<M,T> extract(uint j=0) const 
	{      
		static_assert(M<=N); 
		if( M+j > N )
		{
			char buf[128];
			sprintf(buf, "Vector<%i>::extract(%i), line: %i"
				" can't extract Vector<%i> from index %i!", N, j, __LINE__, M, j);
			throw std::out_of_range(buf);

		}

		Vector<M,T> res;
		for (uint i = 0; i < M; ++i) 
			res[i] = v[j+i];

		return res;
	}

    // methods
	/** Scales vector, so that |v| == 1 */
	Vector<N,T>& Normalize();											 // set 1 == norm(v)
	/** Vector-Vector multiplication */
	inline Vector<N,T>& mul(const Vector<N,T>& a, const Vector<N,T>& b); // element wise mul
	/** Vector dot-product */
	inline T	dot(const Vector<N,T>& a) const;				 // dot-product
	/** Vector-Vector addition */
	inline Vector<N,T>& add(const Vector<N,T>& a, const Vector<N,T>& b); 
	/** Vector-Vector subtraction */
	inline Vector<N,T>& sub(const Vector<N,T>& a, const Vector<N,T>& b);	    
	/** Vector negation. @note: Permanently modifies vector */
    inline Vector<N,T>& neg(const Vector<N,T>& a);
	/** Vector scaling */
	inline Vector<N,T>& smul(const T s, const Vector<N,T>& a);			 // scaling
	/** Vector scaling */
	inline Vector<N,T>& sdiv(const Vector<N,T>& a, const T s);
	/* assign */
	Vector<N,T>& set( T x );
	Vector<N,T>& set( T x, T y );
	Vector<N,T>& set( T x, T y, T z );
	Vector<N,T>& set( T x, T y, T z, T w );


	/** Inserts maximum elements from 'a' or 'b' */
    Vector<N,T>& max(const Vector<N,T>& a, const Vector<N,T>& b); // inserts max element from 'a' or 'b'
	/** Inserts minimum elements from 'a' or 'b' */
    Vector<N,T>& min(const Vector<N,T>& a, const Vector<N,T>& b); // inserts min element from 'a' or 'b'
	/** Gets maximum element */
	const T& max() const;										  // returns max element
	/** Gets minimum element */
	const T& min() const;										  // returns min element
	/** clamps all elements to specified range */
    Vector<N,T>& clamp(const T& min, const T& max);				  // clamps all elements to range

    // operators	    	    
	inline Vector<N,T>& operator+=(const Vector<N,T>& a);
	inline Vector<N,T>& operator-=(const Vector<N,T>& a);
    inline Vector<N,T>& operator/=(const T s);
    inline Vector<N,T>& operator*=(const T s);

    // comparisons
	inline bool lt(const Vector<N,T>& a) const;
	inline bool lte(const Vector<N,T>& a) const;
	inline bool eq(const Vector<N,T>& a) const;
	inline bool neq(const Vector<N,T>& a) const;
    
    // type conversion
	// usage: intvector = floatvector.retype<int>()         
    template<typename T2>
    Vector<N,T2> retype() const 
    {
        Vector<N,T2> r;
        for (uint i = 0; i < N; ++i)
            r[i] = T2(v[i]);
        return r;
    }

public: 
        T v[N];
};
     
    // ctors
template <unsigned int N, typename T>
    inline Vector<N,T>::Vector()
{
	memset(v, 0, sizeof(T)*N);
}

template <unsigned int N, typename T>
    inline Vector<N,T>::Vector(const T x)
{
	fill(*this,x);
}

template <unsigned int N, typename T>
    inline Vector<N,T>::Vector(const T x, const T y)
{
	v[0]=x;
    if (N > 1) v[1] = y;
}

template <unsigned int N, typename T>
    inline Vector<N,T>::Vector(const T x, const T y, const T z)
{
    v[0] = x;
    if (N > 1) v[1] = y;
    if (N > 2) v[2] = z;
}

template <unsigned int N, typename T>
    inline Vector<N,T>::Vector(const T x, const T y, const T z, const T w)
{

    v[0] = x;
    if (N > 1) v[1] = y;
    if (N > 2) v[2] = z;
    if (N > 3) v[3] = w;
}   

// access
template<unsigned int N, typename T>
    inline T& Vector<N,T>::idx(uint i) 
{ 
	if( i>= N)
	{
		char buf[128];    
		sprintf(buf, "Vector<%i>::idx(%i), line: %i"
						" Illegal index!", N,i,__LINE__);
		throw std::out_of_range(buf);
	}
    return v[i]; 
}

template<unsigned int N, typename T>
    inline const T& Vector<N,T>::idx(uint i) const
{ 
	if( i>= N)
	{
		char buf[128];    
		sprintf(buf, "Vector<%i>::idx(%i)const, line: %i"
						" Illegal index!", N,i,__LINE__);
		throw std::out_of_range(buf);
	}
    return v[i]; 
}

template <unsigned int N, typename T>
    inline T& Vector<N,T>::operator[](uint i) 
{ 
	if( i>= N)
	{
		char buf[128];    
		sprintf(buf, "Vector<%i>::operator[](%i), line: %i"
						" Illegal index!", N,i,__LINE__);
		throw std::out_of_range(buf);
	}
    return v[i]; 
}

template <unsigned int N, typename T>
    inline const T& Vector<N,T>::operator[](uint i) const 
{ 
   	if( i>= N)
	{
		char buf[128];    
		sprintf(buf, "Vector<%i>::operator[](%i)const, line: %i"
						" Illegal index!", N,i,__LINE__);
		throw std::out_of_range(buf);
	}
    return v[i]; 
}

// methods
template <unsigned int N, typename T>
    inline Vector<N,T>& Vector<N,T>::Normalize()
{
	// @todo: This fails HARD if norm(*this) is zero!
	double mod =  norm(*this); //songchf added lines below for erroring
	if (mod < 1e-12f) {
		char buf[128];    
		sprintf(buf, "the norm of vector is too near to zero!");
		throw std::out_of_range(buf);
	}

    double len = 1.0/mod;
    for (uint i = 0; i < N; ++i) v[i] = T(v[i] * len);
    return *this;
}  

template <unsigned int N, typename T>
    inline Vector<N,T>& Vector<N,T>::mul(const Vector<N,T>& a, const Vector<N,T>& b)
{
    for (uint i = 0; i < N; ++i)
        v[i] = a.v[i] * b.v[i];
    return *this;
}

// dot-product / inner-product / scalar-product, whatever!!!
template <unsigned int N, typename T>
    inline T Vector<N,T>::dot(const Vector<N,T>& a) const
{
    T x = T(0);
    for (uint i = 0; i < N; ++i)
        x = x + a.v[i] * v[i];
    return x;
}

template <unsigned int N, typename T>
    inline Vector<N,T>& Vector<N,T>::add(const Vector<N,T>& a, const Vector<N,T>& b)
{
    for (uint i = 0; i < N; ++i)
        v[i] = a.v[i] + b.v[i];
    return *this;
}

template <unsigned int N, typename T>
    inline Vector<N,T>& Vector<N,T>::sub(const Vector<N,T>& a, const Vector<N,T>& b)
{
    for (uint i = 0; i < N; ++i)
        v[i] = a.v[i] - b.v[i];
    return *this;
}    

template <unsigned int N, typename T>
    inline Vector<N,T>& Vector<N,T>::neg(const Vector<N,T>& a)
{
    for (uint i = 0; i < N; ++i)
        v[i] = -a.v[i];
    return *this;
}

template <unsigned int N, typename T>
    inline Vector<N,T>& Vector<N,T>::smul(const T s, const Vector<N,T>& a)
{
    for (uint i = 0; i < N; ++i)
        v[i] = s * a.v[i];
    return *this;
}

template <unsigned int N, typename T>
    inline Vector<N,T>& Vector<N,T>::sdiv(const Vector<N,T>& a, const T s)
{
    for (uint i = 0; i < N; ++i)
        v[i] = a.v[i] / s;
    return *this;
}    

template <unsigned int N, typename T>
	Vector<N,T>& Vector<N,T>::set( T x )
{
	v[0] = x;
	return *this;
}

template <unsigned int N, typename T>
	Vector<N,T>& Vector<N,T>::set( T x, T y )
{
	v[0] = x;
	if( N > 1 )
		v[1] = y;
	return *this;
}

template <unsigned int N, typename T>
	Vector<N,T>& Vector<N,T>::set( T x, T y, T z )
{
	v[0] = x;
	if( N > 1 )
		v[1] = y;
	if( N > 2 )
		v[2] = z;
	return *this;
}

template <unsigned int N, typename T>
	Vector<N,T>& Vector<N,T>::set( T x, T y, T z, T w )
{
	v[0] = x;
	if( N > 1 )
		v[1] = y;
	if( N > 2 )
		v[2] = z;
	if( N > 3 )
		v[3] = w;
	return *this;
}


template <unsigned int N, typename T>
    inline Vector<N,T>& Vector<N,T>::max(const Vector<N,T>& a, const Vector<N,T>& b)
{
    for (uint i = 0; i < N; ++i)
    {
        v[i] = max(a[i], b[i]);
    }
    return *this;
}

template <unsigned int N, typename T>
    inline Vector<N,T>& Vector<N,T>::min(const Vector<N,T>& a, const Vector<N,T>& b)
{
    for (uint i = 0; i < N; ++i)
    {
        v[i] = min(a[i], b[i]);
    }
    return *this;
}

template <unsigned int N, typename T>
	inline const T& Vector<N,T>::max()const			  // returns max element
{
	const T* res = &idx(0);
	for(uint i=0; i<N; i++)
	{
		if( *res < idx(i) )
			res = &idx(i);
	}
	return *res;
}

template <unsigned int N, typename T>
	inline const T& Vector<N,T>::min()const			  // returns min element
{
	const T* res = &idx(0);
	for(uint i=0; i<N; i++)
	{
		if( *res > idx(i) )
			res = &idx(i);
	}
	return *res;
}

template <unsigned int N, typename T>
    inline Vector<N,T>& Vector<N,T>::clamp(const T& a, const T& b)
{
    for (uint i = 0; i < N; ++i)
    {
        if (v[i] < a) v[i] = a;
        else if (v[i] > b) v[i] = b;
    }
    return *this;
}    

template <unsigned int N, typename T>
    inline Vector<N,T>& Vector<N,T>::operator+=(const Vector<N,T>& a)
{
    add(*this, a);
    return *this;
}

template <unsigned int N, typename T>
    inline Vector<N,T>& Vector<N,T>::operator-=(const Vector<N,T>& a)
{
    sub(*this, a);
    return *this;
}

template <unsigned int N, typename T>
    inline Vector<N,T>& Vector<N,T>::operator/=(const T s)
{
    sdiv(*this, s);
    return *this;
}

template <unsigned int N, typename T>
    inline Vector<N,T>& Vector<N,T>::operator*=(const T s)
{
    smul(s, *this);
    return *this;
}

// comparisons
template <unsigned int N, typename T>
    inline bool Vector<N,T>::lt(const Vector<N,T>& a) const
{
    for (uint i = 0; i < N; ++i)
    {
        if (!(v[i] < a[i])) return false;
    }
    return true;
}

template <unsigned int N, typename T>
    inline bool Vector<N,T>::lte(const Vector<N,T>& a) const
{
    for (uint i = 0; i < N; ++i)
    {
        if(!(v[i] <= a[i])) 
			return false;
    }
    return true;
}

template <unsigned int N, typename T>
    inline bool Vector<N,T>::eq(const Vector<N,T>& a) const
{
    for (uint i = 0; i < N; ++i)
    {
        if( fuzzy_neq(v[i],a[i]) ) 
			return false;
    }
    return true;
}

	template <unsigned int N, typename T>
    inline bool Vector<N,T>::neq(const Vector<N,T>& a) const
{        
    return !eq(a);
}

// ************************************* End of member funcs **********************************************

// Non-member operators & methods
/** Vector-Vector addition */
template<unsigned int N, typename T>
    inline Vector<N,T> operator+(const Vector<N,T>& a, const Vector<N,T>& b)
{
	Vector<N,T> res;
	return res.add(a,b);
}

/** Vector-Vector subtraction */
template<unsigned int N, typename T>
    inline Vector<N,T> operator-(const Vector<N,T>& a, const Vector<N,T>& b)
{
	Vector<N,T> res;
	return res.sub(a,b);
}

/** Vector negation
	* @note: Permanently negates vector
	*/
template<unsigned int N, typename T>
    inline Vector<N,T> operator-(const Vector<N,T>& a)
{
	Vector<N,T> res;
	return res.neg(a);
}

/** Component-wise multiplication, not dot product!
	*  Think Matlabs .* operator
	*/
template<unsigned int N, typename T>
    inline Vector<N,T> operator*(const Vector<N,T>& a, const Vector<N,T>& b)
{
	Vector<N,T> res;
	return res.mul(a,b);
}

/** Vector scaling, pre-multiply */
template<unsigned int N, typename T1, typename T2>
    inline Vector<N,T1> operator*(const T2 s, const Vector<N,T1>& a)
{
    Vector<N,T1> res;
	res.smul(T1(s), a);
	return res;
}

/** Vector scaling, post-multiply */
template<unsigned int N, typename T1, typename T2>
    inline Vector<N,T1> operator*(const Vector<N,T1>& a, const T2 s)
{
    Vector<N,T1> res;
	res.smul(T1(s), a);
	return res;
}

/** Vector scaling, division */
template<unsigned int N, typename T1, typename T2>
    inline Vector<N,T1> operator/(const Vector<N,T1>& a, const T2 s)
{
    Vector<N,T1> res;
	res.sdiv(a, T1(s));
	return res;
}

/** Component-wise comparison */
template<unsigned int N, typename T>
    inline bool operator<(const Vector<N,T>& a, const Vector<N,T>& b)
{
	return a.lt(b);
}

/** Component-wise comparison */
template<unsigned int N, typename T>
    inline bool operator<=(const Vector<N,T>& a, const Vector<N,T>& b)
{
	return a.leq(b);
}

/** Component-wise comparison */
template<unsigned int N, typename T>
	inline bool operator==(const Vector<N,T>& a, const Vector<N,T>& b)
{
	return a.eq(b);
}

/** Component-wise comparison */
template<unsigned int N, typename T>
	inline bool operator!=(const Vector<N,T>& a, const Vector<N,T>& b)
{
	return a.neq(b);
}

/** Sets all elements in vector to specified value */
template <unsigned int N, typename T1, typename T2>
    inline Vector<N,T1>& fill(Vector<N,T1>& v, const T2& value)
{
    for (uint i = 0; i < N; ++i) 
		v[i] = T1(value);
	return v;
}

/** Sets all elements in vector to zero */
template <unsigned int N, typename T>
    inline Vector<N,T>& zero(Vector<N,T>& v)
{        
	return fill(v,T(0));
}

/** Sets all elements in vector to 1 (ones) */
template <unsigned int N, typename T>
    inline Vector<N,T>& ones(Vector<N,T>& v)
{        
	return fill(v,T(1));
}

/** Returns |v| * |v| */
template <unsigned int N, typename T>
    inline double normSq(const Vector<N,T>& v)
{
	return v.dot(v);
}

/** Returns |v| */
template <unsigned int N, typename T>
    inline double norm(const Vector<N,T>& v)
{
    return sqrt( normSq(v) );
}

/** Returns dot-product */
template<unsigned int N, typename T>
	inline double dot(const Vector<N,T>& a, const Vector<N,T>& b)
{
	return a.dot(b);
}

/** Determinant of 2 2D vectors */
template<typename T>
    inline T det(const Vector<2,T>& a, const Vector<2,T>& b)
{
    return a[X]*b[Y]-a[Y]*b[X];
}

/** Cross-product of 3D vectors, alternative version */
template<typename T>
	inline void cross(const Vector<3,T>& a, const Vector<3,T>& b, Vector<3,T>& c)
{
    c[0] = a[1]*b[2] - a[2]*b[1];
    c[1] = a[2]*b[0] - a[0]*b[2];
    c[2] = a[0]*b[1] - a[1]*b[0];
}

/** Cross-product of 3D vectors */
template<typename T>
	inline Vector<3,T> cross(const Vector<3,T>& a, const Vector<3,T>& b)
{
    // only for 3 dimensions!
    Vector<3,T> res;
    cross(a,b,res);
    return res;
}

typedef Vector<3, float> Vec3f;
typedef Vector<3, double> Vec3d;
typedef Vector<4, float> Vec4f;
typedef Vector<4, float> Vec4d;


} // namespace cgMath

//float length( const Vec3f &v )
//{
//	return sqrt( v[0] * v[0] + v[1] * v[1] + v[2] * v[2] );
//}
//
//Vec3f normalize( const Vec3f &v )
//{
//	return Vec3f( v[0] / length(v), v[1] / length(v), v[2] / length(v) );
//}
/*@}*/
#endif //VECTORN_H