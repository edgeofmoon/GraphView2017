// file name: cdt.cpp
//
//      main program to compute a
//      constrained delaunay triangultaion (CDT).
//
// author: Zhihua Wang (zhihua@cs.nyu.edu)
// date: Dec 12, 2002

/*************************************************************


    Purpose:
      This program implements a divide-and-conquer algorithm
      to compute the Constrained Delaunay Triangulation (CDT) for a given 
     planar straight line graph.
      For detail, please refer to the accompanying paper "cdt.ps"
     about this algorithm.

    Usage:
          % ./cdt input_file [output_file]

      where the format of the input_file is described
      in the accompanying doc/format.txt in this directory.
      Basically, the inputFile format is a subset of .noff format.

      If the output_file is not given, the default output file will
   be called "result.noff".  Output file format is the same
   as the input file format.


    More Details:
   The input file comprises a sequence of n vertices and 
      m line-segments in the plane.

        The output file describes a triangulation represented 
      by the same sequence of n vertices, and a sequence
      of M line-segments (M >= m).  All the input line-
      segments will be included in the output.

    Algorithm:

    (1) Initialization:
        1) Read and store input as a half-edge data structure;
            the necessary topological information will be computed
       (e.g., the clockwise order of all the edges incident
      upon a given vertex).

           1.1) Read the vertices and save them in an array.
           1.2) Sort vertices by X-coordinate and construct a 
      map into the original array of vertices.
           1.3) Read the edges and construct the 
                 half-edges using the map above.

        2) Conceptually, add n+1 vertical lines which separate each vertex.  
           (We assume that no two vertices have same X-value.)

        3) Add two horizontal line-segments which lie (respectively) above
          and below all the vertices; the line-segments
     is sufficiently long so that it crosses all the vertical lines.

    (2) Division:
           Create a list of quads, where each quad is the bounded region 
           between two consecutive vertical lines and between the
      two horizontal line-segments.


    (3) Conquer:
   We pair up the quads so that each pair are adjacent to each
   other.  For each pair of quads, we want to merge them 
        and construct the CDT of the merged quad.

    NOTE:
      The function with a comment line "//#EXACT " before it 
      means it requires exact computation.

*************************************************************/

#include "cdt.h"
#include <math.h>
#include <string.h>
#include <gl/glut.h>

//*****************************************************************
//extension of IO for Debuging
//*****************************************************************



void	reportString(char* str)
{
	fprintf(stderr,"%s\n",str);
}

void	reportErrorString(char*	str)
{
	fprintf(stderr,"Error: %s\n",str);
	exit(-1);
}

void	reportError(char*	str,int line,char* file)
{
	fprintf(stderr,"Error: [%s:%d] %s\n",file,line,str);
	exit(-1);
}

void	reportWarningString(char* str)
{
	fprintf(stderr,"Warning: %s\n",str);
}

void	reportWarning(char* str,int line,char* file)
{
	fprintf(stderr,"Warning: [%s:%d] %s\n",file,line,str);
}

// read one line of file fp points to from current point
void	readToLineEnd(FILE * fp)
{
	int	c;
	c=fgetc(fp);
	while	(c!='\n')
		c=fgetc(fp);
}

int	readFirstNonspaceChar(FILE* fp)
{
	int	c;
	c=fgetc(fp);
	while	(c==BLANK || c==TAB)
		c=fgetc(fp);
	return c;
}

// omit the comment in fp
// in case the first character of line is '#'
void	omitComment(FILE* fp,char Leading_Comment_Char='#')
{
	/*

	int c;
	//omit comment
	c=readFirstNonspaceChar(fp);

	#ifdef	DEBUG
	printf("First Charater:%c\n",c);
	#endif//	DEBUG

	// omit comment line
	while	(c==Leading_Comment_Char)
	{
	readToLineEnd(fp);
	c=readFirstNonspaceChar(fp);
	#ifdef	DEBUG
	printf("First Charater:%c\n",c);
	#endif//	DEBUG
	}
	fseek(fp,-1L,SEEK_CUR);	//put back one character
	*/	
}


//print information about slab
void   printSlab(SPointer s_head)
{

      printf("%s\n","# information of slab and quads");
//      cout<<"# information of slab and quads"<<endl;
      SPointer   tmps;
      tmps=s_head;
      while   (tmps!=NULL)
      {
//         cout<<""# slab ("<<tmps->l<<','<<tmps->r<<")"<<endl;
         printf("%s%d,%d%s\n","# slab (",tmps->l,tmps->r,")");

         QPointer   tmpq;
         int   cnt=0;
         tmpq=tmps->q_head;
         while   (tmpq!=NULL)
         {
            cnt++;
            printf("%s%d%s%d\n","# quad (",cnt,") highest vertex's index is ",tmpq->top_v->getIndex());
            printf("%s%d%s%d\n","# quad (",cnt,") lowest vertex's index is ",tmpq->btm_v->getIndex());
//            printf("%s%d%s%f\n","# quad (",cnt,") top y =",tmpq->topy);
//            printf("%s%d%s%f\n","# quad (",cnt,") bottom y =",tmpq->btmy);
            printf("%s%d%s%d%s%d\n","# quad (",cnt,") top half edge is ",tmpq->top_he->getBackVertex()->getIndex(),"-->",tmpq->top_he->getFrontVertex()->getIndex());
            printf("%s%d%s%d%s%d\n","# quad (",cnt,") bottom half edge is ",tmpq->btm_he->getBackVertex()->getIndex(),"-->",tmpq->btm_he->getFrontVertex()->getIndex());
/*            cout<<"# quad ("<<cnt<<") highest vertex's index is "<<tmpq->hv->index<<endl;
            cout<<"# quad ("<<cnt<<") lowest vertex's index is "<<tmpq->lv->index<<endl;
//            printf("%s%d%s%f\n","# quad (",cnt,") top y =",tmpq->topy);
//            printf("%s%d%s%f\n","# quad (",cnt,") bottom y =",tmpq->btmy);
            printf("%s%d%s%d%s%d\n","# quad (",cnt,") top half edge is ",tmpq->top->getBackVertex()->index,"-->",tmpq->top->fv->index);
            cout<<"# quad ("<<cnt<<") top half edge is "<<tmpq->top->getBackVertex()->index<<"-->"<<tmpq->top->fv->index<<endl;
            printf("%s%d%s%d%s%d\n","# quad (",cnt,") bottom half edge is ",tmpq->btm->getBackVertex()->index,"-->",tmpq->btm->fv->index);
            cout<<"# quad ("<<cnt<<") bottom half edge is "<<tmpq->btm->getBackVertex()->index<<"-->"<<tmpq->btm->fv->index<<endl;
*/
         // forward one link
            tmpq=tmpq->next;
         }
         tmps=tmps->next;   
      }   
}


// quick sort
// input :from is the index where sort begins
// count is the index of where sort ends
// usually quicksort(array,0,n-1)
void quicksort(VPointer* array,int from,int to)
{
	int middle;
	if(from<to)
	{
		middle=partition(array,from,to);
#ifdef DEBUG
		if
			(middle>=0)
		{
#endif
			quicksort(array,from,middle-1);
			quicksort(array,middle+1,to);
#ifdef DEBUG
		}
#endif
	}	
}
//given two quad which share a separate line, return their intersection window
//return null if no intersection and set lgr to true if lq is above rq
WPointer	getWindow(QPointer lq,QPointer rq,bool &lgr)
{
#define	LEFT_ABOVE_RIGHT	true
#define	RIGHT_ABOVE_LEFT	!LEFT_ABOVE_RIGHT

	WPointer	w;
#ifdef	DEBUG
	assert(lq->r==rq->l);
#endif
	//use this to avoid comparision double numbers
	if	(	lq->btm_he	==	rq->top_he->inv_he
		||	lq->right_btm_y>rq->left_top_y)
	{
		lgr=LEFT_ABOVE_RIGHT;
		return NULL;
	}

	//	w->m=lq->r;

	//use this to avoid comparision double numbers
	if	(	lq->top_he->inv_he	==	rq->btm_he
		||	lq->right_top_y<rq->left_btm_y)
	{
		lgr=RIGHT_ABOVE_LEFT;
		return NULL;
	}

	w=new Window();
	//use this to avoid comparision double numbers
	if	(lq->top_he==rq->top_he)
	{
		w->upsign=0;
	}
	else
		if	(lq->right_top_y>rq->left_top_y)
		{
			w->upsign=-1;
		}
		else//lq->right_top_y<rq->left_top_y
		{
			w->upsign=1;
		}

		if	(lq->btm_he==rq->btm_he)
		{
			w->downsign=0;
		}
		else
			if	(lq->right_btm_y>rq->left_btm_y)
			{
				w->downsign=-1;
			}
			else//lq->right_btm_y<rq->left_btm_y
			{
				w->downsign=1;
			}

			w->lq=lq;
			w->rq=rq;

			return w;
}
/******************************************************************
Given two vertices on a line,
get the y-value of vertex on line whose x-vaule is given too.

Using line equation (y-y1)/(x-x1)=(y2-y1)/(x2-x1)
y*(x2-x1)=(x-x1)*(y2-y1)+y1*(x2-x1)=(x-x1)*y2+y1*(x2-x)
******************************************************************/
//#EXACT
double	yOfLineGivenX(double x1,double y1,double x2,double y2,double x)	
{
	assert(x2-x1);
	return ((x-x1)*y2+y1*(x2-x))/(x2-x1);	
}

