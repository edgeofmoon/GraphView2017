#pragma once

#include "MyGenericGraph.h"
#include "MyArray.h"

class MyGraphFilter
{
public:
	MyGraphFilter(void);
	~MyGraphFilter(void);

	void SetInputGraph(const MyGenericGraphf* graph);

	// remember to free/delete the new graph!
	// memory leak alert!
	MyGenericGraphf* MakeFilteringByEdgeWeight(float minWeight, float maxWeight);
	MyGenericGraphf* MakeFilteringByEdgeWeightRatio(float ratioleft);

	void GetNodesByEdgeWeight(std::vector<int>& nodeIdx, float minWeight, float maxWeight) const;
	void GetEdgesByEdgeWeight(std::vector<int>& edgeIdx, float minWeight, float maxWeight) const;
	const int GetInputNodeIndex(int outputNodeIndex) const;
	const int GetOutputNodeIndex(int inputNodeIndex) const;
	MyArray<MyGenericGraphf*>* MakeGraphsByHierarchy(const MyGenericNode* hierarchyRoot, MyArray<MyArray<MyArrayi>>* nodesLod = 0) const;

protected:
	const MyGenericGraphf* mInGraph;

	// only stores the latest filtering
	std::vector<int> mNewToOldNodeIndexMap;
};

