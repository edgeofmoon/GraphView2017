#pragma once

#include "MyPrimitiveDrawer.h"

#include <vector>

class MyGraphDrawer
	:public MyPrimitiveDrawer
{
public:
	MyGraphDrawer(void);
	~MyGraphDrawer(void);
	
	static void DrawEdgeAt(const MyVec3f& s, const MyVec3f& e);
	static void DrawNodeAt(const MyVec3f& n, float r = 1.f);

	enum MyGraphComponent{
		BOUNDINGBOX = 3
	};

	static bool InterpretHit(std::vector<MyVec2i>& see, unsigned int *buffer, int nHits);
	static bool InterpretHit(std::vector<int>& nodeList, std::vector<int>& edgeList, std::vector<int>& maskList, unsigned int *buffer, int nHits);
	static bool HitAny(unsigned int *buffer, int nHits);
};