/******************************************************************
Point on the side of radial line: (x,y)'s position according to 
radial line (ox,oy) to (x1,y1)
return value: 
-1 left
0 on line
1 right
******************************************************************/
//#EXACT	this function requires exact computation
int	pointToLine(double x,double y,double ox,double oy,double x1,double y1)
{
	// translation, o(x1,y1) as the new Oriental point
	x=x-ox;	y=y-oy;
	x1=x1-ox;	y1=y1-oy;

	//rotation, op1 as the positive x direction
	// x'=cos(a)*x+sin(a)*y *sqrt(x1^2+y1^2)
	// y'=-sin(a)*x+cos(a)*y *sqrt(x1^2+y1^2)
	//-->
	//	x'=x1*x+y1*y
	//	y'=-y1*x+x1*y

	double D=y1*x-x1*y;
	if (D != 0)
		return (D > 0) ? 1 : -1;
	else
		return 0;
}

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
				  double cx,double cy)
{
	bx = bx- ax;
	by = by- ay;
	double bw = bx*bx + by*by;

	cx = cx- ax;
	cy = cy- ay;
	double cw = cx*cx + cy*cy; 

	double d1 = cy*bw - by*cw;
	double d2 = bx*cw - cx*bw;
	double d3 = by*cx - bx*cy;

	px = px - ax;
	py = py - ay;
	double pw = px*px + py*py;

	double D = px*d1 + py*d2 + pw*d3;

	if (D != 0)
		return (D > 0) ? 1 : -1;
	else
		return 0;
} 

/******************************************************************
is xq in between x1,x2, assume xq not eq x1 or x2
******************************************************************/
bool	inBetween(double xq,double x1,double x2)
{
	assert(xq-x1);
	assert(xq-x2);
	return((xq-x1)*(xq-x2)<0);
}

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
				 double	x2,double y2)
{
	double xx,yy;
	// translation, o(xo,yo) as the new Oriental point
	xq=xq-xo;	yq=yq-yo;
	x1=x1-xo;	y1=y1-yo;
	x2=x2-xo;	y2=y2-yo;

	// whether (xq,yq) is collinear with (x1,y1) and on the same side of (0,0)
	if	(	pointToLine(xq,yq,0,0,x1,y1)==0
		&&	!	inBetween(0,xq,x1)	)
		return	-1;
	if	(	pointToLine(xq,yq,0,0,x2,y2)==0
		&&	!	inBetween(0,xq,x2)	)
		return	-2;

	//rotation, op1 as the positive x direction
	// x'=cos(a)*x+sin(a)*y 
	// x'*sqrt(x1^2+y1^2)=x1*x+y1*y
	// y'=-sin(a)*x+cos(a)*y 
	// y'*sqrt(x1^2+y1^2)=-y1*x+x1*y
	xx=xq;	yy=yq;
	xq=x1*xx+y1*yy;
	yq=-y1*xx+x1*yy;

	xx=x2;	yy=y2;
	x2=x1*xx+y1*yy;
	y2=-y1*xx+x1*yy;

	double sign=xq*y2-x2*yq;
	// here we assume no collinear
	if(x2>=0)
	{
		if(y2>0)
		{
			// (x2,y2) is in 1st quadrant
			return(sign>0 && xq>0 && yq>0);
		}
		else
		{
			//4th quadrant
			return !(sign<0 && xq>0 && yq<0);
		}
	}
	else
	{
		if(y2>=0)
		{
			//2nd 
			return( yq>0 &&(xq>=0 || xq<0 && sign>0));
		}
		else
		{
			//3rd 
			return( yq>=0 || yq<0 && xq<0 && sign>0);
		}
	}
}

// for partition
void	swap(VPointer* array,int i,int j)
{
	VPointer tmpv;

	tmpv=array[i];
	array[i]=array[j];
	array[j]=tmpv;
	//modify index
	/* commented 08/02, so that array[i]->index remains the sort information
	array[i]->index=i;
	array[j]->index=j;
	*/
}

// for quick sort
int	partition(VPointer* array,int p,int r)
{
	double x;
	int	i;

	x=array[r]->pos.x;
	i=p-1;
	for (int j=p;j<r;j++)
	{
		if(array[j]->pos.x==x)
		{
			// vertices sharing same x values
			reportWarning("Not supporting vertiecs sharing same x value now!",__LINE__,__FILE__);
#ifdef	NOT_SUPPORT_SAME_X
			exit(-1);
#endif
		}
		else
			if(array[j]->pos.x<x)
			{
				i++;
				//	exchange array[i] and array[j]
				swap(array,i,j);
			}
	}
	swap(array,i+1,r);
	return i+1;
}

//*****************************************************************
//extension of geomtry primitive operations 
//*****************************************************************
/*
  The side of v to he
*/
int   vertexToEdge(VPointer v,HePointer he)
{
   Point   q=v->pos;
   Point   p1=he->getBackVertex()->pos;
   Point   p2=he->getFrontVertex()->pos;

   return pointToLine(q.x,q.y,p1.x,p1.y,p2.x,p2.y);
}

/*
  The side of v to edge (v1->v2)
*/
int   vertexToVertexVertex(VPointer v,VPointer v1,VPointer v2)
{
   Point   q=v->pos;
   Point   p1=v1->pos;
   Point   p2=v2->pos;

   return pointToLine(q.x,q.y,p1.x,p1.y,p2.x,p2.y);
}

/*
  The side of v to circumcircle of v1,v2,v3
*/
int   vertexToCircle(VPointer v,VPointer v1,VPointer v2,VPointer v3)
{
   Point   q=v->pos;
   Point   p1=v1->pos;
   Point   p2=v2->pos;
   Point   p3=v3->pos;
#ifdef   DETECT_COLLINEAR_DEGENERACY
   if   (pointToLine(p1.x,p1.y,p2.x,p2.y,p3.x,p3.y)==0)
      reportError("Three collinear points can no define a circumcircle.",__LINE__,__FILE__);
#endif
   return   pointToCircle(q.x,q.y,p1.x,p1.y,p2.x,p2.y,p3.x,p3.y);
}

/*
  test on whether edge he lies in angle from he1 to he2 
*/
int    edgeInAngle(HePointer he,HePointer he1,HePointer he2)
{
/*
#ifdef   DEBUG
   if   (!   (he->getBackVertex()==he1->getBackVertex() && he->getBackVertex()==he2->getBackVertex()) )
   {
      reportError("Wrong use of call to function edgeInAngle().",__LINE__,__FILE__);
   }
#endif
   */
//   return   pointInAngle(he->getBackVertex()->x,he->getBackVertex()->y,he->fv->x,he->fv->y,he1->fv->x,he1->fv->y,he2->fv->x,he2->fv->y);
   Point   o=he1->getBackVertex()->pos;
   Point   q=he->getFrontVertex()->pos;
   Point   p1=he1->getFrontVertex()->pos;
   Point   p2=he2->getFrontVertex()->pos;

   return   pointInAngle(o.x,o.y,q.x,q.y,p1.x,p1.y,p2.x,p2.y);
}

/* 
  test whether vertex v lies in the angle from (vo->v1) to (vo->v2)
*/
int   vertexInAngle(VPointer vo,VPointer v,VPointer v1,VPointer v2)
{
   Point   o=vo->pos;
   Point   q=v->pos;
   Point   p1=v1->pos;
   Point   p2=v2->pos;

   return   pointInAngle(o.x,o.y,q.x,q.y,p1.x,p1.y,p2.x,p2.y);
}


/*
// get the halfedge from v1 to v2
HPointer getHalfedge(VPointer v1,VPointer v2)
{
   HPointer tmpe;
   tmpe=v1->out;
   if   (tmpe==NULL)
   {
      return   NULL;
   }
   else
   {
      if   (tmpe->fv ==v2)   return   tmpe;
      tmpe=tmpe->clk;
      while   (tmpe->fv !=v2 &&   tmpe!=v1->out)
      {
         tmpe=tmpe->clk;
      }
      if   (tmpe==v1->out)
      {   return NULL;}
      else
      {   return   tmpe;}
   }
}
*/
//*****************************************************************
//implementation of primitives
//*****************************************************************
Vertex::Vertex(double _x,double _y,int   _i, int _extIdx)
:index(_i)
{
   pos.x=_x;
   pos.y=_y;
   out_he=NULL;
   valence=0;
   extIdx =_extIdx;
}
/*
Vertex::Vertex(const Point &p,int _i)
:pos(p),index(_i)
{
   out_he=NULL;
   valence=0;
}
*/
Vertex::~Vertex()
{
   /*
   while   (out_he!=NULL)
   {
      delete   out_he->e;
   }
   */
}

Halfedge::~Halfedge()
{
   //getBackVertex()->deleteFromOutHalfedgeList(this);
}


// utility to maintain the halfedge ring of vertex
// assume he is not added yet
void Vertex::addToOutHalfedgeList(HePointer he)
{
   if   (out_he==NULL)
   {
      he->next_he=he->pre_he=he;
      out_he=he;
      return;
   }

   if   (out_he->next_he==out_he)
   {
      out_he->next_he=he;
      out_he->pre_he=he;
      he->next_he=out_he;
      he->pre_he=out_he;
      return;
   }

   // insert in order of ring_circulation_order
   HePointer n_he;

   n_he=out_he->next_he;
   int flag;
   flag=edgeInAngle(he,n_he,n_he->pre_he);

   // he is in the angle span from n_he->pre_he to n_he
   while   (flag==0)
      //&& n_he!=out_he)   //we can always find it
   {
      //forward
      n_he=n_he->next_he;
      flag=edgeInAngle(he,n_he,n_he->pre_he);
   }

   if (flag!=1)
   {
      reportError("Collinear Degeneracy!",__LINE__,__FILE__);
   }
   //else we find inserting place, in fact we can always find it
   //INSERT_INBETWEEN(he,n_he->pre_he,n_he);
   he->next_he=n_he;
   he->pre_he=n_he->pre_he;
   n_he->pre_he->next_he=he;
   n_he->pre_he=he;
}

