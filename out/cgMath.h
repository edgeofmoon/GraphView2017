/********************************************************************
	created:	2005/03/08
	created:	8:3:2005   23:47
	filename: 	cgMath.h
	file path:	\...\inc\Utilities
	file base:	cgMath
	file ext:	h
	author:		scf
	
	purpose:	Define some math struct for CG
*********************************************************************/
#ifndef __AFX__CGMATH_H__INCLUDE__
#define __AFX__CGMATH_H__INCLUDE__

#pragma once

#include "MathCommon.h"
#include "Vector.h"
//#include "Quaternion.h"

namespace cgMath {

template <typename T=float>
class CPoint2
{
public:
	CPoint2(T cx=0, T cy=0):x(cx), y(cy) { } 
	CPoint2(const Vector<2, T>& vec2):x(vec2[0]), y(vec2[1]) { }
	virtual ~CPoint2() {}

	void	SetPoint(T cx, T cy) { x = cx;	y = cy; }
	void	OffsetPoint(T dx, T dy) { x += dx;	y += dy; }
	void	OffsetPoint(const Vector<2, T>& vec2) { x += vec2[0];	y += vec2[1]; }
	
	CPoint2& operator= (const Vector<2, T>& vec2) { x = vec2[0]; y = vec2[1];	 return (*this); }
	CPoint2& operator+= (const Vector<2, T>& vec2) { x += vec2[0]; y += vec2[1];	 return (*this); }

	T	 x, y;

protected:

private:

};

typedef CPoint2<int> CPoint2i;
typedef CPoint2<float> CPoint2f;
typedef CPoint2<double> CPoint2d;

template <typename T=float>
class CPoint3
{
public:
	CPoint3(T cx=0, T cy=0, T cz=0):x(cx), y(cy), z(cz) { }
	CPoint3(const Vector<3, T>& vec3):x(vec3[0]), y(vec3[1]), z(vec3[2]) { }
	virtual ~CPoint3() { }

	void	SetPoint(T cx, T cy, T cz) { x = cx;	y = cy;	z = cz; }
	void	OffsetPoint(T dx, T dy, T dz) { x += dx;	y += dy; z +=dz; }
	void	OffsetPoint(const Vector<3, T>& vec3) { x += vec3[0];	y += vec3[1]; z += vec3[2]; }

	CPoint3<T>& operator= (const Vector<3, T>& vec3)
	{ x = vec3[0]; y = vec3[1]; z = vec3[2];	return (*this); }
	
	CPoint3<T>& operator+= (const Vector<3, T>& vec3)
	{ x += vec3[0]; y += vec3[1]; z += vec3[2];	return (*this); }

	CPoint3<T> operator+ (const Vector<3,T>& vec3) const
	{
		CPoint3<T> res(x+vec3[0], y+vec3[1], z+vec3[2]);
		return res;
	}

//	CPoint3<T> operator+ (const CPoint3<T>& pnt) const
//	{
//		CPoint3<T> res(x+vec3[0], y+vec3[1], z+vec3[2]);
//		return res;
//	}

	Vector<3,T> operator- (const CPoint3<T>& pnt2) const
	{
		Vector<3,T> res(x-pnt2.x, y-pnt2.y, z-pnt2.z);
		return res;
	}

	T	 x, y, z;

protected:

private:

};
typedef CPoint3<float> CPoint3f;
typedef CPoint3<double> CPoint3d;

// Global operator overloading
//template <typename T>
//inline CPoint3<T> operator+ (const CPoint3<T>& pnt, const Vector<3,T>& vec3)
//{
//	CPoint3<T> res(pnt.x+vec3[0],pnt.y+vec3[1],pnt.z+vec3[2]);
//	return res;
//}
//
//template <typename T>
//inline CPoint3<T> operator+ (const Vector<3,T>& vec3, const CPoint3<T>& pnt)
//{
//	CPoint3<T> res(pnt.x+vec3[0],pnt.y+vec3[1],pnt.z+vec3[2]);
//
//	return res;
//}
//
//template <typename T>
//inline Vector<3,T> operator- (const CPoint3<T>& pnt1, const CPoint3<T>& pnt2)
//{
//	Vector<3,T> res(pnt1.x-pnt2.x, pnt1.y-pnt2.y, pnt1.z-pnt2.z);
//
//	return res;
//}


struct SBB {//АќЮЇКа
	float		m_fMinX, m_fMinY, m_fMinZ;			
	float		m_fMaxX, m_fMaxY, m_fMaxZ;

