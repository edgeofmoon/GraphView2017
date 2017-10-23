#pragma once
#include "MyGraphHierarchicalLayout.h"
#include "MyArray.h"

class MyGraphRadialLayout :
	public MyGraphHierarchicalLayout
{
public:
	MyGraphRadialLayout(void);
	~MyGraphRadialLayout(void);
	
	virtual void Update();
	
	virtual MyArray3f* MakeCLusterCenterArray(const MyArray<MyArrayi>& clusterSet) const;

	virtual void SetNodeSize(MyArrayf sizes);
protected:
	
	// MyVec3f(radius, startRadian, endRadian)
	virtual void layoutSubHierarchy(const MyGenericNode* hierarchy, MyBoundingBox boundingBox);

	float mLayerRadius;
	float mOverallRadius;

	bool mFixedOverallRadius;

	MyArray3f mParentPos;
	MyArrayf mNodeSizes;
};