// not an interface for user
// assume he's back vertex is this v, v->out_he is not NULL
void Vertex::deleteFromOutHalfedgeList(HePointer he)
{
/*
#ifdef DEBUG
   assert(he);
   assert(he->getBackVertex()==this);   // this means this v has at least one out halfedge, so out_he!=NULL
#endif//DEBUG
*/
   if ( out_he == out_he->next_he )
   {out_he = NULL;}
    else if ( he == out_he )
      out_he = out_he->next_he;      // move out_he to next place
   
   //delete he from the list
   he->next_he->pre_he = he->pre_he;
   he->pre_he->next_he = he->next_he;
//   delete he;
}

// find the halfedge which point from this v to v2
HePointer Vertex::findOutHalfedgeInRing(VPointer v2)
{
#ifdef DEBUG
   assert(v2);
#endif//DEBUG
   if   (out_he==NULL)   return   NULL;
   HePointer   he=out_he->pre_he;
   while   (he!=out_he   &&   he->v!=v2)
      he=he->pre_he;
   if   (he->v==v2)   return   he;
   return   NULL;
}

// find the halfedge which point from v2 to this v 
HePointer Vertex::findInHalfedgeInRing(VPointer v2)
{
   HePointer he=findOutHalfedgeInRing(v2);
   if   (he!=NULL)   return   he->inv_he;
   return   NULL;
}

// not an interface to user, only called by new Edge()
// add halfedge and update all the topology information
// assume v->v2 is not added
HePointer   Vertex::addHalfedge(VPointer v2,EPointer e)
{
   
   HePointer   he=new   Halfedge(v2,e);

   Vector   d;
   d.x=v2->pos.x-this->pos.x;
   d.y=v2->pos.y-this->pos.y;

   he->d=d;

   valence++;
   //this will update next_he and pre_he of he
   addToOutHalfedgeList(he);

   //inv_he will be updated by new Edge()
   return he;
}

Edge::Edge(VPointer v1,VPointer v2,int _type, bool isExt)
:type(_type)
{
   iter=NULL;
   if   (v1->getIndex()<v2->getIndex())
   {
      // add halfedge
      he[0]=v1->addHalfedge(v2,this);
      he[1]=v2->addHalfedge(v1,this);
   }
   else
   {
      he[0]=v2->addHalfedge(v1,this);
      he[1]=v1->addHalfedge(v2,this);
   }
   he[0]->inv_he=he[1];
   he[1]->inv_he=he[0];

   isExternal = isExt;
}

//delete related halfedges
Edge::~Edge()
{
   he[0]->v->deleteFromOutHalfedgeList(he[1]);
   he[1]->v->deleteFromOutHalfedgeList(he[0]);

   delete he[0];
   delete he[1];

}

Slab::Slab(int li,int ri)
:l(li),r(ri)
{
   q_head=q_tail=NULL;
   next=NULL;
}

//assume q_head is not null
void Slab::pushBackQuad(QPointer q)
{
/*#ifdef   DEBUG
   assert(q_tail!=NULL);
#endif
*/   
   if   (q_tail==NULL)
   {
      q_head=q_tail=q;
   }
   else
   {
      q_tail->next=q;
      q_tail=q;
   }
}

Quad::Quad(VPointer hv,VPointer lv,HePointer t,HePointer b,int   _l,int _r)
:top_v(hv),btm_v(lv),top_he(t),btm_he(b),l(_l),r(_r)
{
   left_top_y=left_btm_y=right_top_y=right_btm_y=0;
   next=NULL;
}

Quad::Quad()
{
   top_v=btm_v=NULL;
   top_he=btm_he=NULL;
   l=r=0;
   left_top_y=left_btm_y=right_top_y=right_btm_y=0;
   next=NULL;
}

void Quad::updateLongEdge(EPointer e)
{
   double ey;   // y value of intersection point of e and vertical line through x 

   VPointer v1=e->he[1]->getFrontVertex();
   VPointer v2=e->he[0]->getFrontVertex();

   // y value of the intersection of e and the vertical line through (x,y)
   ey=yOfLineGivenX(v1->pos.x,v1->pos.y,v2->pos.x,v2->pos.y,top_v->pos.x);


   if   (ey>top_v->pos.y   &&   ey<top_y)
   {
      top_he=e->he[1];
      top_y=ey;
   }
   else
   if   (ey<btm_v->pos.y   &&   ey>btm_y)
   {
      btm_he=e->he[0];
      btm_y=ey;
   }
   else
   if   (ey==top_y   ||   ey==btm_y)
   {
      reportError("Intersection detected!",__LINE__,__FILE__);
   }
#ifdef   DEBUG
   else
   if   (ey==top_v->pos.y   ||   ey==btm_v->pos.y)
   {
      reportWarningString("Collinear degeneracy but does not matter the program!");
   }
#endif
}

void Quad::computeCornorY(double lx,double rx)
{
   VPointer v1=top_he->getFrontVertex();
   VPointer v2=top_he->getBackVertex();

   // y value of the intersection of e and the vertical line through (x,y)
   this->left_top_y=yOfLineGivenX(v1->pos.x,v1->pos.y,v2->pos.x,v2->pos.y,lx);
   this->right_top_y=yOfLineGivenX(v1->pos.x,v1->pos.y,v2->pos.x,v2->pos.y,rx);

   v1=btm_he->getBackVertex();
   v2=btm_he->getFrontVertex();
   this->left_btm_y=yOfLineGivenX(v1->pos.x,v1->pos.y,v2->pos.x,v2->pos.y,lx);
   this->right_btm_y=yOfLineGivenX(v1->pos.x,v1->pos.y,v2->pos.x,v2->pos.y,rx);
}

Mesh::Mesh()
{
   e_head=e_tail=NULL;
   v_num=e_num=ce_num=0;
   maxs.y=maxs.x=HUGE_VAL;
   mins.y=mins.x=-HUGE_VAL;
   v_array=NULL;
   sep_array=NULL;
}

//***************** quick sort functions end  ***********************
// sort vertices by x-coordinate
void Mesh::sortVertices()
{
   // implemented in quick sort
   quicksort(v_array,0,v_num-1);      
   // update order information of vertices
/*not here
   for   (int   vj=0;vj<v_num;vj++)
   {
      v_array[vj]->setIndex(vj);
   }*/

}

EPointer Mesh::addEdge(VPointer v1,VPointer v2,int type, bool isExt)
{
   
//#ifdef   DEBUG
   // no conflicts
   if   (! (v1->findOutHalfedgeInRing(v2)==NULL   ) )//&&   v2->findOutHalfedgeInRing(v1)==NULL))
   {
      reportError("Duplicate edge!",__LINE__,__FILE__);
	  //return v1->findOutHalfedgeInRing(v2)->e;
   }
//#endif

   EPointer   e;
   EListPointer   el;
   
   // virtual edge are not take into account
   if   (type==VIRTUAL)
   {
      e=new Edge(v1,v2,CONSTRAINED,isExt);
   }
   else
   {
      e=new Edge(v1,v2,type,isExt);

      e_num++;
   
      el=new EdgeList(e);

      if   (type==CONSTRAINED)   ce_num++;

      if   (e_head==NULL)
      {
         e_head=e_tail=el;
      }
      else
      {
         e_tail->next=el;
         e_tail=el;
      }
   }
#ifdef OUTPUT
   printf("Edge added:(%d,%d)\n",v1->getIndex(),v2->getIndex());
#endif
   return e;
}

void   Mesh::deleteEdge(EPointer e)
{
   EListPointer el;
   el=e->iter;

#ifdef   OUTPUT
   printf("Delete edge:(%d,%d)\n",e->he[1]->v->getIndex(),e->he[0]->v->getIndex());
#endif

   if   (e_head==el)
   {
      if   (e_tail==e_head)
         e_tail=NULL;

      e_head=e_head->next;
   }
   else
   {
      EListPointer tmpe;
      tmpe=e_head;
      while   (tmpe->next!=el)
         tmpe=tmpe->next;
      
      if   (e_tail==el)
      {
         e_tail=tmpe;
      }
      
      tmpe->next=el->next;
   }
   
   delete e;
}

