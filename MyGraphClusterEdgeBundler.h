#pragma once
#include "mygraphedgebundler.h"
#include "MyGenericNode.h"

class MyGraphClusterEdgeBundler :
	public MyGraphEdgeBundler
{
public:
	MyGraphClusterEdgeBundler(void);
	~MyGraphClusterEdgeBundler(void);

	void SetHierarchy(const MyGenericNode* hierarchy);
	void SetUseLeastCommomAncestor(bool use=true);

	virtual void Update();

protected:
	const MyGenericNode* mHierarchy;
	int mNumControlPoints;
	float mStiffness;
	bool mUseLeastCommonAncestor;
};