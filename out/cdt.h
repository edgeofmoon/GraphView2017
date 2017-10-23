// file name: cdt.h
//	including all the data structure used in 
//	constrained delaunay triangultaion(CDT).
//
// author: Zhihua Wang (zhihua@cs.nyu.edu)
// date: Dec 12, 2002
/*************************************************************/

//#define	OUTPUT
//#define	DEBUG
//#define	DETECT_COLLINEAR_DEGENERACY
#pragma once
#define	COMMENT_ON

#define	NOT_SUPPORT_SAME_X
#undef	NOT_SUPPORT_SAME_X
// *************************************************************
//	predefinitions
// *************************************************************
#include	<math.h>
#include	"geom2d.h"
#include	"myMacro.h"

// *************************************************************
//	io utility
// *************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


#define	BLANK	' '
#define	TAB	'\t'

#define	noff_keyword "NOFF"

#define	LINE_LENGTH	80
#define	NO_ERR	1
#define	ERR_IMPORT_FILE	-1
#define	ERR_CANNOT_OPEN_FILE -2
#define	ERR_NOT_VALID_FORMAT -3
#define	ERR_WRONG_VERTEX_NUMBER -4
#define	ERR_WRONG_FACE_NUMBER -4


void	reportWarning(char* str,int line,char* file);
void	reportError(char*	str,int line,char* file);

// *************************************************************
//	primitives
// 	data structure
// *************************************************************
#define	NON_CONSTRANED	-1	// type value of non constrained edge (see below)
#define	CONSTRAINED		1
#define	VIRTUAL			0

#define SEPOFFSET	1	// some value >0 to make offset
#define MAX_Y	HUGE_VAL
#define MIN_Y	-HUGE_VAL

// my Face
class myFace;
typedef myFace* FPointer;
// end my Face

class	Mesh;
class	Vertex;
class	Halfedge;
class	Edge;
class	EdgeList;
class	Slab;
class	Quad;
struct	Window;

typedef	Mesh*	MeshPointer;
typedef	Vertex*	VPointer;
typedef	Halfedge*	HePointer;
typedef	Edge*	EPointer;
typedef	EdgeList*	EListPointer;
typedef	Slab*	SPointer;
typedef	Quad*	QPointer;
typedef	Window* WPointer;

// planar graph
class	Mesh{
	HePointer	getHullHalfedge(VPointer v,int sep_index,bool lowside);
	HePointer	getHullHalfedgeNextTo(HePointer _he,VPointer v,int l,int sep_index,int r,bool lowside);
	void	findInitialCandidatePair(WPointer w,
		VPointer &p0,VPointer &q0,
		HePointer &p_he,HePointer &q_he,
		bool	lowside);
	EPointer	advancePairToBridge(WPointer w,VPointer p0,VPointer q0,HePointer p_he,HePointer q_he,bool lowside);
public:
	int v_num;	//the number of vertices read from input
	VPointer* v_array;	// the array pointing to the vertices
	double*	sep_array;	// x-value of vertical separate lines

	int e_num;	// numver of edges
	int ce_num;	// number of constrained edges read from input
	EListPointer	e_head,e_tail;

	//bounding box (mins.x, maxs.x) * (mins.y maxs,y)
	Point	maxs;	
	Point	mins;
	//bounding utility
	VPointer vlb,vlt;	//left most, btm and top vertex added manually
	VPointer vrb,vrt;
	EPointer sky_e;
	EPointer eth_e;
	//slab head
	SPointer	s_head;
//	SPointer	s_tail;

	Mesh();
	EPointer addEdge(VPointer v1,VPointer v2,int type=NON_CONSTRANED, bool isExt = false);
	void	deleteEdge(EPointer e);

	int initSlabs();
	int	mergeSlabs();
	void mergeSlabPair(SPointer	ls);
	void sortVertices();
	int import2OFFFile(char* sf);
	int	export2OFFFile(char* sf,char* f=NULL);
	EPointer findBridge(WPointer w,bool lowside);
	EPointer findNextBaseEdge(WPointer w,EPointer e);
	void buildCDTOfWindow(WPointer w);
	int	buildCDT();
	void	display();

	MeshPointer copy();
};

//vertex	
class Vertex{
private:
////members
	int	valence;
	int	index;	// index in x coordinate order
////functions
public:
////members
	////topology
	HePointer	out_he;	//out halfedge pointer

	////geometry
	Point	pos;

	//// external edge idx
	int extIdx;

////functions
	//constructor
	Vertex(double _x,double _y,int i, int _extIdx = -1);
//	Vertex(const Point &p=O,int i=0);
	~Vertex();

	//field access
	int	getIndex()const	{return	index;};	//not user interface, only used by exportXXFile()
	void setIndex(int _i){index=_i;};			//not user interface, only used by exportXXFile()

	//initOutHalfedgeList();
	void addToOutHalfedgeList(HePointer he);	// not an interface for user
	void deleteFromOutHalfedgeList(HePointer he);	//not an interface for use

	HePointer findOutHalfedgeInRing(VPointer v2);
	HePointer findInHalfedgeInRing(VPointer v2);

	HePointer	addHalfedge(VPointer v2,EPointer e);// not an interface to user, only called by addEdge()

	int	getValence()const{return	valence;};

};// of Vertex

// Halfedge
class Halfedge{
public:
////members
	//topology
	VPointer	v;	// vertex pointer to front v(fv)
	//back v(bv) is pre_he->v

	// pointer to the face
	FPointer f;

	EPointer	e;	// edge pointer
	HePointer inv_he;	// opposite halfedge pointer
	HePointer next_he;	//next halfedge
	HePointer pre_he;	//pre halfedge
	
