#pragma once

#include "MyArray.h"

class MyGenericNode
{
public:
	MyGenericNode(MyGenericNode* parent = 0);
	virtual ~MyGenericNode(void);
	
	virtual MyGenericNode* MakeCopy() const;
	virtual void CopyAll(const MyGenericNode* node);
	virtual void CopyAscendants(const MyGenericNode* node);
	virtual void CopyDescendants(const MyGenericNode* node);
	virtual void CopyAuxiliaryContent(const MyGenericNode* node);
	virtual MyGenericNode* MakeNew() const;

	virtual int GetNumChildren() const{
		return mChildren.size();
	};
	virtual int GetNumParents() const{
		return mParents.size();
	}
	
	virtual int GetNumLeaves() const;

	virtual const MyGenericNode* GetChild(int i)const {
		return mChildren[i];
	}

	const MyGenericNode* GetFirstOrigin() const{
		const MyGenericNode* parent = this;
		while(parent->HasAnyParent()){
			parent = parent->GetParent();
		}
		return parent;
	}

	virtual const MyGenericNode* GetParent(int i = 0)const {
		return mParents[i];
	}

	virtual MyGenericNode* GetChild(int i){
		return mChildren[i];
	}
	virtual MyGenericNode* GetParent(int i = 0){
		return mParents[i];
	}

	virtual bool HasAnyChild() const{
		return !mChildren.empty();
	}
	
	virtual bool HasAnyParent() const{
		return !mParents.empty();
	}

	virtual bool HasDescendantNode(const MyGenericNode* node) const;
	virtual bool CutDescendantNode(MyGenericNode* node);
	// in order to destory all, there must be a root
	// all ascendants can only have child that is the
	// direct ascendant of this node; all descendants
	// must be the direct descendant of this node

	// in short, this node must be the root of both
	// ascendant tree and descendant tree
	virtual void DestoryAll();
	virtual void DestoryAscendants();
	virtual void DestoryDescendants();
	virtual void TraverseDescendants(void(*func)(MyGenericNode*));
	virtual void TraverseDescendants(void(*func)(const MyGenericNode*)) const;

	// for tree
	virtual int GetHeight() const;
	virtual int GetDepth() const;

	static int Connect(MyGenericNode* parent,
		MyGenericNode* child);
	static int Disconnect(MyGenericNode* parent,
		MyGenericNode* child);

	// for debug use
	static int GetNumNodeInTotal(){
		return mNumNodeInTotal;
	}

	MyArray<const MyGenericNode*>* MakeArrayWithCutLeaveNoDeeperThan(int depth) const;
protected:
	
	MyArray<MyGenericNode*> mParents;
	MyArray<MyGenericNode*> mChildren;

	int addChild(MyGenericNode* child);
	int addParent(MyGenericNode* parent);
	int removeChild(MyGenericNode* child);
	int removeParent(MyGenericNode* parent);

	static int mNumNodeInTotal;

private:

	static int mDepth;
	static MyArray<const MyGenericNode*> mArray;
	static void addDepth(const MyGenericNode* node);
};

