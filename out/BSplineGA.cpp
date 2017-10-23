//#include "stdafx.h"
//#include <stdio.h>
//#include <stdlib.h>

//#ifdef _WIN32
//	#define WIN32_LEAN_AND_MEAN
//	#include <windows.h>
//#endif

#include ".\bsplinega.h"

#pragma warning(disable:4267)  // disable warning C4267: conversion from size_t to int

CBSplineGA::CBSplineGA(void)
: m_fAppRatio(1.1f)
, p(3)
, P(NULL)
, Q(NULL)
, U(NULL)
, Ub(NULL)
, Nf(NULL)
, m_pOutputPts(NULL)
, m_pDataPtsOnCurve(NULL)
, m_bInit(false)
{
}

CBSplineGA::CBSplineGA( const std::vector< Vec3f>& vDataPoint, float fErrTol, int bDeg)
: m_fAppRatio(1.1f)
, p(3)
, P(NULL)
, Q(NULL)
, U(NULL)
, Ub(NULL)
, Nf(NULL)
, m_pOutputPts(NULL)
, m_pDataPtsOnCurve(NULL)
, m_bInit(false)
{
	m = vDataPoint.size() - 1;

	if (m<3) {
		m_bInit = false;
		return;
	}
	else 	if ( m==3 ) {
		p = 2;
		n = 3;
	}
	else if ( m>3) {
		p = bDeg;
		n = (int) (m+1) / m_fAppRatio - 1;
	}

	if ( p<1 || (m-p)<0 ) {
		m_bInit = false;
		return;
	}

	Q = vDataPoint;

	try{
		P = new Vec3f[n+1];
	}
	catch (std::bad_alloc exp) {
		throw exp;
		P = NULL;
	}

	if (NULL == P) {
		m_bInit = false;
		return;
	}

	ComputeUkVector();
	ComputeKnotVector();

	m_bInit = true;
}

CBSplineGA::~CBSplineGA(void)
{
	Invalidate();
}

void CBSplineGA::Invalidate()
{
	if ( NULL != P) {
		delete[] P;
		P = NULL;
	}

	if ( NULL != Nf) {
		delete[] Nf;
		Nf = NULL;
	}

	if ( NULL != m_pOutputPts ) {
		delete[] m_pOutputPts;
		m_pOutputPts = NULL;
	}
	
	if ( NULL == U) {
		delete[] U;
		U = NULL;
	}

	if ( NULL == Ub) {
		delete[] Ub;
		Ub = NULL;
	}

	if ( NULL != m_pDataPtsOnCurve ) {
		delete[] m_pDataPtsOnCurve;
		m_pDataPtsOnCurve = NULL;
	}

	m_bInit = false;
}

bool CBSplineGA::Create( const std::vector< Vec3f>& vDataPoint, float fErrTol, int bDeg)
{
	Invalidate();
	
	m = vDataPoint.size() - 1;

	//if (m<2) {
	if (m<3) {
		m_bInit = false;
		return false;
	}
	//else if ( m==2 ) {
	//	p = 2;
	//	n = 2;
	//}
	else 	if ( m==3 ) {
		p = 2;
		n = 3;
	}
	else if ( m>3) {
		p = bDeg;
		n = (int) (m+1) / m_fAppRatio - 1;
	}
	
	if ( p<1 || (m-p)<0 ) {
		m_bInit = false;
		return false;
	}

	Nf = new float[p+1];
	if ( NULL == Nf ) {
		m_bInit = false;
		return false;
	}

	Q = vDataPoint;

	try{
		P = new Vec3f[n+1];
	}
	catch (std::bad_alloc exp) {
		throw exp;
		P = NULL;
	}

	if (NULL == P) {
		m_bInit = false;
		return false;
	}

	ComputeUkVector();
	ComputeKnotVector();

	m_bInit = true;

	return true;
}

// Get a Point on the spline
Vec3f CBSplineGA::CurvePoint(float u)
{
	Vec3f cvPt;

	if (!m_bInit) {
		return cvPt;
	}

	int span = FindSpan(u);
	
	BasisFuns(span, u);

	for (int i=0; i<=p; i++) {
		cvPt += Nf[i]*P[span-p+i];
	}

	return cvPt;
}