/* Read the source file and convert to data structure
   Input: sf is the source file name, the format refers to above
   Output:   0 means no error
*/
int   Mesh::import2OFFFile(char* sf){
   FILE * fp;

   if
   ((fp=fopen(sf,"r"))==NULL)
   {
      reportError("Cannot open this file!",__LINE__,__FILE__);
   }
   else
   {

/*      char   line[LINE_LENGTH];

      fgets(line,LINE_LENGTH,fp);
*/
      omitComment(fp);

      // read OFF, currently only support 'OFF'
      // file name length is maximum 256
      char   keyword[256];
      fscanf(fp,"%s",keyword);

      if   (strcmp((keyword),noff_keyword)!=0 )
      {
         reportError("Not valid format(*.noff)!",__LINE__,__FILE__);
      }

      readToLineEnd(fp);
      omitComment(fp);

      int   _d;
      fscanf(fp,"%d",&_d);
      if   (_d!=2)
      {
         reportError("Not valid dimension!",__LINE__,__FILE__);
      }
      readToLineEnd(fp);
      omitComment(fp);

      int   vnum;
      // read vertices count, faces count and edges count respectively
      // read num
      fscanf(fp,"%d",&vnum);
      if   (vnum<1)
      {
         reportError("Vertex number should above zero.",__LINE__,__FILE__);
      }

      v_array=new VPointer[vnum];

      int   fnum;   // f_num is not checked
      fscanf(fp,"%d",&fnum);

      int cenum;
      fscanf(fp,"%d",&cenum);
      readToLineEnd(fp);
      omitComment(fp);

      double   x;
      double   y;

      //init 
      mins.x=mins.y=HUGE_VAL;
      maxs.x=maxs.y=-HUGE_VAL;

      // read information of vertices
      for (int i=0;i<vnum;i++)
      {
         
         fscanf(fp,"%lf%lf",&x,&y);

         maxs.x=MAX(maxs.x,x);
         maxs.y=MAX(maxs.y,y);
         mins.x=MIN(mins.x,x);
         mins.y=MIN(mins.y,y);

         v_array[i]=new Vertex(x,y,i);
         v_num++;

         readToLineEnd(fp);         
         
         omitComment(fp);
      }

      //vnum should eq v_num now

      //sort the vertex
      sortVertices();

      //get the information of sorting
      int*   reorder=new int[v_num];
      for   (int ni=0;ni<v_num;ni++)
      {
         reorder[v_array[ni]->getIndex()]=ni;
      }

      //update the information of vertices;
      for   (int nj=0;nj<v_num;nj++)
      {
         v_array[nj]->setIndex(nj);
      }
      //omit faces
      int   side_num;
      for   (int   f_i=0;f_i<fnum;f_i++)
      {
         //read side num
         fscanf(fp,"%d",&side_num);
         int vi;
         for   (int v_i=0;v_i<side_num;v_i++)
         {
            fscanf(fp,"%d",&vi);
         }
            readToLineEnd(fp);         
            omitComment(fp);
      }

      //read constrained edge
      for (int j=0;j<cenum;j++)
      {
         int   i1,i2;

         //get first constrained edge
         fscanf(fp,"%d%d",&i1,&i2);
      
         if   (i1<0 || i1>=v_num || i2<0 || i2>=v_num)
               reportError("Incorrect input of edge's endpoint's index. It should not below 0 or large or equal to vertex total count.",__LINE__,__FILE__);      

         addEdge(v_array[reorder[i1]],v_array[reorder[i2]],CONSTRAINED);

         readToLineEnd(fp);
         omitComment(fp);
      }   

      //ce_num shoulde eq cenum now

      fclose(fp);   
      //sort the v_array while let the index in each vertex unchanged
   }

   return(0);
}

int   Mesh::export2OFFFile(char* sf,char* f)
{
   char delimiter=' ';
   char* df="result.noff";
   FILE* fp;   

   if(f!=NULL)
   {
      df=f;   
   }

   if
   ((fp=fopen(df,"w"))==NULL)
   {
      reportError("Cannot create or overwrite this file!",__LINE__,__FILE__);
   }
   else
   {
      // add a mark in the edge: writed
#ifdef   COMMENT_ON
      fprintf(fp,"# generated from source file: %s \n",sf);
#endif
      fprintf(fp,"%s\n",noff_keyword);
      fprintf(fp,"%d\n",2);

      //output vertex count
#ifdef   COMMENT_ON
      fprintf(fp,"%s\n","# vertices num, face num=0, edge num");
#endif
      fprintf(fp,"%d%c%d%c%d\n",v_num,delimiter,0,delimiter,e_num);

      //output vertex information in form of x, y value
      for (int i=0;i<v_num;i++)
      {
#ifdef   COMMENT_ON
         fprintf(fp,"%s %d\n","# vertex index: ",i);
#endif
         fprintf(fp,"%f %f\n",v_array[i]->pos.x,v_array[i]->pos.y);
      }
      

      //output edge information in form of i1,i2 value
      EListPointer el;
      el=e_head;
      while(el!=NULL)
      {
         VPointer v1=el->e->he[0]->getBackVertex();
         VPointer v2=el->e->he[0]->getFrontVertex();
#ifdef   COMMENT_ON

         fprintf(fp,"%s%f,%f%s%f,%f%s\n","# (",v1->pos.x,v1->pos.y,")--(",v2->pos.x,v2->pos.y,")");
#endif
         fprintf(fp,"%d %d\n",v1->getIndex(),v2->getIndex());

         el=el->next;
      }

      fclose(fp);   
   }
   return (0);
}

/* return 0 means no error*/
int Mesh::initSlabs()
{
   // the index itself shows it's position
//   sortVertices();

#define   OFFSET 1

   //index is -2,-1
   vlt=new Vertex(mins.x-3*OFFSET,maxs.y+OFFSET,-2);
   vlb=new Vertex(mins.x-2*OFFSET,mins.y-OFFSET,-1);

   //index is v_num+1,v_num
   vrt=new Vertex(maxs.x+3*OFFSET,maxs.y+OFFSET,v_num+1);
   vrb=new Vertex(maxs.x+2*OFFSET,mins.y-OFFSET,v_num);

   // 2 long horizontal lines
   sky_e=addEdge(vlt,vrt,VIRTUAL);
   eth_e=addEdge(vlb,vrb,VIRTUAL);

   //init separate lines
   sep_array=new double[v_num+1];

   sep_array[0]=mins.x-OFFSET;
   sep_array[v_num]=maxs.x+OFFSET;
#ifdef DEBUG
      printf("%s%d%s%f\n","# add separator line ",0," x:=",sep_array[0]);
      printf("%s%d%s%f\n","# add separator line ",v_num," x:=",sep_array[v_num]);
#endif

   double x1;
   double x2;
   x1=v_array[0]->pos.x;

   for (int v_i=1;v_i<v_num;v_i++)
   {   
      x2=v_array[v_i]->pos.x;
      sep_array[v_i]=(x1+x2)/2;
      x1=x2;
#ifdef DEBUG
      printf("%s%d%s%f\n","# add separator line ",v_i," x:=",sep_array[v_i]);
#endif
   }

   //init slabs
   SPointer*slab_array=new SPointer[v_num];
   // create an array to accelerate the initialization of quad and slab
   QPointer tmpq;
   VPointer tmpv;

   //other slabs
   for (int v_j=0;v_j<v_num;v_j++)
   {
      slab_array[v_j]=new Slab(v_j,v_j+1);
      //initially, each quad has sky and earth as its top and bottom halfedge
      tmpv=v_array[v_j];
      tmpq=new Quad(tmpv,tmpv,sky_e->he[1],eth_e->he[0],v_j,v_j+1);
      tmpq->top_y=maxs.y+OFFSET;
      tmpq->btm_y=mins.y-OFFSET;

      slab_array[v_j]->q_head=slab_array[v_j]->q_tail=tmpq;
   }

   for (int k=0;k<v_num-1;k++)
   {
      slab_array[k]->next=slab_array[k+1];
   }

   // init slab list's head
   s_head=slab_array[0];

   //   init quad for each slab by comparing each constrained edges indices to the indices of separator lines
   EListPointer   tmpe;
   tmpe=e_head;
   while(tmpe!=NULL)
   {
      int   l,r;
      l=tmpe->e->he[1]->v->getIndex();
      r=tmpe->e->he[0]->v->getIndex();

      for(int m=l+1;m<r;m++)
      {
         // update slab m's initial quad with constrained edge tmpe->e
         slab_array[m]->q_head->updateLongEdge(tmpe->e);
      }
      tmpe=tmpe->next;
   }

   // update quad's cornor's y value
   for (int s_i=0;s_i<v_num;s_i++)
      slab_array[s_i]->q_head->computeCornorY(sep_array[s_i],sep_array[s_i+1]);

   //release slab_array's space, not the slab's space
   delete []slab_array;
   return 0;
}

//get the hull edge here is different from Halfedge::getNext()
// here, we want to find one halfedge he for v such that
//if v->index <sep_index
// he->v is (for lowside=true,below otherwise) 
// the first halfedge just right to the lower vertical ray start from v
// if no such halfedge exist, return null
//# EXACT
HePointer   Mesh::getHullHalfedge(VPointer v,int sep_index,bool lowside)
{
   HePointer   he;
   HePointer   tmphe;

   Vector   in;

   int   v_i;
   v_i=v->getIndex();
   int   v_j;

   he=v->out_he;
   if   (he==NULL)   return   NULL;

   v_j=he->v->getIndex();

   bool   leftv;
   leftv=v_i<sep_index;

   //if just on the same horizontal
   if   (he->d.x==0)
   {
      return   he;
   }

   bool inangle;
   //else
   //here we used the property that no two vertices has the same x value
   // so the loop below won't run forever
   if   (lowside)
   {
      in.x=Y.x;
      in.y=-Y.y;
      if   (leftv)
      {
         tmphe=he->next_he;
         if   (tmphe!=he)   //test if there is only one halfedge
         {
            inangle=pointInAngle(0,0,in.x,in.y,tmphe->d.x,tmphe->d.y,he->d.x,he->d.y) == 1;
            while   (!inangle)
            {
               he=tmphe;
               tmphe=tmphe->next_he;
               inangle=pointInAngle(0,0,in.x,in.y,tmphe->d.x,tmphe->d.y,he->d.x,he->d.y) == 1;
            }
         }
      }
      else
      {
         tmphe=he->next_he;
         if   (tmphe!=he)   //test if there is only one halfedge
         {
            inangle=pointInAngle(0,0,in.x,in.y,tmphe->d.x,tmphe->d.y,he->d.x,he->d.y) == 1;
            while   (!inangle)
            {
               he=tmphe;
               tmphe=tmphe->next_he;
               inangle=pointInAngle(0,0,in.x,in.y,tmphe->d.x,tmphe->d.y,he->d.x,he->d.y) == 1;
            }
            //right, then set the left one
            he=tmphe;
         }
      }
   }
   else
   {
      in.x=Y.x;
      in.y=Y.y;
      if   (leftv)
      {
         tmphe=he->next_he;
         if   (tmphe!=he)   //test if there is only one halfedge
         {
            inangle=pointInAngle(0,0,in.x,in.y,tmphe->d.x,tmphe->d.y,he->d.x,he->d.y) == 1;
            while   (!inangle)
            {
               he=tmphe;
               tmphe=tmphe->next_he;
               inangle=pointInAngle(0,0,in.x,in.y,tmphe->d.x,tmphe->d.y,he->d.x,he->d.y) == 1;
            }
         }
         //left, then set the right one
         he=tmphe;
      }
      else
      {
         tmphe=he->next_he;
         if   (tmphe!=he)   //test if there is only one halfedge
         {
            inangle=pointInAngle(0,0,in.x,in.y,tmphe->d.x,tmphe->d.y,he->d.x,he->d.y) == 1;
            while   (!inangle)
            {
               he=tmphe;
               tmphe=tmphe->next_he;
               inangle=pointInAngle(0,0,in.x,in.y,tmphe->d.x,tmphe->d.y,he->d.x,he->d.y) == 1;
            }
         }
      }
   }
   return   he;
}

