#define DEBUG
#ifdef DEBUG
#include <iostream>
#include <vector>
using namespace std;
#endif
#include <cassert>
#include <gl/glut.h>
#include "myMesh.h"

#define HALFEDGE_DIRECTION 0
#define HALFEDGE_DIRECTION_REVERSE 1

myMesh::myMesh(void)
{
}

myMesh::~myMesh(void)
{
	clear();
}

void myMesh::loadPoints( vector<MyVec2f> &points )
{
	if(points.size()<3)
	{
		reportError("Cannot build mesh!",__LINE__,__FILE__);
	}
	else
	{ 
		int   fnum = 0;
		int   vnum = points.size();
		if   (vnum<1)
		{
			reportError("Vertex number should above zero.",__LINE__,__FILE__);
		}

		v_array=new VPointer[vnum];

		double   x;
		double   y;

		//init 
		mins.x=mins.y=HUGE_VAL;
		maxs.x=maxs.y=-HUGE_VAL;

		// read information of vertices
		for (int i=0;i<vnum;i++)
		{
			x = points[i][0];
			y = points[i][1];

			maxs.x=MAX(maxs.x,x);
			maxs.y=MAX(maxs.y,y);
			mins.x=MIN(mins.x,x);
			mins.y=MIN(mins.y,y);

			v_array[i]=new Vertex(x,y,i,i);
			v_num++;
		}

		//vnum should eq v_num now

		//sort the vertex
		sortVertices();

		//get the information of sorting
		vector<int> reorder(v_num, -1);
		for   (int ni=0;ni<v_num;ni++)
		{
			reorder[v_array[ni]->getIndex()]=ni;
		}

		//update the information of vertices;
		for   (int nj=0;nj<v_num;nj++)
		{
			v_array[nj]->setIndex(nj);
		}

		
	
		// newly add, force consective points to form lines
		for(int i = 0;i<points.size();i+=2){
			/*
			int start = -1, end = -1;
			for(int j = 0;j<points.size();j++){
				if(v_array[j]->pos.x == points[i][0] && v_array[j]->pos.y == points[i][1]){
					start = j;
					break;
				}
			}
			for(int j = 0;j<points.size();j++){
				if(v_array[j]->pos.x == points[i+1][0] && v_array[j]->pos.y == points[i+1][1]){
					end = j;
					break;
				}
			}
			if(v_array[start]->pos.x > v_array[end]->pos.x){
				int tmp = end;
				end = start;
				start = tmp;
			}
			assert(start>=0 && end>=0);
			this->addEdge(v_array[start], v_array[end], VIRTUAL);
			*/
			//this->addEdge(v_array[reorder[i]], v_array[reorder[i+1]],CONSTRAINED);

		}
	}

}

void myMesh::clear()
{
	if (NULL != v_array){
		delete [] v_array;
	}
	if (NULL != sep_array){
		delete [] sep_array;
	}
	e_head=e_tail=NULL;
	v_num=e_num=ce_num=0;
	maxs.y=maxs.x=HUGE_VAL;
	mins.y=mins.x=-HUGE_VAL;
	v_array=NULL;
	sep_array=NULL;

	for (int i = 0;i<faceVector.size();i++){
		delete faceVector[i];
	}
	faceVector.clear();
}

int numIntersections(MyVec2f start, MyVec2f end,vector<MyVec2f> &points){
	int its = 0;
	int n = points.size();
	for (int i = 1;i<n;i++){
		if (MyVec2f::Intersected(start,end,points[i-1],points[i])){
			its++;
		}
	}
	if (MyVec2f::Intersected(start,end,points[n-1],points[0])){
		its++;
	}
	return its;
}

#define BIGLENGTH 99999
bool isPointInside(MyVec2f p,vector<MyVec2f> &points){
	if (points.size()<3){
		return false;
	}
	MyVec2f midEdge = (points[0]+points[1])/2;
	float length = (midEdge-p).norm();
	if (length == 0){
		return true;
	}
	MyVec2f infinite = p+(midEdge-p)/length*BIGLENGTH;
	int nit = numIntersections(p,infinite,points);
	if (0 == nit%2){
		return false;
	}
	else return true;
}
#undef BIGLENGTH

