#pragma once
#include "mygraphrepresentation.h"
#include "MyGenericNode.h"
#include "My1DSampler.h"
#include "MyColor4.h"

#include <vector>
class MyGraphHierarchicalRepresentation :
	public MyGraphRepresentation
{
public:
	MyGraphHierarchicalRepresentation(void);
	~MyGraphHierarchicalRepresentation(void);

	virtual void SetHierarchy(const MyGenericNode* hierarchyRoot);

	virtual void ShowNode(int i);
	virtual void ShowEdge(int i);
	virtual void ShowHighlightedNode(int i);
	virtual void ShowHighlightedEdge(int i);

protected:
	const MyGenericNode* mHierarchyRoot;

	void updateNodeClusterIndex();
	std::vector<int> mClusterIndices;

	void buildClusterColors();
	My1DSampler<MyColor4f> mClusterColors;
	int mNumClusters;
};