Vec3f* CBSplineGA::OutputPoints( int Num)
{
	if ( Num < p+1 || !m_bInit) {
		return NULL;
	}
	
	if ( NULL != m_pOutputPts ) {
		delete[] m_pOutputPts;
		m_pOutputPts = NULL;
	}
	m_pOutputPts = new Vec3f[Num];
	if ( NULL == m_pOutputPts ) {
		return NULL;
	}

	float step = 1.0 / (Num-1);

	m_pOutputPts[0] = Q[0];
	m_pOutputPts[Num-1] = Q[m];

	for (int i=1; i<Num-1; i++) {
		m_pOutputPts[i] = CurvePoint(i*step);
	}

	return m_pOutputPts;
}

Vec3f* CBSplineGA::ProjectOnCurve()
{
	if ( !m_bInit || NULL==Ub ) {
		return NULL;
	}

	if ( NULL != m_pDataPtsOnCurve ) {
		delete[] m_pDataPtsOnCurve;
		m_pDataPtsOnCurve = NULL;
	}
	m_pDataPtsOnCurve = new Vec3f[m+1];
	if ( NULL == m_pDataPtsOnCurve ) {
		return NULL;
	}

	m_pDataPtsOnCurve[0] = Q[0];
	m_pDataPtsOnCurve[m] = Q[m];

	for (int i=1; i<m; i++) {
		m_pDataPtsOnCurve[i] = CurvePoint( Ub[i] );
	}

	return m_pDataPtsOnCurve;
}

int CBSplineGA::FindSpan(float u)
{ // only none-zero span are indexed, so the size is n-p+1: 0 ---- (n-p)
	int low, high, mid; 

	assert(U);

	if (u >= U[n+1]) // special case 
		return (n);
	if (u<=U[p]) {
		return p;
	}
	
	low = p; 
	high = n+1; 
	mid = (low+high)/2.0f;
	
	while (u < U[mid] || u >= U[mid+1]) 
	{
		if ( u < U[mid] ) 
		{ high = mid;}
		else
		{ low = mid;	}
        
		mid = (low+high)/2.0f;
	}

	return (mid); 
}

void CBSplineGA::BasisFuns(int i, float u)
{
	float* left = new float[2*(p+1)];
	float* right = &left[p+1];

	float temp, saved;

	if (NULL == Nf) {
		return;
	}

	Nf[0] = 1.0f;
	for(int j=1; j<=p; j++){
		left[j] = u-U[i+1-j];
		right[j] = U[i+j]-u;
		saved = 0.0f;
		for(int r=0 ; r<j; r++){
			temp = Nf[r] / (right[r+1]+left[j-r]) ;
			Nf[r] = saved + right[r+1]*temp;
			saved = left[j-r] * temp;
		}
		Nf[j] = saved;
	}

	delete[] left;
}

void CBSplineGA::ComputeUkVector(void)
{
	if ( NULL == Ub) {
		delete[] Ub;
		Ub = NULL;
	}
	Ub = new float[m+1];
	if ( NULL == Ub) {
		return;
	}
	
	int i ;
	float d = 0.0f;

	Ub[0] = 0.0f; 
	for(i=1; i<m+1; i++){
		d += normSq(Q[i]-Q[i-1]) ;
	}
	for(i=1;i<m;i++){
		Ub[i] = Ub[i-1] + normSq(Q[i]-Q[i-1])/d ;
	}
	Ub[m] = 1.0f; // In case there is some addition round-off
}

void CBSplineGA::ComputeKnotVector(void)
{
	if ( NULL == U) {
		delete[] U;
		U = NULL;
	}
	U = new float[n+p+2];
	if ( NULL == U) {
		return;
	}

	int i, j;
	float d, a;

	d = (float)(m+1) / (float)(n) ; 
	
	for(j=0; j<=p; ++j)
	{
		U[j] = 0;
		U[n+j+1] = 1.0f;
	}

	for(j=1;j<=n-p;++j){
		U[p+j] = 0.0f;
		for(int k=j;k<j+p;++k){
			i = (int)(k*d) ;
			a = float(k*d) - float(i);
			int i2 = (int)((k-1)*d);
			U[p+j] += a*Ub[i2] + (1-a)*Ub[i];
		}
		U[p+j] /= p;
	}
}

#define __USE_GUASS_LINEAR_RESOV__