// return he:
// if he->v->index is not in (sep_index-1, v->index) when v->=index>sep_index
// return NULL
HePointer Mesh::getHullHalfedgeNextTo(HePointer _he,VPointer v,int l,int sep_index,int r,bool lowside)
{
#ifdef   DEBUG
   assert(_he->getBackVertex()==v);
   assert(v->out_he!=NULL);
#endif
   HePointer he;
   
   int   v_i;
   v_i=v->getIndex();
   int   v_j;

   he=v->out_he;

   if   (he->next_he==v->out_he)   return NULL;

   v_j=he->v->getIndex();

   bool   leftv;
   leftv=v_i<sep_index;

   while   (he!=_he)
   {
      //if there is a bug, it will loop forever
      he=he->pre_he;
   }

   //now he=_he;
   if   (leftv)
   {
      if   (lowside)
      {
         he=he->pre_he;
         if   (he->v->getIndex()<v->getIndex()
            ||   he->v->getIndex()>=r
            ||   pointInAngle(v->pos.x,v->pos.y,he->v->pos.x,he->v->pos.y,v->pos.x,v->pos.y-1,_he->v->pos.x,_he->v->pos.y))
         {
            he=NULL;
         }
      }
      else
      {
         he=he->next_he;
         if   (he->v->getIndex()<v->getIndex()
            ||   he->v->getIndex()>=r
            ||   pointInAngle(v->pos.x,v->pos.y,he->v->pos.x,he->v->pos.y,_he->v->pos.x,_he->v->pos.y,v->pos.x,v->pos.y+1))
         {
            he=NULL;
         }
      }
//      v_j=he->v->getIndex();
//      return   (v_j>v_i&&v_j<sep_index)?he:NULL;
      return   he;
   }
   else
   {
      if   (lowside)
      {
         he=he->next_he;
         if   (he->v->getIndex()>v->getIndex()
            ||   he->v->getIndex()<l
            ||   pointInAngle(v->pos.x,v->pos.y,he->v->pos.x,he->v->pos.y,v->pos.x,v->pos.y+1,_he->v->pos.x,_he->v->pos.y))
         {
            he=NULL;
         }
      }
      else
      {
         he=he->pre_he;
         if   (he->v->getIndex()>v->getIndex()
            ||   he->v->getIndex()<l
            ||   pointInAngle(v->pos.x,v->pos.y,he->v->pos.x,he->v->pos.y,_he->v->pos.x,_he->v->pos.y,v->pos.x,v->pos.y-1))
         {
            he=NULL;
         }
      }
//      v_j=he->v->getIndex();
//      return   (v_j>=sep_index&&v_j<v_i)?he:NULL;
      return   he;
   }
}

// In this function, we get the initial candidate pair
// and corresponding halfedge on the hull which we can use to advance
// this procedure request too much exact computation!!!
// see 'diff' below
//# EXACT
void   Mesh::findInitialCandidatePair(WPointer w,
      VPointer &p0,VPointer &q0,
      HePointer &p_he,HePointer &q_he,
      bool   lowside)
{
   int   sign;

   //care!, here I flip the upsign to make this function more general to use
   sign=lowside?   w->downsign:   -(w->upsign);

   int   m=w->lq->r;
   int   l,r;
   l=w->lq->l;
   r=w->rq->r;
   HePointer   he;
   double   diff;
   bool test;

   //in this switch
   // we get the lowset(highest) vertices on either side
   // and corresponding halfedges
   //while there may be situation that more than one vertex is on the same horizontal line
   // then we need to adjust it:
   //for left, we move vertex to right most one and update its halfedge
   //for right, vice versa

   //find the lowest vertex
   switch(sign)
   {
   case   0:
      if   (lowside)
      {
         p0=w->lq->btm_v;
         q0=w->rq->btm_v;
      }
      else
      {
         p0=w->lq->top_v;
         q0=w->rq->top_v;
      }

      //get the halfedge of v between the vertical line through v->index and separate line
      //if lower side, it should be the first counter-clockwisely which is above v, otherwise vice versa
      // also may be horizental
      // possbile is null
      p_he=getHullHalfedge(p0,m,lowside);
      q_he=getHullHalfedge(q0,m,lowside);
      break;
   case   1:
      // q0 can be settled
      if   (lowside)
      {
         //right is higher, q0 is settled
         q0=w->rq->btm_v;
         //w->rq->top_he->v is a vertex in w->lq
         he=w->rq->btm_he;
         p0=he->inv_he->v;
      }
      else
      {
         //right is lower, 
         q0=w->rq->top_v;
         he=w->rq->top_he->inv_he;
         p0=he->inv_he->v;
      }
      q_he=getHullHalfedge(q0,m,lowside);
      //p_he is a halfedge of p0 which lies just above(if lowside=true)he
      //and between p0 and sep[m]
      p_he=getHullHalfedgeNextTo(he,p0,l,m,r,lowside);

      if   (p_he!=NULL&&(p_he->v->getIndex()<m&&p_he->v->getIndex()>p0->getIndex()))
      {
         //whether next v is lower(if lowside=true) than current, if so, advance
         //here although the inverse of '<' is '>=' instead of '>', 
         // it still work because we exclude that case
         diff=p_he->v->pos.y - p0->pos.y;
         if   (diff!=0)
         {
            test=diff<0;
            test=(lowside)?test:!test;
            //if test==true, the very special case occur
            //assume lowside==true, then
            // we know the lowest vertex can be find by repeating comparing y value
            // and it will never go back, it can be only lower and lower to the right side of v
            while   (test)//&&p_he!=NULL)
            {
               //advance
               p0=p_he->v;
               if(lowside)
               {
                  p_he=p_he->inv_he->pre_he;
               }
               else
               {
                  p_he=p_he->inv_he->next_he;
               }

               if   (p_he->v->getIndex()>=m)
               {
               //   p_he=NULL;
                  test=false;
               }
               else
               {
                  //test again
               diff=p_he->v->pos.y - p0->pos.y;
               if   (diff==0)
               {
                  test=false;
               }
               else
               {
                  test=diff<0;
                  test=(lowside)?test:!test;
               }
            }
            }
         }
         /*
         if   (p_he!=NULL   &&(p_he->v->getIndex()>=m||p_he->v->getIndex()<p0->getIndex()) )
         {
            //has found some lowest(if lowside=true) vertex
            //while its hull edge intersect the middle separator line
            p_he=NULL;
         }
         */
      }
      break;
   case   -1:
      // p0 can be settled
      if   (lowside)
      {
         //left is higher
         p0=w->lq->btm_v;
         //w->rq->top_he->v is a vertex in w->lq
         he=w->lq->btm_he->inv_he;
         q0=he->inv_he->v;
      }
      else
      {
         //right is lower
         p0=w->lq->top_v;
         he=w->lq->top_he;
         q0=he->inv_he->v;
      }
      p_he=getHullHalfedge(p0,m,lowside);
      q_he=getHullHalfedgeNextTo(he,q0,l,m,r,lowside);

      if   (q_he!=NULL   && (q_he->v->getIndex()>=m&&q_he->v->getIndex()<q0->getIndex()) )
      {
         diff=q_he->v->pos.y - q0->pos.y;
         if   (diff!=0)
         {
            test=diff<0;
            test=(lowside)?test:!test;
            while   (test)//&&q_he!=NULL)
            {
               //advance
               q0=q_he->v;
               if(lowside)
               {
                  q_he=q_he->inv_he->next_he;
               }
               else
               {
                  q_he=q_he->inv_he->pre_he;
               }

               if   (q_he->v->getIndex()<m)
               {
               //   q_he=NULL;
                  test=false;
               }
               else
               {
                  //test again
                  diff=q_he->v->pos.y - q0->pos.y;
                  if   (diff==0)
                  {
                     test=false;
                  }
                  else
                  {
                     test=diff<0;
                     test=(lowside)?test:!test;
                  }
               }
            }
         }
         /*
         if   (q_he!=NULL   && (q_he->v->getIndex()<m||q_he->v->getIndex()>q0->getIndex()) )
         {
            q_he=NULL;
         }
         */
      }
      break;
   default:   break;
   }

   //if there are vertices which have same lowest y value, 
   //move to the vertex closet to middle separate line
   if   (p_he!=NULL)
   {
      //since we have check in the above
      //here the p_he->v is in the middle of p0 and separater line m
      // we only advance p0 if p_he->v is right to p0, left to m and diff==0
      test=(   p_he->v->getIndex()>p0->getIndex()   
         &&   p_he->v->getIndex()<m
         &&   p_he->v->pos.y == p0->pos.y   );

      while   (test)
      {
         //advance
         p0=p_he->v;
         if(lowside)
         {
            p_he=p_he->inv_he->pre_he;
         }
         else
         {
            p_he=p_he->inv_he->next_he;
         }

         if   (   p_he->v->getIndex()>=m
            ||   p_he->v->getIndex()<p0->getIndex())
         {
         //   p_he=NULL;
            test=false;
         }
         else
         {
            //test again
            test=(p_he->v->pos.y == p0->pos.y);
         }
      }
   }
   //advance q0 in similar way
   if   (q_he!=NULL)
   {
      test=(   q_he->v->getIndex()<q0->getIndex()   
         &&   q_he->v->getIndex()>=m
         &&   q_he->v->pos.y == q0->pos.y   );

      while   (test)
      {
         //advance
         q0=q_he->v;
         if(lowside)
         {
            q_he=q_he->inv_he->next_he;
         }
         else
         {
            q_he=q_he->inv_he->pre_he;
         }

         if   (   q_he->v->getIndex()<m
            ||   q_he->v->getIndex()>q0->getIndex())
         {
         //   q_he=NULL;
            test=false;
         }
         else
         {
            //test again
            test=(q_he->v->pos.y == q0->pos.y);
         }
      }
   }
}

