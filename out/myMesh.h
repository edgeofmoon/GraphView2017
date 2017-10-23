#pragma once
#include "cdt.h"
#include "../MyVec.h"

#include <vector>
using namespace std;

#define FACE_INVALID -1
#define FACE_JUNCTION 0
#define FACE_SLEEVE 1
#define FACE_TERMINAL 2

class myFace{
public:
	myFace(HePointer _he = NULL);

	// any of the halfEdges
	HePointer he;
	int type;

	int getEdgeNumber() const;
	int getTypeByHalfEdge( const HePointer _he) const;
	bool equals(const FPointer other) const;
	bool hasHalfEdge(const HePointer hed) const;
	void connectHalfEdges();
	void display();
};

class myMesh :
	public Mesh
{
public:
	myMesh(void);
	~myMesh(void);

	vector<FPointer> faceVector;

	void clear();
	void loadPoints(vector<MyVec2f> &points);
	void reconnectHalfEdges();
	void tagExternalAndDeleteOutsideEdges(vector<MyVec2f> &points);
	void buildFaces();
	void connectHalfEdgesToFaces() const;
	bool hasFace(const FPointer fp);
	bool hasFaceWithHalfEdge( const HePointer hep);
};