	//geometry
	//normalized direction	vector
	//	double	v[Dimension];
	Vector	d;//direction from back v to front v

////functions
	Halfedge(VPointer _v=NULL,EPointer _e=NULL):v(_v),e(_e){inv_he=next_he=pre_he=NULL;d.x=0;d.y=0;f = NULL;};
	~Halfedge();
	//field access
	//topology query
	VPointer	getBackVertex()	const{return	inv_he->v;};
	VPointer	getFrontVertex()	const{return	v;};

};// of Halfedge

// Edge
class Edge{
public:
//members
	//topology
	//he[0] is always from left to right
	//he[1] is always from right to left
	HePointer	he[2];	//halfedge pointer pair, he[0]->v->pos.x > he[1]'s

	//scene level
	EListPointer	iter;	

	//type, can be constrained or nonconstrained
	int	type;
	bool isExternal;

//functions
	Edge(VPointer v1,VPointer v2,int type=NON_CONSTRANED, bool isExt = false);
	~Edge();
	//field access

	//topology query
	//if an Edge exists, there must be 2 Halfedge 
	VPointer	getOneVertex()const	{return	he[0]->v;};	
	VPointer	getTheOtherVertex()const{return he[1]->v;};

};// of Edge

class	EdgeList{
public:
	EPointer	e;
	EListPointer next;
	
	EdgeList(EPointer _e):e(_e){e->iter=this;next=NULL;};
};

//slab
class Slab{
public:
	int	l,r;	// indices of left and right separate line
	QPointer	q_head,q_tail;	// head of list of quads
	SPointer	next;	// next slab 
//methords
	//constructor with li as left index and ri as right index
	Slab(int li,int ri);
	void pushBackQuad(QPointer q);
};

// quad
class Quad{
public:
	// highest and lowest vertices in the quad
	VPointer	top_v,btm_v;

	// top and bottom long edges of this quad (refer to long edge above)
	HePointer	top_he;// top half-edge from right to left
	HePointer	btm_he;// bottom half-edge from left to right

	// indices of left and right vertical line
	int		l,r;

	// y value corresponding to the intersection of top_v and top_he
	double		top_y;
	// y value corresponding to the intersection of btm_v and btm_he
	double		btm_y;
	// y value of four cornor
	double		left_top_y;		
	double		left_btm_y;		
	double		right_top_y;	
	double		right_btm_y;	

	QPointer		next;	// next quad in the list for this slab

//methords
	// constructor with highv as hv, lowv as lv, t as top and b as btm
	Quad(VPointer highv,VPointer lowv,HePointer t,HePointer b,int _l,int _r);
	Quad();
	void updateLongEdge(EPointer e);
	void computeCornorY(double lx,double rx);
};

struct	Window{
//	int	m;		//middle separate line index
	QPointer	lq,rq;	// left and right quad

	//-1 :left above right
	//0	: identical
	// 1: left below right
	int	upsign,downsign;	//sign indicating the intersection situation for lq,rq's top,btm halfedges
};

//given two quad which share a separate line, return their intersection window
//return null if no intersection and set lgr to true if lq is above rq
WPointer	getWindow(QPointer lq,QPointer rq,bool &lgr);

//primitives in computational geometry
/******************************************************************
Given ratio of (xq-x1)/(xq-x2), assume xq!=x2
Return:
never be 1
=0	:xq eq x1
<0  : xq in middle of x1,x2
0< <1: x1 is in middle of xq,x2
>1	:x2 is in middle of xq,x1
******************************************************************/
/*
//#EXACT
double	getLambda(double xq,double x1,doube x2)
{
	assert(xq-x2);
	return((xq-x1)/(xq-x2));
}
*/
/******************************************************************
is xq in between x1,x2, assume xq not eq x1 or x2
******************************************************************/
bool	inBetween(double xq,double x1,double x2);

/******************************************************************
Given two vertices on a line,
get the y-value of vertex on line whose x-vaule is given too.

Using line equation (y-y1)/(x-x1)=(y2-y1)/(x2-x1)
      y*(x2-x1)=(x-x1)*(y2-y1)+y1*(x2-x1)=(x-x1)*y2+y1*(x2-x)
******************************************************************/
//#EXACT
double	yOfLineGivenX(double x1,double y1,double x2,double y2,double x)	;

/******************************************************************
 Point on the side of radial line: (x,y)'s position according to 
 radial line (ox,oy) to (x1,y1)
 return value: 
   -1 left
    0 on line
    1 right
******************************************************************/
//#EXACT	this function requires exact computation
int	pointToLine(double x,double y,double ox,double oy,double x1,double y1);

/*
  The side of (px,py) to the circumcircle of three points
  (ax,ay), (bx,by) and (cx,cy)
  return value:
  -1 outside
   0 on the circle
   1 inside
*/
//#EXACT	this function requires exact computation
int pointToCircle(double px,double py,
		double ax,double ay,
		double bx,double by,
		double cx,double cy); 

/* 
  primitive test on whether q(xq,yq) falls in the angle from 
 op1((xo,yo)->(x1,y1)) to op2((xo,yo)->(x2,y2))
  return value: 
  -2 collinear to op2		here COLLINEAR stands for ONLY half of the line
  -1 collinear to op1
  0	outside
  1	yes, inside
*/
//#EXACT	this function requires exact computation
int	pointInAngle(double xo,double yo,
			double	xq,double yq,
			double	x1,double y1,
			double	x2,double y2);

// *************************************************************
//	sort algorithm used to sort vertices by x value
// *************************************************************
//***************** quick sort functions begin **********************
// for partition
void	swap(VPointer* array,int i,int j);

// for quick sort
int	partition(VPointer* array,int p,int r);

// quick sort
// input :from is the index where sort begins
// count is the index of where sort ends
// usually quicksort(array,0,n-1)
void quicksort(VPointer* array,int from,int to);