EPointer   Mesh::advancePairToBridge(WPointer w,VPointer p0,VPointer q0,HePointer p_he,HePointer q_he,bool lowside)
{
   double   dy;
   dy=(q0->pos.y - p0->pos.y);
   HePointer he;

   if   (q_he==NULL   &&   p_he==NULL)
   {
      he=p0->findOutHalfedgeInRing(q0);
      return   (he==NULL)?addEdge(p0,q0):he->e;
   }

   if   (dy==0   )
   {
      he=p0->findOutHalfedgeInRing(q0);
      return   (he==NULL)?addEdge(p0,q0):he->e;
   }

   //else dy<>0
   int   l,m,r;
   l=w->lq->l;
   m=w->rq->l;
   r=w->rq->r;

   bool   posSlope;
   posSlope=dy>0;

   HePointer   tmphe;
   int   v_i;
   if   (lowside)
   {
      if   (posSlope)
      {
         if   (q_he!=NULL)
         {
            //(q_he!=NULL)
            //candidate search given one
            tmphe=q_he->pre_he;
            v_i=tmphe->v->getIndex();
            if   (v_i<q0->getIndex()   ||   v_i>=r)
               tmphe=NULL;

            while(tmphe!=NULL
               &&   (vertexToVertexVertex(tmphe->v,p0,q0)==1))
            {
               q0=tmphe->v;
               tmphe=tmphe->inv_he->pre_he;
               v_i=tmphe->v->getIndex();
               if   (v_i<q0->getIndex()   ||   v_i>=r)
                  tmphe=NULL;
            }
            q_he=tmphe;
         }
         if   (p_he!=NULL)
         {
         // p_he is in wedge, must advance
            int   v_i=p_he->v->getIndex();

            if   (v_i>=m   || v_i<l)
               p_he=NULL;
         }
         while   (p_he!=NULL   
            &&   (vertexToVertexVertex(p_he->v,p0,q0)!=-1))
         {
            p0=p_he->v;
            p_he=p_he->inv_he->pre_he;
            int   v_i=p_he->v->getIndex();

            if   (v_i>=m   || v_i<p0->getIndex())
               p_he=NULL;
            
            if   (q_he!=NULL)
            {
               //advance ,same to above
               //candidate search given one
//!!!changed. fix bug 1204, it is not same to above, actually, here we don't need to initialize q_he like above
//               tmphe=q_he->pre_he;
//               v_i=tmphe->v->getIndex();
//               if   (v_i<q0->getIndex()   ||   v_i>=r)
//                  tmphe=NULL;
//replaced with
//since we have checked tmphe->v->index above, we don't need to do it here
               tmphe=q_he;

               while(tmphe!=NULL
                  &&   (vertexToVertexVertex(tmphe->v,p0,q0)==1))
               {
                  q0=tmphe->v;
                  tmphe=tmphe->inv_he->pre_he;
                  v_i=tmphe->v->getIndex();
                  if   (v_i<q0->getIndex()   ||   v_i>=r)
                     tmphe=NULL;
               }
               q_he=tmphe;
            }
         }
         //find it,test whether exists
         tmphe=p0->findOutHalfedgeInRing(q0);
         return   (tmphe==NULL)?addEdge(p0,q0):tmphe->e;
      }
      else
      {
         if   (p_he!=NULL)
         {
            //(p_he!=NULL)
            //candidate search given one
            tmphe=p_he->next_he;
            v_i=tmphe->v->getIndex();
            if   (v_i>p0->getIndex()   ||   v_i<l)
               tmphe=NULL;

            while(tmphe!=NULL
               &&   (vertexToVertexVertex(tmphe->v,p0,q0)==1))
            {
               p0=tmphe->v;
               tmphe=tmphe->inv_he->next_he;

               v_i=tmphe->v->getIndex();
               if   (v_i>p0->getIndex()   ||   v_i<l)
                  tmphe=NULL;
            }
            p_he=tmphe;
         }
         if   (q_he!=NULL)
         {
            int   v_i=q_he->v->getIndex();

            if   (v_i<m   ||v_i>=r)
               q_he=NULL;
         }
         // p_he is in wedge, must advance
         while   (q_he!=NULL   
            &&   (vertexToVertexVertex(q_he->v,p0,q0)!=-1))
         {
            q0=q_he->v;
            q_he=q_he->inv_he->next_he;
            int   v_i=q_he->v->getIndex();

            if   (v_i<m   ||v_i>q0->getIndex())
               q_he=NULL;
            
            if   (p_he!=NULL)
            {
               //advance ,same to above
               //candidate search given one
//!!!changed. fix bug 1204, it is not same to above, actually, here we don't need to initialize q_he like above
//               tmphe=p_he->next_he;
//               v_i=tmphe->v->getIndex();
//               if   (v_i>p0->getIndex()   ||   v_i<l)
//                  tmphe=NULL;
//replaced with
//since we have checked tmphe->v->index above, we don't need to do it here
               tmphe=p_he;

               while(tmphe!=NULL
                  &&   (vertexToVertexVertex(tmphe->v,p0,q0)==1))
               {
                  p0=tmphe->v;
                  tmphe=tmphe->inv_he->next_he;

                  v_i=tmphe->v->getIndex();
                  if   (v_i>p0->getIndex()   ||   v_i<l)
                     tmphe=NULL;
               }
               p_he=tmphe;
            }
         }
         //find it,test whether exists
         tmphe=p0->findOutHalfedgeInRing(q0);
         return   (tmphe==NULL)?addEdge(p0,q0):tmphe->e;
      }
   }
   else
   {
      if   (!posSlope)
      {
         if   (q_he!=NULL)
         {
            //(q_he!=NULL)
            //candidate search given one
            tmphe=q_he->next_he;
            v_i=tmphe->v->getIndex();
            if   (v_i<q0->getIndex()   ||   v_i>=r)
               tmphe=NULL;

            while(tmphe!=NULL
               &&   (vertexToVertexVertex(tmphe->v,p0,q0)==-1))
            {
               q0=tmphe->v;
               tmphe=tmphe->inv_he->next_he;
               v_i=tmphe->v->getIndex();
               if   (v_i<q0->getIndex()   ||   v_i>=r)
                  tmphe=NULL;
            }
            q_he=tmphe;
         }
         if   (p_he!=NULL)
         {
            int   v_i=p_he->v->getIndex();

            if   (v_i>=m   || v_i<l)
               p_he=NULL;
         }
         // p_he is in wedge, must advance
         while   (p_he!=NULL   
            &&   (vertexToVertexVertex(p_he->v,p0,q0)!=1))
         {
            p0=p_he->v;
            p_he=p_he->inv_he->next_he;
            int   v_i=p_he->v->getIndex();

            if   (v_i>=m   || v_i<p0->getIndex())
               p_he=NULL;
            
            if   (q_he!=NULL)
            {
               //advance ,same to above
               //candidate search given one
//!!!changed. fix bug 1204, it is not same to above, actually, here we don't need to initialize q_he like above
//               tmphe=q_he->pre_he;
//               v_i=tmphe->v->getIndex();
//               if   (v_i<q0->getIndex()   ||   v_i>=r)
//                  tmphe=NULL;
//replaced with
//since we have checked tmphe->v->index above, we don't need to do it here
               tmphe=q_he;

               while(tmphe!=NULL
                  &&   (vertexToVertexVertex(tmphe->v,p0,q0)==-1))
               {
                  q0=tmphe->v;
                  tmphe=tmphe->inv_he->next_he;
                  v_i=tmphe->v->getIndex();
                  if   (v_i<q0->getIndex()   ||   v_i>=r)
                     tmphe=NULL;
               }
               q_he=tmphe;
            }
         }
         //find it,test whether exists
         tmphe=p0->findOutHalfedgeInRing(q0);
         return   (tmphe==NULL)?addEdge(p0,q0):tmphe->e;
      }
      else
      {
         //(p_he!=NULL)
         //actually the following just change the above counter-part 
         // all next_he to pre_he, all 1 to -1

         if   (p_he!=NULL)
         {
            //candidate search given one
            tmphe=p_he->pre_he;
            v_i=tmphe->v->getIndex();
            if   (v_i>p0->getIndex()   ||   v_i<l)
               tmphe=NULL;

            while(tmphe!=NULL
               &&   (vertexToVertexVertex(tmphe->v,p0,q0)==-1))
            {
               p0=tmphe->v;
               tmphe=tmphe->inv_he->pre_he;

               v_i=tmphe->v->getIndex();
               if   (v_i>p0->getIndex()   ||   v_i<l)
                  tmphe=NULL;
            }
            p_he=tmphe;
         }
         if   (q_he!=NULL)
         {
            int   v_i=q_he->v->getIndex();
            if   (v_i<m   ||v_i>=r)
               q_he=NULL;
         }
         // p_he is in wedge, must advance
         while   (q_he!=NULL   
            &&   (vertexToVertexVertex(q_he->v,p0,q0)!=1))
         {
            q0=q_he->v;
            q_he=q_he->inv_he->pre_he;

            int   v_i=q_he->v->getIndex();
            if   (v_i<m   ||v_i>q0->getIndex())
               q_he=NULL;
            
            if   (p_he!=NULL)
            {
               //advance ,same to above
               //candidate search given one
//!!!changed. fix bug 1204, it is not same to above, actually, here we don't need to initialize q_he like above
//               tmphe=p_he->next_he;
//               v_i=tmphe->v->getIndex();
//               if   (v_i>p0->getIndex()   ||   v_i<l)
//                  tmphe=NULL;
//replaced with
//since we have checked tmphe->v->index above, we don't need to do it here
               tmphe=p_he;

               while(tmphe!=NULL
                  &&   (vertexToVertexVertex(tmphe->v,p0,q0)==-1))
               {
                  p0=tmphe->v;
                  tmphe=tmphe->inv_he->pre_he;

                  v_i=tmphe->v->getIndex();
                  if   (v_i>p0->getIndex()   ||   v_i<l)
                     tmphe=NULL;
               }
               p_he=tmphe;
            }
         }
         //find it,test whether exists
         tmphe=p0->findOutHalfedgeInRing(q0);
         return   (tmphe==NULL)?addEdge(p0,q0):tmphe->e;
      }
   }
}

