#pragma once

#include "MyGraphLayout.h"
#include "MyGenericNode.h"


class MyGraphHierarchicalLayout
	: public MyGraphLayout
{
public:
	MyGraphHierarchicalLayout();
	~MyGraphHierarchicalLayout();

	virtual void SetHierarchy(const MyGenericNode* hierarchyRoot);
	virtual void Update();


protected:
	const MyGenericNode* mHierarchyRoot;
	
	virtual void flatThings();
	virtual void layoutSubHierarchy(const MyGenericNode* hierarchy, MyBoundingBox boundingBox);
};