// Least Squares Curve Approximation
bool CBSplineGA::LeastSquaresApp(void)
{
	if ( !m_bInit ) {
		return false;
	}
	
	int i, j, span;
	int k;
	
	//Vector< Point_nD<T,D> > R(n), rk(m);
	//Vector<T> funs(deg_+1) ; // Equivalent to Nf 
	//Matrix_DOUBLE N(m,n) ;
	Vec3d* R = new Vec3d[n-1];
	Vec3d* rk = new Vec3d[m-1];
	//Matrix<m, n> N;
	double* N = new double[(m-1)*(n+1)];
	memset( N, 0, sizeof(double)*(m-1)*(n+1) );
	
	//R[0] = Q[0];
	//R[n-1] = Q[m-1];

	// Set up N 
	//N[0] = 1.0;
	//N[(m-1)*(n+1)-1] = 1.0;
	//N(0,0) = 1.0 ;
	//N(m-1,n-1) = 1.0 ;

	//  for(i=1;i<m-1;i++){
	for(i=0; i<m-1; i++) {
		span = FindSpan(Ub[i+1]) ;
		BasisFuns(span, Ub[i+1]);
		
		for(j=0; j<=p; ++j) { // BOOO
			//if(span-deg_+j>0)
			//N(i, span-p+j) = (double) Nf[j];
			N[i*(n+1)+(span-p+j)] = (double) Nf[j];
		}
		rk[i] = Q[i+1].retype<double>() - N[i*(n+1)]*Q[0].retype<double>() - N[i*(n+1)+n]*Q[m].retype<double>();
	}

	// Set up R
	//  for(i=1;i<n-1;i++){
	for(i=0; i<n-1; i++){
		R[i].set(0.0, 0.0, 0.0);
		//    for(j=1;j<m-1;j++)
		for(j=0; j<m-1; j++)
			R[i] += N[ j*(n+1)+(i+1) ] * rk[j];
		
		//if(R[i].x()*R[i].x()<1e-10 && 
		//	R[i].y()*R[i].y()<1e-10 &&
		//	R[i].z()*R[i].z()<1e-10)
		if ( normSq(R[i]) < EPSILON_D ) {
			return false; 
		}
	}

	// Construct N^t*N and RV
	double* NtN = new double[(n-1)*(n-1)];
	memset(NtN, 0, sizeof(double)*(n-1)*(n-1));

	// setup Matrix--Ntn
	for (i=0; i<n-1; i++) {
		for (j=0; j<n-1; j++) {
			for (k=0; k<m-1; k++) {
				NtN[(n-1)*i+j] += N[(i+1)+(n+1)*k]*N[(j+1)+(n+1)*k];
			}
		}
	}
	
	assert(P);

#ifdef __USE_GUASS_LINEAR_RESOV__

	// setup Vector--RV
	double* RV = new double[(n-1)*3];
	memset( RV, 0, sizeof(double)*(n-1)*3 );

	for (i=0; i<n-1; i++) {
		RV[i*3] = R[i][0];
		RV[i*3+1] = R[i][1];
		RV[i*3+2] = R[i][2];
	}

	// Solve  N^T*N*P = R using Guassian Linear Equation resolution
	bool bInSlv = gaussj( NtN, n-1, RV, 3 );
	if ( !bInSlv ) {
		return false;
	}
	for (i=0; i<n-1; i++) {
		P[i+1].set(RV[i*3], RV[i*3+1], RV[i*3+2]);
	}

#else

	// Just Guassian Elimination method
	bool bInvt = GaussElim(NtN, n-1);
	if (!bInvt) {
		return false;
	}

	// Matrix-Vector multiple
	for (i=0; i<n-1; i++) {
		for (j=0; j<n-1; j++) {
			P[i+1] += (NtN[i*(n-1)+j]*R[j]).retype<float>();
		}
	}

#endif // __USE_GUASS_LINEAR_RESOV__

	// must check for the case where we want a curve of degree 1 having
	// only 2 points.
	//if(n-2>0){ 
	//	Matrix_DOUBLE X(n-2,D),B(n-2,D),Ns(m-2,n-2) ;
	//	for(i=0;i<B.rows();i++){
	//		for(j=0;j<D;j++)
	//			B(i,j) = (double)R[i+1].data[j] ;
	//	}
	//	Ns = N.get(1,1,m-2,n-2) ;

	//	solve(transpose(Ns)*Ns,B,X) ;

	//	for(i=0;i<X.rows();i++){
	//		for(j=0;j<X.cols();j++)
	//			P[i+1].data[j] = (T)X(i,j) ;
	//		P[i+1].w() = 1.0 ;
	//	}
	//}
	
	P[0] = Q[0];
	P[n] = Q[m];

	delete[] rk;
	delete[] R;
	delete[] N;
	delete[] NtN;

	return true;
}