EPointer   Mesh::findBridge(WPointer w,bool lowside)
{
   VPointer   p0,q0;
   HePointer   p_he,q_he;
   //get the initial start candidate pair
   // p0 is the lowest vertex in window which lies left to separate line
   findInitialCandidatePair(w,p0,q0,p_he,q_he,lowside);

   return   advancePairToBridge(w,p0,q0,p_he,q_he,lowside);
}

EPointer   Mesh::findNextBaseEdge(WPointer w,EPointer e)
{
   VPointer   p0,p;
   VPointer   q0,q;
   HePointer   p_he,q_he;

   int   m=w->lq->r;
   int   l=w->lq->l;
   int   r=w->rq->r;

   p0=e->he[1]->v;
   q0=e->he[0]->v;
   //set p_he as (p0->p) which is the first halfedge above e
   p_he=e->he[0]->pre_he;
   q_he=e->he[1]->next_he;

   p=p_he->v;
   q=q_he->v;

   VPointer   tmpv;
   HePointer   tmphe;
   //if p0 has only one edge(=e), q0 must have more than one edge 
   //because otherwise low_e==high_e 
   //so we just create new base above it using the safe q
   if   (p_he==e->he[0])
   {
      //find safe q
      tmphe=q_he->inv_he->next_he;
      tmpv=tmphe->v;
      while   (   q_he->e->type!=CONSTRAINED
         &&      (vertexToCircle(tmpv,q,p0,q0)==1)   )
      {
   #ifdef   DEBUG
         assert(q_he->v!=p0); //can't rotate back
   #endif
         deleteEdge(q_he->e);

         q_he=tmphe;
         q=tmpv;
         tmphe=q_he->inv_he->next_he;
         tmpv=tmphe->v;
      }
      return   addEdge(p0,q);
   }

   if   (q_he==e->he[1])
   {
      //find safe p
      tmphe=p_he->inv_he->pre_he;
      tmpv=tmphe->v;
      while   (   p_he->e->type!=CONSTRAINED
         &&      (vertexToCircle(tmpv,p,p0,q0)==1)   )
      {
   #ifdef   DEBUG
         assert(p_he->v!=q0); //can't rotate back
   #endif
         deleteEdge(p_he->e);

         //forward
         p_he=tmphe;
         p=tmpv;
         tmphe=p_he->inv_he->pre_he;
         tmpv=tmphe->v;
      }
      return   addEdge(p,q0);
   }

   // since it would not be the current base edge
   //if p_he->e intersect the middle separate line
   // it must be a constrained base edge
//   if   (p_he->v->getIndex()>=m)   return   (p_he->e);
//   if   (q_he->v->getIndex()<m)      return   (q_he->e);

   int valid;
   //find safe p
   tmphe=p_he->pre_he;
   //tmphe must above p_he
   tmpv=tmphe->v;
   
   //!!!changed 1204
//   if   (   tmpv->getIndex()>=r
   if   (   tmpv->getIndex()>=m
      ||   tmpv->getIndex()<l)
   {
      valid=-1;
   }
   else
   {
      valid=vertexToVertexVertex(tmpv,q0,p0);
   }
#ifdef   DEBUG
   if   (valid==0)   reportError("Collinear degeneracy find",__LINE__,__FILE__);
#endif

   while   (   p_he->e->type!=CONSTRAINED
      &&      valid==1
      &&      (vertexToCircle(tmpv,p,p0,q0)==1)   )
   {
#ifdef   DEBUG
      assert(p_he->v!=q0); //can't rotate back
#endif
      deleteEdge( p_he->e);

      //forward
      p_he=tmphe;
      p=tmpv;
      tmphe=p_he->pre_he;
      tmpv=tmphe->v;
      if   (   tmpv->getIndex()>=r
         ||   tmpv->getIndex()<l)
      {
         valid=-1;
      }
      else
      {
         valid=vertexToVertexVertex(tmpv,q0,p0);
      }
#ifdef   DEBUG
      if   (valid==0)   reportError("Collinear degeneracy find",__LINE__,__FILE__);
#endif
   }

   //find safe q
   tmphe=q_he->next_he;
   tmpv=tmphe->v;

   //!!!changed 1204
   if   (   tmpv->getIndex()>=r
//      ||   tmpv->getIndex()<l)
      ||   tmpv->getIndex()<m)
   {
      valid=-1;
   }
   else
   {
      valid=vertexToVertexVertex(tmpv,q0,p0);
   }
#ifdef   DEBUG
   if   (valid==0)   reportError("Collinear degeneracy find",__LINE__,__FILE__);
#endif
   while   (   q_he->e->type!=CONSTRAINED
      &&      valid==1
      &&      (vertexToCircle(tmpv,q,p0,q0)==1)   )
   {
#ifdef   DEBUG
      assert(q_he->v!=p0); //can't rotate back
#endif
      deleteEdge( q_he->e);

      q_he=tmphe;
      q=tmpv;
      tmphe=q_he->next_he;
      tmpv=tmphe->v;
      if   (   tmpv->getIndex()>=r
         ||   tmpv->getIndex()<l)
      {
         valid=-1;
      }
      else
      {
         valid=vertexToVertexVertex(tmpv,q0,p0);
      }
#ifdef   DEBUG
      if   (valid==0)   reportError("Collinear degeneracy find",__LINE__,__FILE__);
#endif
   }


   //test again to speed up
   // since it would not be the current base edge
   //if p_he->e intersect the middle separate line
   // it must be a constrained base edge
   // the above statement is wrong
//   if   (p_he->v->getIndex()>=m)   return   (p_he->e);
//   if   (q_he->v->getIndex()<m)      return   (q_he->e);

   if   (p==q)
   {
      //if p_he->e intersect the middle separate line
      // it must be a constrained base edge
      return   (p->getIndex()>=m?p_he->e:q_he->e);
   }

   //otherwise compare p,q
   if   (vertexToCircle(p,q,p0,q0)==1)
   {
      //p in O(q,p0,q0), select p
      return   addEdge(p,q0);
   }
   else
   {
      return   addEdge(p0,q);
   }
}

void   Mesh::display()
{
   //display

   VPointer   v1,v2;
   glBegin(GL_LINES);
   //glLineWidth(2.0);

   EListPointer   el;
   el=e_head;
   while   (el!=NULL)
   {
      if   (el->e->type!=VIRTUAL)
      {
         if   (el->e->isExternal)
         {
            glColor3f(1.0,.0,.0);
         }
         else
         {
            glColor3f(1.0,1.0,1.0);
         }

		 if (el->e->he[0]->f == NULL && el->e->he[1]->f == NULL){
			 glColor3f(0,1,0);
		 }
		 
         v1=el->e->he[1]->v;
         v2=el->e->he[0]->v;

         glVertex2d(v1->pos.x,v1->pos.y);
         glVertex2d(v2->pos.x,v2->pos.y);
      }

      el=el->next;
   }
   glEnd();

   glBegin(GL_POINTS);
   glPointSize(2.0);
   glColor3f(.0,1.0,.0);
   for   (int i=0;i<v_num;i++)
      glVertex2d(v_array[i]->pos.x,v_array[i]->pos.y);
   glEnd();
}

void   Mesh::buildCDTOfWindow(WPointer w)
{
   EPointer low_e;
   EPointer high_e;
   bool   lowside=true;

   low_e=findBridge(w,lowside);
   high_e=findBridge(w,!lowside);

   //from low_e, repeatly add base edge until meet high_e
   while   (low_e!=high_e)
   {
      low_e=findNextBaseEdge(w,low_e);
   }
}

