#pragma once

#include "MyGenericGraph.h"
#include "MyArray.h"
#include "MyMatrix.h"

class MyGraphAlgorithm
{
public:
	MyGraphAlgorithm(void);
	~MyGraphAlgorithm(void);

	void SetGraph(const MyGenericGraphf* graph);
	void FloydWarshallWithPathReconstruction();
	MyArrayi GetShortestPath(int i, int j) const;
	float GetShortestPathLength(int i, int j) const;
	MyArrayf GetBetweennessCentrality() const;
	MyArrayf GetDegreeCentrality() const;
	MyArrayf GetEigenvectorCentrality() const;
	MyArrayi GetTopDegreeNodes(int num2get) const;
	void SetAsBinaryGraph(bool b = true);
	bool IsBinary();

	MyGenericGraphf* MakeDifferenceGraph(const MyGenericGraphf* newGraph) const;
protected:

	void initNextArray();
	const MyGenericGraphf* mGraph;
	MyMatrixf mDistance;
	MyMatrixi mNext;

	int mFlag;
};