void myMesh::tagExternalAndDeleteOutsideEdges( vector<MyVec2f> &points )
{
	EListPointer   el;
	el=e_head;
	vector<EPointer> outEdges;
	while   (el!=NULL)
	{
		if   (el->e->type!=VIRTUAL )
		{
			EPointer t = el->e;
			VPointer v1 = el->e->he[1]->v;
			VPointer v2 = el->e->he[0]->v;
			int extIdx1 = v1->extIdx;
			int extIdx2 = v2->extIdx;
			int lastExtIdx = points.size()-1;
			if (abs(extIdx1-extIdx2) != 1 && !((extIdx1 == 0 && extIdx2 == lastExtIdx)||(extIdx2 == 0 && extIdx1 == lastExtIdx))){
				Point p1 = v1->pos;
				Point p2 = v2->pos;
				MyVec2f midP((p1.x+p2.x)/2,(p1.y+p2.y)/2);
				bool bInside = isPointInside(midP,points);
				if (!bInside){
					outEdges.push_back(t);
				}
			}
			else{
				t->isExternal = true;
			}
			el=el->next;
		}
	}

	for (int i = 0;i<outEdges.size();i++){
		deleteEdge(outEdges[i]);
	}
}

void myMesh::buildFaces()
{
	EListPointer   el;
	el=e_head;
	vector<EPointer> outEdges;
	while   (el!=NULL){
		EPointer e = el->e;
		if ( !e->isExternal && e->type != VIRTUAL){
			if (!hasFaceWithHalfEdge(el->e->he[HALFEDGE_DIRECTION])){
				faceVector.push_back(new myFace(e->he[HALFEDGE_DIRECTION]));
			}
			if (!hasFaceWithHalfEdge(el->e->he[HALFEDGE_DIRECTION_REVERSE])){
				faceVector.push_back(new myFace(e->he[HALFEDGE_DIRECTION_REVERSE]));
			}
		}
		el = el->next;
	}
#ifdef DEBUG
	cout << "[DEBUG]" << faceVector.size() << " faces." << endl;
	int numTer = 0;
	int numJunc = 0;
	int numSle = 0;
	for (int i = 0;i<faceVector.size();i++){
		switch(faceVector[i]->type){
			case FACE_TERMINAL:
				numTer++;
				break;
			case FACE_JUNCTION:
				numJunc++;
				break;
			case FACE_SLEEVE:
				numSle++;
				break;
		}
		cout << "[DEBUG] the " << i<<"th face has " << faceVector[i]->getEdgeNumber() << " edges." << endl;
	}
	cout << "[DEBUG]" << numTer << " terminal faces." << endl;
	cout << "[DEBUG]" << numJunc << " junction faces." << endl;
	cout << "[DEBUG]" << numSle << " sleeve faces." << endl;
#endif
}

bool myMesh::hasFace( const FPointer fp )
{
	for (int i = 0;i<faceVector.size();i++){
		if (fp ==faceVector[i] ){
			return true;
		}
	}
	return false;
}

bool myMesh::hasFaceWithHalfEdge( const HePointer hep )
{
	for (int i = 0;i<faceVector.size();i++){
		if (faceVector[i]->hasHalfEdge(hep) ){
			return true;
		}
	}
	return false;
}

void myMesh::connectHalfEdgesToFaces() const
{
	for (int i = 0;i<faceVector.size();i++){
		faceVector[i]->connectHalfEdges();
	}
}

void myMesh::reconnectHalfEdges()
{
	EListPointer   el;
	el=e_head;
	vector<EPointer> outEdges;
	while   (el!=NULL){
		EPointer e = el->e;
		el = el->next;
	}
}

myFace::myFace(HePointer _he)
{
	he = _he;
	type = getTypeByHalfEdge(he);
}

bool myFace::equals( const FPointer other ) const
{
	return other->hasHalfEdge(he);
}

bool myFace::hasHalfEdge( const HePointer hed ) const
{
	HePointer next = he;
	do {
		if (next == hed){
			return true;
		}
		next = next->next_he;
	} while (next != he);
	return false;
}

int myFace::getTypeByHalfEdge( const HePointer _he ) const
{
	HePointer next = _he;
	int numExt = 0;
	if (NULL != next){
		do {
			if (next->e->isExternal){
				numExt++;
			}
			next = next->next_he;
		} while (next != _he);

		switch(numExt){
			case 0:
				return FACE_JUNCTION;
			case 1:
				return FACE_SLEEVE;
			case 2:
				return FACE_TERMINAL;
			default:
				return FACE_INVALID;
		}
	}
	else{
		return FACE_INVALID;
	}
}

void myFace::connectHalfEdges()
{
	HePointer next = he;
	if (NULL != next){
		do {
			next->e->he[HALFEDGE_DIRECTION]->f = this;
			next = next->next_he;
		} while (next != he);
	}
}

int myFace::getEdgeNumber() const
{
	int n = 0;
	HePointer next = he;
	if (NULL != next){
		do {
			n++;
			next = next->next_he;
		} while (next != he);
	}
	return n;
}

void myFace::display()
{
	glBegin(GL_LINE_LOOP);
	HePointer next = he;
	if (NULL != next){
		do {
			glVertex2f(next->v->pos.x,next->v->pos.y);
			next = next->next_he;
		} while (next != he);
	}
	glEnd();
}