	//CPoint3f		m_CorPointMin;
	//CPoint3f		m_CorPointMax;
};

template <typename T=float>
struct Vertex {
	T  Coord[3];
	T  Normal[3];
	T	TexCoord[2];
};
typedef	Vertex<float>	VertexF;
typedef	Vertex<double>	VertexD;

template <typename T=float>
class CCube
{
public:
	CCube(void) {	}
	CCube(const CPoint3<T>& orgPoint, T longX, T widthY, T heightZ, bool bFrmCent = true) 
	{
		if (bFrmCent) {	ConstructC(orgPoint, longX, widthY, heightZ); }
		else{ ConstructO(orgPoint, longX, widthY, heightZ); }
	}

	virtual ~CCube(void) {	}

	CPoint3<T>  m_Vertices[8];
//			 6----- 4
//         /        /  |
//        0------2  |
//        | 7     |   5
//        |        |  /
//        1------3 

	// construct from the origin at down-left
	void ConstructO(const CPoint3<T>& origin, T longX, T widthY, T heightZ);
	// construct from the center point
	void ConstructC(const CPoint3<T>& center, T longX, T widthY, T heightZ);

	// Translate
	void Translate(T longX, T widthY, T heightZ);
};

template <typename T>
inline void CCube<T>::ConstructO(const CPoint3<T>& origin, T longX, T widthY, T heightZ)
{
	m_Vertices[0] = origin + Vector<3, T>(0, 0, heightZ);
	m_Vertices[1] = origin;
	m_Vertices[2] = origin + Vector<3, T>(longX, 0, heightZ);
	m_Vertices[3] = origin + Vector<3, T>(longX, 0, 0);
	m_Vertices[4] = origin + Vector<3, T>(longX, widthY, heightZ);
	m_Vertices[5] = origin + Vector<3, T>(longX, widthY, 0);
	m_Vertices[6] = origin + Vector<3, T>(0, widthY, heightZ);
	m_Vertices[7] = origin + Vector<3, T>(0, widthY, 0);
}

template <typename T>
inline void CCube<T>::ConstructC(const CPoint3<T>& center, T longX, T widthY, T heightZ)
{
	m_Vertices[0] = center + Vector<3, T>(-longX, -widthY, heightZ);
	m_Vertices[1] = center + Vector<3, T>(-longX, -widthY, -heightZ);
	m_Vertices[2] = center + Vector<3, T>(longX, -widthY, heightZ);
	m_Vertices[3] = center + Vector<3, T>(longX, -widthY, -heightZ);
	m_Vertices[4] = center + Vector<3, T>(longX, widthY, heightZ);
	m_Vertices[5] = center + Vector<3, T>(longX, widthY, -heightZ);
	m_Vertices[6] = center + Vector<3, T>(-longX, widthY, heightZ);
	m_Vertices[7] = center + Vector<3, T>(-longX, widthY, -heightZ);
}

template <typename T>
inline void CCube<T>::Translate(T longX, T widthY, T heightZ)
{
	for (short s=0; s<8; ++s) {
		m_Vertices[s] += Vector<3, T>(longX, widthY, heightZ);
	}
}

typedef CCube<float> CCubeF;
typedef CCube<double> CCubeD;

class CArrow3D
{
public:
	CArrow3D(): LongRatio(0.125), CrossRatio(0.0625), l(0.0), m(0.0), n(0.0), u(1.0), v(1.0), w(1.0)  {}
	CArrow3D(const CPoint3f& atPnt, const Vec3f& Dirc, bool bCenter=true);
	CArrow3D(const CPoint3f& headPnt, const CPoint3f tailPnt);

	virtual ~CArrow3D() {}

	CPoint3f  m_HeadPnt,  m_TailPnt;
	CPoint3f  m_WingPnt1, m_WingPnt2;

