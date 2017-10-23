#pragma once

//class MyPolyline3f;
#include "MyArray.h"
class MyGraphLayout;

class MyGraphEdgeBundler
{
public:
	MyGraphEdgeBundler(void);
	~MyGraphEdgeBundler(void);

	virtual void SetGraphLayout(const MyGraphLayout* layout);

	virtual const MyArray<MyArray3f>* GetControPoints() const;

	virtual void Update();
	virtual void ShowDebug() const{};

	virtual void SmoothControlPoints(int numPoints, float strength = 0.8f);

protected:
	const MyGraphLayout* mLayout;
	// control points for each edge
	MyArray<MyArray3f> mControPoints;

	//MyArray<MyPolyline3f*> mEdgeCurves;

};

