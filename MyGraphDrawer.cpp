#include "MyGraphDrawer.h"
#include "MyGenericGraph.h"
#include <cassert>

MyGraphDrawer::MyGraphDrawer(void)
{
}


MyGraphDrawer::~MyGraphDrawer(void)
{
}

void MyGraphDrawer::DrawEdgeAt(const MyVec3f& s, const MyVec3f& e){
	// for now line is sufficient
	MyPrimitiveDrawer::DrawLineAt(s,e);
}

void MyGraphDrawer::DrawNodeAt(const MyVec3f& n, float r){
	// for now just a sphere
	MyPrimitiveDrawer::DrawSphereAt(n, r);
}

bool MyGraphDrawer::InterpretHit(std::vector<MyVec2i>& see, unsigned int *buffer, int nHits){
	int idx = 0;
	bool seeAny = false;
	for (int i = 0; i < nHits; i++) { /*  for each hit  */
		if(buffer[idx] == 0) {
			idx+=3;
			continue;
		};
		assert(buffer[idx] == 2);
		idx++;
		float zNear = buffer[idx++];
		float zFar = buffer[idx++];
		MyVec2i hit;
		hit[0] = buffer[idx++];
		hit[1] = buffer[idx++];
		see.push_back(hit);
		seeAny = true;
	}
	return seeAny;
}

bool MyGraphDrawer::InterpretHit(std::vector<int>& nodeList, std::vector<int>& edgeList, std::vector<int>& maskList, unsigned int *buffer, int nHits){
	std::vector<MyVec2i> see;
	MyGraphDrawer::InterpretHit(see, buffer, nHits);
	for (int i = 0; i < see.size(); i++){
		switch (see[i][0]){
		case MyGenericGraphf::GRAPH_EDGE:
			edgeList.push_back(see[i][1]);
			break;
		case MyGenericGraphf::GRAPH_NODE:
			nodeList.push_back(see[i][1]);
			break;
		case MyGraphDrawer::BOUNDINGBOX:
			maskList.push_back(see[i][1]);
			break;
		default:
			// some problem here
			return false;
		}
	}
	return !nodeList.empty() || !edgeList.empty() ||
		!maskList.empty();
}

bool MyGraphDrawer::HitAny(unsigned int *buffer, int nHits){
	if (nHits <= 0) return false;
	return buffer[0] > 0;
}