	bool BuildC(const CPoint3f& centPnt, const Vec3f& Dirc);
	bool BuildO(const CPoint3f& orgnPnt, const Vec3f& Dirc);
	bool Build2P(const CPoint3f& headPnt, const CPoint3f tailPnt);

protected:

private:
	double l, m, n;
	double u, v, w;
	const double LongRatio, CrossRatio;

};

inline bool CArrow3D::BuildC(const CPoint3f& centPnt, const Vec3f& Dirc)
{
	CPoint3f sectPnt;

	l = Dirc[0];
	m = Dirc[1];
	n = Dirc[2];

	m_HeadPnt = centPnt + Vec3f(l*0.5, m*0.5, n*0.5);
	m_TailPnt = centPnt + Vec3f(- l*0.5, - m*0.5, - n*0.5);

	sectPnt = centPnt + Vec3f(l*(0.5-LongRatio), m*(0.5-LongRatio), n*(0.5-LongRatio));

	if ( ! fuzzy_eqz(l) ) // l != 0
	{
		if ( ! fuzzy_eqz(m) ) { // m != 0
			if ( ! fuzzy_eqz(n) ) { // n != 0
				u = 1.0;  // u , v and w be arbitrary
				v = 1.0;
				w = - (l+m) / n;
			}
			else{ // n = 0
				w = 1.0;
				
				u = 1.0; // u and v be arbitrary
				v = - l / m;
			}
		}
		else{ // m = 0
			v = 1.0;

			if ( ! fuzzy_eqz(n) ) { // n != 0
				u = 1.0; // u and w be arbitratry
				w = - l / n;
			}
			else{ // n = 0
				w = 1.0;
				u = 0.0;
			}
		}
	}
	else // l = 0
	{
		u = 1.0;
		
		if ( ! fuzzy_eqz(m) ) { // m != 0
			if ( ! fuzzy_eqz(n) ) { // n != 0
				v = 1.0;  // v and w be arbitrary
				w = - m / n;
			}
			else{ // n = 0
				w = 1.0;
				v = 0.0;
			}
		}
		else{ // m = 0
			v = 1.0;

			if ( ! fuzzy_eqz(n)) { // n != 0
				w = 0.0;
			}
			else{ // n = 0
				w = 1.0;
			}
		}
	}

	double t = CrossRatio * sqrt(l*l + m*m + n*n) / sqrt(u*u + v*v + w*w);

	m_WingPnt1.SetPoint(sectPnt.x+t*u, sectPnt.y+t*v, sectPnt.z+t*w);
	m_WingPnt2.SetPoint(sectPnt.x-t*u, sectPnt.y-t*v, sectPnt.z-t*w);

	return true;
}

// Guassian Elimination
inline bool GaussElim(double* a, int n)
{
	int i, j, k, l, ll, irow, icol;
	double big, pivinv, dum;

	if ( n<2 ) {
		return false;
	}

	int* ipiv = new int[n*3];
	int* indxr = &ipiv[n];
	int* indxc = &ipiv[2*n];
	//int ipiv[60], indxr[60], indxc[60];

	for (j=0;j<=n-1;j++)
	{
		ipiv[j]=0;
	}
	for (i=0;i<=n-1;i++)
	{
		big=0.0;
		for (j=0;j<=n-1;j++)
		{
			if(ipiv[j]!=1)
			{
				for(k=0;k<=n-1;k++)
				{
					if(ipiv[k]==0)
					{
						if(fabs(a[j*n+k])>=big)
						{
							big=fabs(a[j*n+k]);
							irow=j;
							icol=k;
						}
						else if(ipiv[k]>1)
						{
							delete[] ipiv;
							return false;
							//cout<<"singular matrix";
						}
					}
				}
			}
		}
		ipiv[icol]=ipiv[icol]+1;
		if(irow!=icol)
		{
			for(l=0;l<=n-1;l++)
			{
				dum=(a[irow*n+l]);
				a[irow*n+l]=a[icol*n+l];
				a[icol*n+l]=dum;
			}
			//dum=b[irow];
			//b[irow]=b[icol];
			//b[icol]=dum;
		}
		indxr[i]=irow;
		indxc[i]=icol;
		if(a[icol*n+icol]==0.0)
		{
			delete[] ipiv;
			return false;
			//cout<< "singular matrix.";
		}
		pivinv=1.0/(a[icol*n+icol]);
		a[icol*n+icol]=1.0;
		for(l=0;l<=n-1;l++)
		{
			a[icol*n+l]=a[icol*n+l]*pivinv;
		}
		//b[icol]=b[icol]*pivinv;
		for(ll=0;ll<=n-1;ll++)
		{
			if(ll!=icol)
			{
				dum=a[ll*n+icol];
				a[ll*n+icol]=0.0;
				for(l=0;l<=n-1;l++)
				{
					a[ll*n+l]=a[ll*n+l]-a[icol*n+l]*dum;
				}
				//b[ll]=b[ll]-b[icol]*dum;
			}
		}
	}	
	for(l=n-1;l<=0;l--)
	{
		if(indxr[l]!=indxc[l])
		{
			for(k=0;k<=n-1;k++)
			{
				dum=a[k*n+indxr[l]];
				a[k*n+indxr[l]]=a[k*n+indxc[l]];
				a[k*n+indxr[l]]=dum;
			}
		}
	}

	delete[] ipiv;

	return true;
}

// Guassian Linear Equation resolution
inline bool gaussj(double* a, int n, double* b, int m)
{
	int i, j, k, l, ll, irow, icol;
	double big, pivinv, dum;

	if ( n<1 || m<1 ) {
		return false;
	}
	else if ( n==1 ) {
		if ( a[0]<EPSILON_D ) {
			return false;
		}
		else{
			a[0] = 1.0;
			for (i=0; i<=m-1; i++) {
				b[i] /= a[0];
			}
	
			return true;
		}
	}

	int* ipiv = new int[n*3];
	int* indxr = &ipiv[n];
	int* indxc = &ipiv[2*n];
	//int ipiv[60], indxr[60], indxc[60];

	for (j=0;j<=n-1;j++)
	{
		ipiv[j]=0;
	}
	for (i=0;i<=n-1;i++)
	{
		big=0.0;
		for (j=0;j<=n-1;j++)
		{
			if(ipiv[j]!=1)
			{
				for(k=0;k<=n-1;k++)
				{
					if(ipiv[k]==0)
					{
						if(fabs(a[j*n+k])>=big)
						{
							big=fabs(a[j*n+k]);
							irow=j;
							icol=k;
						}
						else if(ipiv[k]>1)
						{
							delete[] ipiv;
							return false;
							//cout<<"singular matrix";
						}
					}
				}
			}
		}
		ipiv[icol]=ipiv[icol]+1;
		if(irow!=icol)
		{
			for(l=0;l<=n-1;l++)
			{
				dum=(a[irow*n+l]);
				a[irow*n+l]=a[icol*n+l];
				a[icol*n+l]=dum;
			}
			for (l=0; l<=m-1; l++)
			{
				dum=b[irow*n+l];
				b[irow*n+l]=b[icol*m+l];
				b[icol*m+l]=dum;
			}
		}
		indxr[i]=irow;
		indxc[i]=icol;
		if(a[icol*n+icol]==0.0)
		{
			delete[] ipiv;
			return false;
			//cout<< "singular matrix.";
		}
		pivinv=1.0/(a[icol*n+icol]);
		a[icol*n+icol]=1.0;
		for(l=0;l<=n-1;l++)
		{
			a[icol*n+l] *= pivinv;
		}
		for(l=0;l<=m-1;l++)
		{
			b[icol*m+l] *= pivinv;
		}
		for(ll=0;ll<=n-1;ll++)
		{
			if(ll!=icol)
			{
				dum=a[ll*n+icol];
				a[ll*n+icol]=0.0;
				for(l=0;l<=n-1;l++)
				{
					a[ll*n+l] -= a[icol*n+l]*dum;
				}
				for(l=0;l<=m-1;l++)
				{
					b[ll*m+l] -= b[icol*m+l]*dum;
				}
			}
		}
	}	
	for(l=n-1;l<=0;l--)
	{
		if(indxr[l]!=indxc[l])
		{
			for(k=0;k<=n-1;k++)
			{
				dum=a[k*n+indxr[l]];
				a[k*n+indxr[l]]=a[k*n+indxc[l]];
				a[k*n+indxr[l]]=dum;
			}
		}
	}

	delete[] ipiv;

	return true;
}

} // namespace

#endif//__AFX__CGMATH_H__INCLUDE__