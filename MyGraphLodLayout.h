#pragma once
#include "mygraphlayout.h"
#include "MyArray.h"

class MyGraphLodLayout :
	public MyGraphLayout
{
public:
	MyGraphLodLayout(void);
	~MyGraphLodLayout(void);
	
	void SetLevelOfDetailIndex(int i);
	int GetLevelOfDetailIndex() const;
	int GetNumLevelOfDetials() const;

	MyArray<const MyGenericNode*>* MakeHierarchyOverlay() const;
	const MyGenericNode* GetHierarchy() const{
		return mHierarchyRoot;
	}
	
	virtual void Update();
	
	virtual void BuildLodByHierarchy(const MyGenericNode* hierarchyRoot);

	
	virtual const MyGenericGraphf* GetGraph() const;

protected:
	MyArray<MyGenericGraphf*> mGraphs;
	MyArray<MyArray3f*> mPoses;
	MyArray<MyArray<MyArrayi>> mNodesLod;

	const MyGenericNode* mHierarchyRoot;
	int mLodIndex;

private:
	static int mDepthToAdd;
	static MyArray<const MyGenericNode*>* mArray;
	static void AddDepthToArray(const MyGenericNode* node);
};