void   Mesh::mergeSlabPair(SPointer   ls)
{
   SPointer   rs;
   SPointer   ms;

   QPointer   tmpq;
   QPointer   newq;

   int      m,l,r;   // middle separator line's index

   rs=ls->next;

#ifdef DEBUG
   assert(ls->r==rs->l);
#endif
   
   l=ls->l;
   m=ls->r;
   r=rs->r;

   ms=new Slab(l,r);

   //repeat merging
   while   (ls->q_head!=NULL
      ||   rs->q_head!=NULL)
   {
      //in this loop, all the quads above ls->q_head and rs->q_head has no intersection with these two
      
      //ls has no quad, just update ms with rs->q_head,then delete rs->q_head
      if   (ls->q_head==NULL)
      {
         //forward rs
         tmpq=rs->q_head;
         newq=new Quad(tmpq->top_v,tmpq->btm_v,tmpq->top_he,tmpq->btm_he,l,r);
         newq->top_y=tmpq->top_y;
         newq->btm_y=tmpq->btm_y;

         newq->computeCornorY(sep_array[l],sep_array[r]);

         ms->pushBackQuad(newq);

         rs->q_head=tmpq->next;
         delete tmpq;
      }
      else
      if   (rs->q_head==NULL)
      {
         //forward ls
         tmpq=ls->q_head;
         newq=new Quad(tmpq->top_v,tmpq->btm_v,tmpq->top_he,tmpq->btm_he,l,r);
         newq->top_y=tmpq->top_y;
         newq->btm_y=tmpq->btm_y;

         newq->computeCornorY(sep_array[l],sep_array[r]);

         ms->pushBackQuad(newq);

         ls->q_head=tmpq->next;
         delete tmpq;
      }
      else
      {
         WPointer w;
         bool   lgr;// left quad is above right quad
         w=getWindow(ls->q_head,rs->q_head,lgr);
         if   (w==NULL)
         {
            if   (lgr)
            {
               //forward ls
               tmpq=ls->q_head;
               newq=new Quad(tmpq->top_v,tmpq->btm_v,tmpq->top_he,tmpq->btm_he,l,r);
               newq->top_y=tmpq->top_y;
               newq->btm_y=tmpq->btm_y;

            newq->computeCornorY(sep_array[l],sep_array[r]);

               ms->pushBackQuad(newq);

               ls->q_head=tmpq->next;
               delete tmpq;

            }
            else
            {
               //forward rs
               tmpq=rs->q_head;
               newq=new Quad(tmpq->top_v,tmpq->btm_v,tmpq->top_he,tmpq->btm_he,l,r);
               newq->top_y=tmpq->top_y;
               newq->btm_y=tmpq->btm_y;

            newq->computeCornorY(sep_array[l],sep_array[r]);

               ms->pushBackQuad(newq);

               rs->q_head=tmpq->next;
               delete tmpq;

            }
         }
         else   
         {

            // they dont have to share top_he
            QPointer lq;
            QPointer rq;

            lq=ls->q_head;
            rq=rs->q_head;

            newq=new Quad();
            newq->l=l;
            newq->r=r;
            
            bool lwin;
            switch   (w->upsign)
            {
            case   -1:   
               newq->top_he=lq->top_he;
               newq->top_v=lq->top_v;
               newq->top_y=lq->top_y;
               break;
            case   0:
               newq->top_he=lq->top_he;
               lwin=lq->top_v->pos.y>rq->top_v->pos.y;
               if (lwin)
               {
                  newq->top_v=lq->top_v;
                  newq->top_y=lq->top_y;
               }
               else
               {
                  newq->top_v=rq->top_v;
                  newq->top_y=rq->top_y;
               }
               break;
            case   1:
               newq->top_he=rq->top_he;
               newq->top_v=rq->top_v;
               newq->top_y=rq->top_y;
               break;
            default:break;
            }
            //top_y is relative to top_v

            while   (w!=NULL)
            {
               // repeat until we find the bottom of this composite quad

               //triangulate window
               buildCDTOfWindow(w);

               if   (w->downsign==0)
               {
                  lq=ls->q_head;
                  rq=rs->q_head;

                  newq->btm_he=lq->btm_he;

                  bool rwin=lq->btm_v->pos.y>rq->btm_v->pos.y;
                  if (rwin)
                  {
                     newq->btm_v=rq->btm_v;
                     newq->btm_y=rq->btm_y;
                  }
                  else
                  {
                     newq->btm_v=lq->btm_v;
                     newq->btm_y=lq->btm_y;
                  }

                  //forward rs
                  tmpq=rs->q_head;
                  rs->q_head=tmpq->next;
                  delete tmpq;

                  //forward ls
                  tmpq=ls->q_head;
                  ls->q_head=tmpq->next;
                  delete tmpq;

                  w=NULL;
               }
               else
               {
                  if   (w->downsign==-1)
                  {
                     newq->btm_he=rs->q_head->btm_he;
                     newq->btm_v=rs->q_head->btm_v;
                     newq->btm_y=rs->q_head->btm_y;
                     //forward ls
                     tmpq=ls->q_head;
                     ls->q_head=tmpq->next;
                     delete tmpq;
                     if   (ls->q_head==NULL)
                     {
                        tmpq=rs->q_head;
                        rs->q_head=tmpq->next;
                        delete tmpq;
                        w=NULL;
                     }
                     else
                     {
                        w=getWindow(ls->q_head,rs->q_head,lgr);
                        if   (w==NULL)
                        {
                           tmpq=rs->q_head;
                           rs->q_head=tmpq->next;
                           delete tmpq;
                        }
                     }
                  }
                  else
                  {
                     newq->btm_he=ls->q_head->btm_he;
                     newq->btm_v=ls->q_head->btm_v;
                     newq->btm_y=ls->q_head->btm_y;
                     //forward rs
                     tmpq=rs->q_head;
                     rs->q_head=tmpq->next;
                     delete tmpq;
                     if   (rs->q_head==NULL)
                     {
                        tmpq=ls->q_head;
                        ls->q_head=tmpq->next;
                        delete tmpq;
                        w=NULL;
                     }
                     else
                     {
                        w=getWindow(ls->q_head,rs->q_head,lgr);
                        if   (w==NULL)
                        {
                           tmpq=ls->q_head;
                           ls->q_head=tmpq->next;
                           delete tmpq;
                        }
                     }
                  }
               }
            }
            newq->computeCornorY(sep_array[l],sep_array[r]);

            ms->pushBackQuad(newq);
         }
         //in this loop, all the quads above ls->q_head and rs->q_head has no intersection with these two
      }
   }
   //copy ms to ls
   ls->r=ms->r;
   ls->q_head=ms->q_head;
   ls->q_tail=ms->q_tail;
   //delete ms
   delete ms;
   //for
   ls->next=rs->next;
   //delete rs
   delete   rs;
}

/* return 0 means no error*/
int   Mesh::mergeSlabs()
{

   SPointer   tmps;
   // merge the list until there is only one slab 
#ifdef   OUTPUT
         printf("\nCurrent slab list:\n");
         printSlab(s_head);
#endif
   while   (s_head->next!=NULL)
   {
      tmps=s_head;
      // one scan of list, merging each two consecutive slabs
      while(tmps!=NULL   && tmps->next!=NULL)
      {
         mergeSlabPair(tmps);
         // now the ts is already the merged slab
         tmps=tmps->next;

#ifdef   OUTPUT
         printf("\nCurrent slab list:\n");
         printSlab(s_head);
#endif
      }
   }   

#ifdef   OUTPUT
         printf("\n The final slab:\n");
         printSlab(s_head);
         printf("\n");
#endif
   //...
   return 0;
}

/* return 0 means no error*/
int   Mesh::buildCDT()
{
   initSlabs();
   return mergeSlabs();
}

MeshPointer   Mesh::copy()
{
   MeshPointer m=new Mesh();
   m->maxs.x=maxs.x;
   m->maxs.y=maxs.y;
   m->mins.x=mins.x;
   m->mins.y=mins.y;

   if   (vlt!=NULL)
   {
      m->vlt=new Vertex(mins.x-3*OFFSET,maxs.y+OFFSET,-2);
      m->vlb=new Vertex(mins.x-2*OFFSET,mins.y-OFFSET,-1);

      //index is v_num+1,v_num
      m->vrt=new Vertex(maxs.x+3*OFFSET,maxs.y+OFFSET,v_num+1);
      m->vrb=new Vertex(maxs.x+2*OFFSET,mins.y-OFFSET,v_num);

      // 2 long horizontal lines
      sky_e=addEdge(vlt,vrt,VIRTUAL);
      eth_e=addEdge(vlb,vrb,VIRTUAL);
   }

   m->v_num=this->v_num;
   m->v_array=new VPointer[v_num];
   for   (int vi=0;vi<v_num;vi++)
   {
      m->v_array[vi]=new   Vertex(this->v_array[vi]->pos.x,this->v_array[vi]->pos.y,vi);
   }

   if   (sep_array!=NULL)
   {
      m->sep_array=new double[v_num+1];
      for   (int si=0;si<v_num;si++)
      {
         m->sep_array[si]=this->sep_array[si];
      }
      m->sep_array[v_num]=sep_array[v_num];
   }


   EListPointer   el;
   int   i1,i2;
   el=e_head;
   while   (el!=NULL)
   {
      if   (el->e->type!=VIRTUAL)
      {
         i1=el->e->he[1]->v->getIndex();
         i2=el->e->he[0]->v->getIndex();
         m->addEdge(m->v_array[i1],m->v_array[i2],el->e->type);
      }
      el=el->next;
   }
   return   m;
}