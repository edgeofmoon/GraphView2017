#pragma once

#include "MyGenericGraph.h"
#include "MyGenericLayout.h"
#include "MyGraphEdgeBundler.h"

class MyGraphLayout
	:public MyGenericLayout
{
public:
	MyGraphLayout(void);
	~MyGraphLayout(void);

	virtual void SetGraph(const MyGenericGraphf* graph);
	virtual void SetEdgeBundler(MyGraphEdgeBundler* edgeBundler);
	virtual void SetEdgeBundle(bool bundle = true);
	virtual bool GetEdgeBundle() const;

	virtual void Update();

	virtual const MyGenericGraphf* GetGraph() const;
	virtual const MyGraphEdgeBundler* GetEdgeBundler() const;
	
	
	virtual MyArray3f* MakeCLusterCenterArray(const MyArray<MyArrayi>& clusterSet) const;

	void SetFlat(bool f = true);
protected:
	// weight directional graph
	const MyGenericGraphf* mGraph;
	
	// use 2D or 3D
	bool mFlat;
	virtual void flatThings();

	MyGraphEdgeBundler* mEdgeBundler;
	bool mBundleEdge;
};

