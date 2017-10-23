#pragma once
#include "mygraphhierarchicallayout.h"

// only 2 layers now
class MyGraphProjectionLayout :
	public MyGraphHierarchicalLayout
{
public:
	MyGraphProjectionLayout(void);
	~MyGraphProjectionLayout(void);

protected:
	
	// only called from root node
	virtual void layoutSubHierarchy(const MyGenericNode* hierarchy, MyBoundingBox boundingBox);

	MyArray<MyBoundingBox> mClusterBoundingBox;
	float mBoarderSize;
};

