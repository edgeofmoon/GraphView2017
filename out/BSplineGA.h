#ifndef __BSPLINE_APP__
#define __BSPLINE_APP__

#pragma once

#include<vector>

//Math lib
#include "cgMath.h"
using namespace cgMath;

typedef int BYTE;
//#include "cgMath/cgMath.h"
//using namespace cgMath;

//template <int N=3>
class CBSplineGA
{
public:
	CBSplineGA(void);
	CBSplineGA( const std::vector< Vec3f>& vDataPoint, float fErrTol=0.05, int bDeg=3 );
	virtual ~CBSplineGA(void);
	
	bool  Create( const std::vector< Vec3f>& vDataPoint, float fErrTol=0.05, int bDeg=3 );
	void  Invalidate();
	
	// Get a Point on the spline
	Vec3f CurvePoint(float u);
	
	Vec3f* OutputPoints( int Num);

	int ControlPtsNum() const { return n+1; }
	Vec3f* ControlPoints() const {return P;}

	Vec3f* ProjectOnCurve();

protected:
	int FindSpan(float u);
	void BasisFuns(int i, float u);
	void ComputeUkVector(void);
	void ComputeKnotVector(void);

	Vec3f*  m_pOutputPts;
	Vec3f*  m_pDataPtsOnCurve;

private:
	bool  m_bInit;
	const  float  m_fAppRatio;
	BYTE  p; // degree >=1
	int n; // number of control points
	Vec3f* P; // Control Points array, size is (n+1): 0--n
	int m; // number of Data points
	std::vector< Vec3f> Q; // Data points Input by outer, size is (m+1): 0--m
	float* U; // Knot-Vector, size if (n+p+2): 0--(n+p+1)
	float* Ub; // Parameters of Data Points, size is (m+1)
	float* Nf; // Coefficients of one Knot span, size is (p+1): 0--p

public:
	// Least Squares Curve Approximation
	bool LeastSquaresApp(void);
};


#endif // undef __BSPLINE_APP__