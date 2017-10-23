#ifndef __MATH_COMMON_H__
#define __MATH_COMMON_H__
/**
 * @file Common.h
 * 
 * Standard definitions
 */
	#pragma once

#include <stdlib.h> 
#include <stdexcept>
#include <assert.h>		// standard assertions
#include <math.h>		// used for sqrt
#include <algorithm>    // typesafe min, max, swap

#ifdef _MSC_VER
	#if _MSC_VER > 1300
		#define COMPILER "MS Visual C.Net"
	#else
		#define COMPILER "MS Visual C"
	#endif
	#ifdef _DEBUG
		#define DEBUG_BUILD 1
	#endif
// hmm... It seems MSVC does NOT define the __cplusplus macro when preprocessing .h files
//  This includes the old-school non-ANSI min/max macros... get rid of 'em
	#undef max
	#undef min

	#pragma warning( disable : 4244 ) // conversion float -> double
	#pragma warning( disable : 4305 ) // truncation Scalar -> double

// @todo: Make strings to identify build: Processor type, Debug/release, version No.
// @todo: Macros for DLL import/export?
// Do a #   pragma error "Unknown compiler." if not defined _MSC_VER, or
// version below, say 1300 (.NET version)
#endif

#if DEBUG_BUILD
	#define BUILD_STRING #__DATE__": "#COMPILER" Debug build"
#else
	#define BUILD_STRING #__DATE__": "#COMPILER
#endif

/** floating-point single precision equality limit */
#ifndef EPSILON_F
#define EPSILON_F 1.192092896e-7f
#endif

/** floating-point double precision equality limit */
#ifndef EPSILON_D
#define EPSILON_D 2.2204460492503131e-16
#endif

/** Pi... for computing areas of circles, y'a know? */
#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif

#ifndef M_TWOPI
#define M_TWOPI 2*M_PI
#endif

/** Conversion from radians to degrees */
#ifndef RAD_2_DEG
#define RAD_2_DEG( radians ) (( radians )*180/M_PI)
#endif

/** Conversion from degrees to radians */
#ifndef DEG_2_RAD
#define DEG_2_RAD( degrees ) (( degrees )*M_PI/180)
#endif

/** Explicit boolean 'and' operation */
#define AND &
/** Explicit boolean 'or' operation */
#define OR  |
/** Explicit boolean 'exclusive or' operation */
#define XOR ^

/** Squaring */
#define SQR( x ) (( x )*( x ))

/** For indicating output parameters */
//#ifndef OUT
//#define OUT
//#endif

/** Short-cuts */
typedef unsigned char    uchar;
typedef unsigned short   ushort;
typedef unsigned int     uint;
typedef unsigned long    ulong;
typedef __int64          longlong; // @todo: Linux #ifdef's
typedef unsigned __int64 ulonglong;

// Bring min,max & swap into global namespace 
using std::max;
using std::min;
using std::swap;

	#ifndef static_assert

    // Boost-like static assertion:
    template<bool x> struct static_assertion_failure;
    template <> struct static_assertion_failure<true> { enum { value = 1 }; };
    template<int x> struct static_assert_test{};

    // This can be made pretty portable (Boost), but unfortunatly
    // the issued error does not say where the errornous instantiation occured
	/** Boost-like static assertion (compile-time assertion) */
    #define static_assert( exp ) \
        typedef static_assert_test<\
        sizeof(static_assertion_failure< (bool)( exp ) >)\
        > static_assert_typedef_    

	#endif


/** Floating-point version of rand() */
inline float randf()
{
	return ( rand()*1.0f / RAND_MAX );
}
 
inline float randfBw(float fMin, float fMax)
{
	return ( fMin + (fMax-fMin)*randf() );		 
}

inline int randnBw(int nMin, int nMax)
{
	int nDiff = nMax - nMin + 1;

	return nMin + (rand( ) % nDiff);
}

inline short randLR()
{
	//return ( rand()>RAND_MAX/2 ? 1 : -1 );
	return ( pow( -1, rand() ) );
}
/** Maximum absolute value returned by randf() */
//#define RANDF_MAX 1.0f;

inline bool fuzzy_eqzf(const float& a)
{
	return EPSILON_F >= fabsf(a);
}

inline bool fuzzy_eqz(const double& a)
{
	return EPSILON_D >= fabs(a);
}
/** fuzzy-logic comparison */
template<typename T>
	inline bool fuzzy_eq(const T& a, const T& b)
{
	// only fuzzy compares for float and double
	return a == b;
}

/** fuzzy-logic not-equal comparison */
template<typename T>
	inline bool fuzzy_neq(const T& a, const T& b)
{
	// only fuzzy compares for float and double
	return a != b;
}

/** fuzzy-logic single precision floating-point comparison */
template<>
	inline bool fuzzy_eq<float>(const float& a, const float& b)
{
    return EPSILON_F >= fabs(a-b);
}

/** fuzzy-logic single precision floating-point not-equal comparison */
template<>
	inline bool fuzzy_neq<float>(const float& a, const float& b)
{
	return EPSILON_F < fabs(a-b);
}

/** fuzzy-logic double precision floating-point comparison */
template<>
	inline bool fuzzy_eq<double>(const double& a, const double& b)
{
	return EPSILON_D >= fabs(a-b);
}

/** fuzzy-logic double precision floating-point not-equal comparison */
template<>
	inline bool fuzzy_neq<double>(const double& a, const double& b)
{
	return EPSILON_D < fabs(a-b);
}

template <typename T>
	inline T clamp(const T& a, const T& min, const T& max) {

	if (a < min) 
		return min;
	if (a > max) 
		return max;
	
	return a;
}

#endif //__MATH_COMMON_H__   //NEROSTYPES_H